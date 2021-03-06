#include "../precompiled.hpp"
#include "friend_box_map.hpp"
#include "../mmain.hpp"
#include <poseidon/singletons/timer_daemon.hpp>
#include <poseidon/multi_index_map.hpp>
#include <poseidon/job_promise.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/event_dispatcher.hpp>
#include "../events/account.hpp"
#include "../friend_box.hpp"
#include "../mongodb/friend.hpp"
#include "account_map.hpp"
#include "../account.hpp"

namespace EmperyCenter {
	namespace {
		struct FriendBoxElement {
			AccountUuid account_uuid;
			std::uint64_t unload_time;

			mutable boost::shared_ptr<const Poseidon::JobPromise> promise;
			mutable boost::shared_ptr<std::vector<boost::shared_ptr<MongoDb::Center_Friend>>> sink;

			mutable boost::shared_ptr<FriendBox> friend_box;
			mutable boost::shared_ptr<Poseidon::TimerItem> timer;

			FriendBoxElement(AccountUuid account_uuid_, std::uint64_t unload_time_)
				: account_uuid(account_uuid_), unload_time(unload_time_)
			{
			}
		};

		MULTI_INDEX_MAP(FriendBoxContainer, FriendBoxElement,
			UNIQUE_MEMBER_INDEX(account_uuid)
			MULTI_MEMBER_INDEX(unload_time)
		)

			boost::weak_ptr<FriendBoxContainer> g_friend_box_map;

		void gc_timer_proc(std::uint64_t now) {
			PROFILE_ME;
			LOG_EMPERY_CENTER_TRACE("Friend box gc timer: now = ", now);

			const auto friend_box_map = g_friend_box_map.lock();
			if (!friend_box_map) {
				return;
			}

			for (;;) {
				const auto it = friend_box_map->begin<1>();
				if (it == friend_box_map->end<1>()) {
					break;
				}
				if (now < it->unload_time) {
					break;
				}

				// 判定 use_count() 为 0 或 1 的情况。参看 require() 中的注释。
				if ((it->promise.use_count() | it->friend_box.use_count()) != 1) { // (a > 1) || (b > 1) || ((a == 0) && b == 0))
					friend_box_map->set_key<1, 1>(it, now + 1000);
				}
				else {
					LOG_EMPERY_CENTER_DEBUG("Reclaiming friend box: account_uuid = ", it->account_uuid);
					friend_box_map->erase<1>(it);
				}
			}

			/*
			Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_Friend",
				"DELETE QUICK `f`.*"
				"  FROM `Center_Friend` AS `f` "
				"  WHERE `f`.`category` = 0");
			*/
			
			Poseidon::MongoDb::BsonBuilder query;
			query.append_unsigned(sslit("category"), 0);
		    Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_Friend", query, true);
			
		}

		boost::weak_ptr<std::vector<AccountUuid>> g_random_pool;

		MODULE_RAII_PRIORITY(handles, 5000) {
			const auto friend_box_map = boost::make_shared<FriendBoxContainer>();
			g_friend_box_map = friend_box_map;
			handles.push(friend_box_map);

			const auto random_pool = boost::make_shared<std::vector<AccountUuid>>();
			g_random_pool = random_pool;
			handles.push(random_pool);

			const auto gc_interval = get_config<std::uint64_t>("object_gc_interval", 300000);
			auto timer = Poseidon::TimerDaemon::register_timer(0, gc_interval,
				std::bind(&gc_timer_proc, std::placeholders::_2));
			handles.push(timer);

			auto listener = Poseidon::EventDispatcher::register_listener<Events::AccountInvalidate>(
				[](const boost::shared_ptr<Events::AccountInvalidate> &event) { FriendBoxMap::unload(event->account_uuid); });
			handles.push(listener);
		}
	}

	boost::shared_ptr<FriendBox> FriendBoxMap::get(AccountUuid account_uuid) {
		PROFILE_ME;

		const auto friend_box_map = g_friend_box_map.lock();
		if (!friend_box_map) {
			LOG_EMPERY_CENTER_WARNING("FriendBoxMap is not loaded.");
			return{};
		}

		auto it = friend_box_map->find<0>(account_uuid);
		if (it == friend_box_map->end<0>()) {
			it = friend_box_map->insert<0>(it, FriendBoxElement(account_uuid, 0));
		}
		if (!it->friend_box) {
			if (!AccountMap::is_holding_controller_token(account_uuid)) {
				LOG_EMPERY_CENTER_DEBUG("Failed to acquire controller token: account_uuid = ", account_uuid);
				return{};
			}

			LOG_EMPERY_CENTER_DEBUG("Loading friend box: account_uuid = ", account_uuid);

			boost::shared_ptr<const Poseidon::JobPromise> promise_tack;
			do {
				if (!it->promise) {
					auto sink = boost::make_shared<std::vector<boost::shared_ptr<MongoDb::Center_Friend>>>();
					Poseidon::MongoDb::BsonBuilder query;
					query.append_uuid(sslit("account_uuid"), account_uuid.get());
					auto promise = Poseidon::MongoDbDaemon::enqueue_for_batch_loading(
						[sink](const boost::shared_ptr<Poseidon::MongoDb::Connection> &conn) {
						auto obj = boost::make_shared<MongoDb::Center_Friend>();
						obj->fetch(conn);
						obj->enable_auto_saving();
						sink->emplace_back(std::move(obj));
					}, "Center_Friend", std::move(query), 0, INT32_MAX);
					it->promise = std::move(promise);
					it->sink = std::move(sink);
				}
				// 复制一个智能指针，并且导致 use_count() 增加。
				// 在 GC 定时器中我们用 use_count() 判定是否有异步操作进行中。
				promise_tack = it->promise;
				Poseidon::JobDispatcher::yield(promise_tack, true);
			} while (promise_tack != it->promise);

			if (it->sink) {
				friend_box_map->set_key<0, 1>(it, 0);
				LOG_EMPERY_CENTER_DEBUG("Async MySQL query completed: account_uuid = ", account_uuid, ", rows = ", it->sink->size());

				auto friend_box = boost::make_shared<FriendBox>(account_uuid, *(it->sink));

				const auto friend_box_refresh_interval = get_config<std::uint64_t>("friend_box_refresh_interval", 60000);
				auto timer = Poseidon::TimerDaemon::register_timer(0, friend_box_refresh_interval,
					std::bind([](const boost::weak_ptr<FriendBox> &weak) {
					PROFILE_ME;
					const auto friend_box = weak.lock();
					if (!friend_box) {
						return;
					}
					friend_box->pump_status();
				}, boost::weak_ptr<FriendBox>(friend_box))
				);

				it->promise.reset();
				it->sink.reset();
				it->friend_box = std::move(friend_box);
				it->timer = std::move(timer);
			}

			assert(it->friend_box);
		}

		const auto now = Poseidon::get_fast_mono_clock();
		const auto gc_interval = get_config<std::uint64_t>("object_gc_interval", 300000);
		friend_box_map->set_key<0, 1>(it, saturated_add(now, gc_interval));

		return it->friend_box;
	}
	boost::shared_ptr<FriendBox> FriendBoxMap::require(AccountUuid account_uuid) {
		PROFILE_ME;

		auto ret = get(account_uuid);
		if (!ret) {
			LOG_EMPERY_CENTER_WARNING("Friend box not found: account_uuid = ", account_uuid);
			DEBUG_THROW(Exception, sslit("Friend box not found"));
		}
		return ret;
	}
	void FriendBoxMap::unload(AccountUuid account_uuid) {
		PROFILE_ME;

		const auto friend_box_map = g_friend_box_map.lock();
		if (!friend_box_map) {
			LOG_EMPERY_CENTER_WARNING("FriendBoxMap is not loaded.");
			return;
		}

		const auto it = friend_box_map->find<0>(account_uuid);
		if (it == friend_box_map->end<0>()) {
			LOG_EMPERY_CENTER_DEBUG("Friend box not loaded: account_uuid = ", account_uuid);
			return;
		}

		friend_box_map->set_key<0, 1>(it, 0);
		it->promise.reset();
		const auto now = Poseidon::get_fast_mono_clock();
		gc_timer_proc(now);
	}

	void FriendBoxMap::random(std::vector<AccountUuid> &ret, std::size_t max_count, const boost::shared_ptr<FriendBox> &excluding_box) {
		PROFILE_ME;

		const auto random_pool = g_random_pool.lock();
		if (!random_pool) {
			LOG_EMPERY_CENTER_WARNING("Random friend pool is gone?");
			return;
		}

		const auto utc_now = Poseidon::get_utc_time();

		const auto filter_friends = [&] {
			std::size_t count_filtered = 0;
			auto it = random_pool->end();
			while ((count_filtered < max_count) && ((it != random_pool->begin()) && (--it, true))) {
				const auto account_uuid = *it;
				const auto account = AccountMap::get(account_uuid);
				if (!account) {
					LOG_EMPERY_CENTER_WARNING("Account not found: account_uuid = ", account_uuid);
					it = random_pool->erase(it);
					continue;
				}
				if (utc_now < account->get_banned_until()) {
					it = random_pool->erase(it);
					continue;
				}
				auto info = excluding_box->get(account->get_account_uuid());
				if (info.category != FriendBox::CAT_DELETED) {
					it = random_pool->erase(it);
					continue;
				}
				++count_filtered;
			}
		};

		filter_friends();
		if (random_pool->size() < max_count) {
			std::vector<boost::shared_ptr<Account>> accounts;
			AccountMap::get_all(accounts, 0, SIZE_MAX);
			std::random_shuffle(accounts.begin(), accounts.end(),
				[](std::uint32_t offset) { return Poseidon::rand32() % offset; });

			random_pool->clear();
			random_pool->reserve(accounts.size());
			std::transform(accounts.begin(), accounts.end(), std::back_inserter(*random_pool),
				[](const boost::shared_ptr<Account> &account) { return account->get_account_uuid(); });
			filter_friends();
		}

		ret.reserve(ret.size() + max_count);
		for (std::size_t i = 0; (i < max_count) && !random_pool->empty(); ++i) {
			ret.emplace_back(random_pool->back());
			random_pool->pop_back();
		}
	}
}
