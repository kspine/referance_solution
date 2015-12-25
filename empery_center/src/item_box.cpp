#include "precompiled.hpp"
#include "item_box.hpp"
#include <poseidon/json.hpp>
#include <poseidon/string.hpp>
#include <poseidon/async_job.hpp>
#include "transaction_element.hpp"
#include "msg/sc_item.hpp"
#include "mysql/item.hpp"
#include "singletons/player_session_map.hpp"
#include "events/item.hpp"
#include "player_session.hpp"
#include "data/item.hpp"
#include "data/global.hpp"
#include "reason_ids.hpp"
#include "singletons/account_map.hpp"
#include "account.hpp"
#include "singletons/mail_box_map.hpp"
#include "mail_box.hpp"
#include "mail_data.hpp"
#include "item_ids.hpp"
#include "mail_type_ids.hpp"
#include "chat_message_slot_ids.hpp"

namespace EmperyCenter {

namespace {
	class FlagGuard {
	private:
		bool &m_locked;

	public:
		explicit FlagGuard(bool &locked)
			: m_locked(locked)
		{
			if(m_locked){
				DEBUG_THROW(Exception, sslit("Transaction locked"));
			}
			m_locked = true;
		}
		~FlagGuard(){
			if(!m_locked){
				assert(false);
			}
			m_locked = false;
		}

		FlagGuard(const FlagGuard &) = delete;
	};

	void fill_item_info(ItemBox::ItemInfo &info, const boost::shared_ptr<MySql::Center_Item> &obj){
		PROFILE_ME;

		info.item_id = ItemId(obj->get_item_id());
		info.count   = obj->get_count();
	}

	void fill_item_message(Msg::SC_ItemChanged &msg, const boost::shared_ptr<MySql::Center_Item> &obj){
		PROFILE_ME;

		msg.item_id = obj->get_item_id();
		msg.count   = obj->get_count();
	}

	// 税收。
	double              g_bonus_ratio;
	std::vector<double> g_level_bonus_array;
	std::vector<double> g_income_tax_array;
	std::vector<double> g_level_bonus_extra_array;

	MODULE_RAII_PRIORITY(/* handles */, 1000){
		const auto promotion_bonus_ratio = get_config<double>("promotion_bonus_ratio", 0.60);
		LOG_EMPERY_CENTER_DEBUG("> promotion_bonus_ratio = ", promotion_bonus_ratio);
		g_bonus_ratio = promotion_bonus_ratio;

		const auto promotion_level_bonus_array = get_config<Poseidon::JsonArray>("promotion_level_bonus_array");
		std::vector<double> temp_vec;
		temp_vec.reserve(promotion_level_bonus_array.size());
		for(auto it = promotion_level_bonus_array.begin(); it != promotion_level_bonus_array.end(); ++it){
			temp_vec.emplace_back(it->get<double>());
		}
		LOG_EMPERY_CENTER_DEBUG("> promotion_level_bonus_array = ", Poseidon::implode(',', temp_vec));
		g_level_bonus_array = std::move(temp_vec);

		const auto promotion_income_tax_array = get_config<Poseidon::JsonArray>("promotion_income_tax_array");
		temp_vec.clear();
		temp_vec.reserve(promotion_income_tax_array.size());
		for(auto it = promotion_income_tax_array.begin(); it != promotion_income_tax_array.end(); ++it){
			temp_vec.emplace_back(it->get<double>());
		}
		LOG_EMPERY_CENTER_DEBUG("> promotion_income_tax_array = ", Poseidon::implode(',', temp_vec));
		g_income_tax_array = std::move(temp_vec);

		const auto promotion_level_bonus_extra_array = get_config<Poseidon::JsonArray>("promotion_level_bonus_extra_array");
		temp_vec.clear();
		temp_vec.reserve(promotion_level_bonus_extra_array.size());
		for(auto it = promotion_level_bonus_extra_array.begin(); it != promotion_level_bonus_extra_array.end(); ++it){
			temp_vec.emplace_back(it->get<double>());
		}
		LOG_EMPERY_CENTER_DEBUG("> promotion_level_bonus_extra_array = ", Poseidon::implode(',', temp_vec));
		g_level_bonus_extra_array = std::move(temp_vec);
	}

	void accumulate_promotion_bonus(AccountUuid account_uuid, boost::uint64_t amount){
		PROFILE_ME;

		const auto send_mail_nothrow = [](const boost::shared_ptr<Account> &referrer, MailTypeId mail_type_id,
			boost::uint64_t amount_to_add, const boost::shared_ptr<Account> &taxer) noexcept
		{
			try {
				const auto referrer_uuid = referrer->get_account_uuid();
				const auto mail_box = MailBoxMap::require(referrer_uuid);

				const auto mail_uuid = MailUuid(Poseidon::Uuid::random());
				const auto language_id = LanguageId(); // neutral

				const auto default_mail_expiry_duration = Data::Global::as_unsigned(Data::Global::SLOT_DEFAULT_MAIL_EXPIRY_DURATION);
				const auto expiry_duration = checked_mul(default_mail_expiry_duration, (boost::uint64_t)60000);
				const auto utc_now = Poseidon::get_utc_time();

				const auto taxer_uuid = taxer->get_account_uuid();

				std::vector<std::pair<ChatMessageSlotId, std::string>> segments;
				segments.emplace_back(ChatMessageSlotIds::ID_TAXER, taxer_uuid.str());

				boost::container::flat_map<ItemId, boost::uint64_t> attachments;
				attachments.emplace(ItemIds::ID_GOLD, amount_to_add);

				const auto mail_data = boost::make_shared<MailData>(mail_uuid, language_id, utc_now,
					mail_type_id, AccountUuid(), std::string(), std::move(segments), std::move(attachments));
				MailBoxMap::insert_mail_data(mail_data);
				mail_box->insert(mail_data, saturated_add(utc_now, expiry_duration), MailBox::FL_SYSTEM);
				LOG_EMPERY_CENTER_DEBUG("Promotion bonus mail sent: referrer_uuid = ", referrer_uuid,
					", mail_type_id = ", mail_type_id, ", taxer_uuid = ", taxer_uuid, ", amount_to_add = ", amount_to_add);
			} catch(std::exception &e){
				LOG_EMPERY_CENTER_ERROR("std::exception thrown: what = ", e.what());
			}
		};

		const auto withdrawn = boost::make_shared<bool>(true);

		const auto account = AccountMap::require(account_uuid);

		const auto dividend_total         = static_cast<boost::uint64_t>(amount * g_bonus_ratio);
		const auto income_tax_ratio_total = std::accumulate(g_income_tax_array.begin(), g_income_tax_array.end(), 0.0);

		std::vector<boost::shared_ptr<Account>> referrers;
		{
			auto referrer = AccountMap::get(account->get_referrer_uuid());
			while(referrer){
				auto next_referrer = AccountMap::get(referrer->get_referrer_uuid());
				referrers.emplace_back(std::move(referrer));
				referrer = std::move(next_referrer);
			}
		}

		boost::uint64_t dividend_accumulated = 0;

		for(auto it = referrers.begin(); it != referrers.end(); ++it){
			const auto &referrer = *it;

			const auto referrer_uuid       = referrer->get_account_uuid();
			const unsigned promotion_level = referrer->get_promotion_level();

			double my_ratio;
			if((promotion_level == 0) || g_level_bonus_array.empty()){
				my_ratio = 0;
			} else if(promotion_level < g_level_bonus_array.size()){
				my_ratio = g_level_bonus_array.at(promotion_level - 1);
			} else {
				my_ratio = g_level_bonus_array.back();
			}

			const auto my_max_dividend = my_ratio * dividend_total;
			LOG_EMPERY_CENTER_DEBUG("> Current referrer: referrer_uuid = ", referrer_uuid,
				", promotion_level = ", promotion_level, ", my_ratio = ", my_ratio,
				", dividend_accumulated = ", dividend_accumulated, ", my_max_dividend = ", my_max_dividend);

			// 级差制。
			if(dividend_accumulated >= my_max_dividend){
				continue;
			}
			const auto my_dividend = my_max_dividend - dividend_accumulated;
			dividend_accumulated = my_max_dividend;

			// 扣除个税即使没有上家（视为被系统回收）。
			const auto tax_total = static_cast<boost::uint64_t>(std::round(my_dividend * income_tax_ratio_total));
			Poseidon::enqueue_async_job(
				std::bind(send_mail_nothrow, referrer, MailTypeIds::ID_LEVEL_BONUS, my_dividend - tax_total, account),
				{ }, withdrawn);

			{
				auto tit = g_income_tax_array.begin();
				for(auto bit = it + 1; (tit != g_income_tax_array.end()) && (bit != referrers.end()); ++bit){
					const auto &next_referrer = *bit;
					if(next_referrer->get_promotion_level() <= 1){
						continue;
					}
					const auto tax_amount = static_cast<boost::uint64_t>(std::round(my_dividend * (*tit)));
					Poseidon::enqueue_async_job(
						std::bind(send_mail_nothrow, next_referrer, MailTypeIds::ID_INCOME_TAX, tax_amount, account),
						{ }, withdrawn);
					++tit;
				}
			}

			if(promotion_level == g_level_bonus_array.size()){
				auto eit = g_level_bonus_extra_array.begin();
				for(auto bit = it + 1; (eit != g_level_bonus_extra_array.end()) && (bit != referrers.end()); ++bit){
					const auto &next_referrer = *bit;
					if(next_referrer->get_promotion_level() != g_level_bonus_array.size()){
						continue;
					}
					const auto extra_amount = static_cast<boost::uint64_t>(std::round(dividend_total * (*eit)));
					Poseidon::enqueue_async_job(
						std::bind(send_mail_nothrow, next_referrer, MailTypeIds::ID_LEVEL_BONUS_EXTRA, extra_amount, account),
						{ }, withdrawn);
					++eit;
				}
			}
		}

		*withdrawn = false;
	}
}

ItemBox::ItemBox(AccountUuid account_uuid,
	const std::vector<boost::shared_ptr<MySql::Center_Item>> &items)
	: m_account_uuid(account_uuid)
{
	for(auto it = items.begin(); it != items.end(); ++it){
		const auto &obj = *it;
		m_items.emplace(ItemId(obj->get_item_id()), obj);
	}
}
ItemBox::~ItemBox(){
}

void ItemBox::pump_status(){
	PROFILE_ME;

	const auto utc_now = Poseidon::get_utc_time();

	LOG_EMPERY_CENTER_TRACE("Checking for auto increment items: account_uuid = ", get_account_uuid());
	std::vector<ItemTransactionElement> transaction;
	std::vector<boost::shared_ptr<const Data::Item>> items_to_check;
	Data::Item::get_auto_inc(items_to_check);
	boost::container::flat_map<boost::shared_ptr<MySql::Center_Item>, boost::uint64_t> new_timestamps;
	for(auto dit = items_to_check.begin(); dit != items_to_check.end(); ++dit){
		const auto &item_data = *dit;
		auto it = m_items.find(item_data->item_id);
		if(it == m_items.end()){
			auto obj = boost::make_shared<MySql::Center_Item>(get_account_uuid().get(), item_data->item_id.get(), 0, 0);
			obj->async_save(true);
			it = m_items.emplace(item_data->item_id, std::move(obj)).first;
		}
		const auto &obj = it->second;

		boost::uint64_t auto_inc_period, auto_inc_offset;
		switch(item_data->auto_inc_type){
		case Data::Item::AIT_HOURLY:
			auto_inc_period = 3600 * 1000;
			auto_inc_offset = checked_mul(item_data->auto_inc_offset, (boost::uint64_t)60000);
			break;
		case Data::Item::AIT_DAILY:
			auto_inc_period = 24 * 3600 * 1000;
			auto_inc_offset = checked_mul(item_data->auto_inc_offset, (boost::uint64_t)60000);
			break;
		case Data::Item::AIT_WEEKLY:
			auto_inc_period = 7 * 24 * 3600 * 1000;
			auto_inc_offset = checked_mul(item_data->auto_inc_offset, (boost::uint64_t)60000) + 3 * 24 * 3600 * 1000; // 注意 1970-01-01 是星期四。
			break;
		case Data::Item::AIT_PERIODIC:
			auto_inc_period = checked_mul(item_data->auto_inc_offset, (boost::uint64_t)60000);
			auto_inc_offset = utc_now + 1; // 当前时间永远是区间中的最后一秒。
			break;
		default:
			auto_inc_period = 0;
			auto_inc_offset = 0;
			break;
		}
		if(auto_inc_period == 0){
			LOG_EMPERY_CENTER_WARNING("Item auto increment period is zero? item_id = ", item_data->item_id);
			continue;
		}
		auto_inc_offset %= auto_inc_period;

		const auto old_count = obj->get_count();
		const auto old_updated_time = obj->get_updated_time();

		const auto prev_interval = checked_sub(checked_add(old_updated_time, auto_inc_period), auto_inc_offset) / auto_inc_period;
		const auto cur_interval = checked_sub(utc_now, auto_inc_offset) / auto_inc_period;
		LOG_EMPERY_CENTER_TRACE("> Checking item: item_id = ", item_data->item_id,
			", prev_interval = ", prev_interval, ", cur_interval = ", cur_interval);
		if(cur_interval <= prev_interval){
			continue;
		}
		const auto interval_count = cur_interval - prev_interval;

		if(item_data->auto_inc_step >= 0){
			if(old_count < item_data->auto_inc_bound){
				const auto count_to_add = saturated_mul(static_cast<boost::uint64_t>(item_data->auto_inc_step), interval_count);
				const auto new_count = std::min(saturated_add(old_count, count_to_add), item_data->auto_inc_bound);
				LOG_EMPERY_CENTER_TRACE("> Adding items: item_id = ", item_data->item_id, ", old_count = ", old_count, ", new_count = ", new_count);
				transaction.emplace_back(ItemTransactionElement::OP_ADD, item_data->item_id, new_count - old_count,
					ReasonIds::ID_AUTO_INCREMENT, item_data->auto_inc_type, item_data->auto_inc_offset, 0);
			}
		} else {
			if(old_count > item_data->auto_inc_bound){
				LOG_EMPERY_CENTER_TRACE("> Removing items: item_id = ", item_data->item_id, ", init_count = ", item_data->init_count);
				const auto count_to_remove = saturated_mul(static_cast<boost::uint64_t>(-(item_data->auto_inc_step)), interval_count);
				const auto new_count = std::max(saturated_sub(old_count, count_to_remove), item_data->auto_inc_bound);
				LOG_EMPERY_CENTER_TRACE("> Removing items: item_id = ", item_data->item_id, ", old_count = ", old_count, ", new_count = ", new_count);
				transaction.emplace_back(ItemTransactionElement::OP_REMOVE, item_data->item_id, old_count - new_count,
					ReasonIds::ID_AUTO_INCREMENT, item_data->auto_inc_type, item_data->auto_inc_offset, 0);
			}
		}
		const auto new_updated_time = saturated_add(old_updated_time, saturated_mul(auto_inc_period, interval_count));
		new_timestamps.emplace(obj, new_updated_time);
	}
	commit_transaction(transaction,
		[&]{
			for(auto it = new_timestamps.begin(); it != new_timestamps.end(); ++it){
				it->first->set_updated_time(it->second);
			}
		});
}

void ItemBox::check_init_items(){
	PROFILE_ME;

	LOG_EMPERY_CENTER_TRACE("Checking for init items: account_uuid = ", get_account_uuid());
	std::vector<ItemTransactionElement> transaction;
	std::vector<boost::shared_ptr<const Data::Item>> items_to_check;
	Data::Item::get_init(items_to_check);
	for(auto dit = items_to_check.begin(); dit != items_to_check.end(); ++dit){
		const auto &item_data = *dit;
		const auto it = m_items.find(item_data->item_id);
		if(it == m_items.end()){
			LOG_EMPERY_CENTER_TRACE("> Adding items: item_id = ", item_data->item_id, ", init_count = ", item_data->init_count);
			transaction.emplace_back(ItemTransactionElement::OP_ADD, item_data->item_id, item_data->init_count,
				ReasonIds::ID_INIT_ITEMS, item_data->init_count, 0, 0);
		}
	}
	commit_transaction(transaction);
}

ItemBox::ItemInfo ItemBox::get(ItemId item_id) const {
	PROFILE_ME;

	ItemInfo info = { };
	info.item_id = item_id;

	const auto it = m_items.find(item_id);
	if(it == m_items.end()){
		return info;
	}
	fill_item_info(info, it->second);
	return info;
}
void ItemBox::get_all(std::vector<ItemBox::ItemInfo> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_items.size());
	for(auto it = m_items.begin(); it != m_items.end(); ++it){
		ItemInfo info;
		fill_item_info(info, it->second);
		ret.emplace_back(std::move(info));
	}
}

ItemId ItemBox::commit_transaction_nothrow(const std::vector<ItemTransactionElement> &transaction,
	const boost::function<void ()> &callback, bool notax)
{
	PROFILE_ME;

	const FlagGuard transaction_guard(m_locked_by_transaction);

	const auto account_uuid = get_account_uuid();

	boost::shared_ptr<bool> withdrawn;
	boost::container::flat_map<boost::shared_ptr<MySql::Center_Item>, boost::uint64_t /* new_count */> temp_result_map;
	boost::uint64_t taxing_amount = 0;

	for(auto tit = transaction.begin(); tit != transaction.end(); ++tit){
		const auto operation   = tit->m_operation;
		const auto item_id     = tit->m_some_id;
		const auto delta_count = tit->m_delta_count;

		if(delta_count == 0){
			continue;
		}

		const auto reason = tit->m_reason;
		const auto param1 = tit->m_param1;
		const auto param2 = tit->m_param2;
		const auto param3 = tit->m_param3;

		switch(operation){
		case ItemTransactionElement::OP_NONE:
			break;

		case ItemTransactionElement::OP_ADD:
			{
				auto it = m_items.find(item_id);
				if(it == m_items.end()){
					auto obj = boost::make_shared<MySql::Center_Item>(account_uuid.get(), item_id.get(), 0, 0);
					obj->enable_auto_saving(); // obj->async_save(true);
					it = m_items.emplace_hint(it, item_id, std::move(obj));
				}
				const auto &obj = it->second;
				auto temp_it = temp_result_map.find(obj);
				if(temp_it == temp_result_map.end()){
					temp_it = temp_result_map.emplace_hint(temp_it, obj, obj->get_count());
				}
				const auto old_count = temp_it->second;
				temp_it->second = checked_add(old_count, delta_count);
				const auto new_count = temp_it->second;

				LOG_EMPERY_CENTER_DEBUG("@ Item transaction: add: account_uuid = ", account_uuid,
					", item_id = ", item_id, ", old_count = ", old_count, ", delta_count = ", delta_count, ", new_count = ", new_count,
					", reason = ", reason, ", param1 = ", param1, ", param2 = ", param2, ", param3 = ", param3);
				if(!withdrawn){
					withdrawn = boost::make_shared<bool>(true);
				}
				Poseidon::async_raise_event(
					boost::make_shared<Events::ItemChanged>(account_uuid,
						item_id, old_count, new_count, reason, param1, param2, param3),
					withdrawn);
			}
			break;

		case ItemTransactionElement::OP_REMOVE:
		case ItemTransactionElement::OP_REMOVE_SATURATED:
			{
				const auto it = m_items.find(item_id);
				if(it == m_items.end()){
					if(operation != ItemTransactionElement::OP_REMOVE_SATURATED){
						LOG_EMPERY_CENTER_DEBUG("Item not found: item_id = ", item_id);
						return item_id;
					}
					break;
				}
				const auto &obj = it->second;
				auto temp_it = temp_result_map.find(obj);
				if(temp_it == temp_result_map.end()){
					temp_it = temp_result_map.emplace_hint(temp_it, obj, obj->get_count());
				}
				const auto old_count = temp_it->second;
				if(temp_it->second >= delta_count){
					temp_it->second -= delta_count;
				} else {
					if(operation != ItemTransactionElement::OP_REMOVE_SATURATED){
						LOG_EMPERY_CENTER_DEBUG("No enough items: item_id = ", item_id,
							", temp_count = ", temp_it->second, ", delta_count = ", delta_count);
						return item_id;
					}
					temp_it->second = 0;
				}
				const auto new_count = temp_it->second;

				if((item_id == ItemIds::ID_DIAMONDS) || (item_id == ItemIds::ID_GOLD)){
					taxing_amount = checked_add(taxing_amount, saturated_sub(old_count, new_count));
				}

				LOG_EMPERY_CENTER_DEBUG("@ Item transaction: remove: account_uuid = ", account_uuid,
					", item_id = ", item_id, ", old_count = ", old_count, ", delta_count = ", delta_count, ", new_count = ", new_count,
					", reason = ", reason, ", param1 = ", param1, ", param2 = ", param2, ", param3 = ", param3);
				if(!withdrawn){
					withdrawn = boost::make_shared<bool>(true);
				}
				Poseidon::async_raise_event(
					boost::make_shared<Events::ItemChanged>(account_uuid,
						item_id, old_count, new_count, reason, param1, param2, param3),
					withdrawn);
			}
			break;

		default:
			LOG_EMPERY_CENTER_ERROR("Unknown item transaction operation: operation = ", (unsigned)operation);
			DEBUG_THROW(Exception, sslit("Unknown item transaction operation"));
		}
	}

	if(callback){
		callback();
	}

	if(!notax && (taxing_amount > 0)){
		LOG_EMPERY_CENTER_DEBUG("Calculating tax: account_uuid = ", account_uuid, ", taxing_amount = ", taxing_amount);

		try {
			accumulate_promotion_bonus(account_uuid, taxing_amount);
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
		}
	}

	for(auto it = temp_result_map.begin(); it != temp_result_map.end(); ++it){
		it->first->set_count(it->second);
	}
	if(withdrawn){
		*withdrawn = false;
	}

	const auto session = PlayerSessionMap::get(get_account_uuid());
	if(session){
		try {
			for(auto it = temp_result_map.begin(); it != temp_result_map.end(); ++it){
				Msg::SC_ItemChanged msg;
				fill_item_message(msg, it->first);
				session->send(msg);
			}
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			session->shutdown(e.what());
		}
	}

	return { };
}
void ItemBox::commit_transaction(const std::vector<ItemTransactionElement> &transaction,
	const boost::function<void ()> &callback, bool notax)
{
	PROFILE_ME;

	const auto insuff_id = commit_transaction_nothrow(transaction, callback, notax);
	if(insuff_id != ItemId()){
		LOG_EMPERY_CENTER_DEBUG("Insufficient items in item box: account_uuid = ", get_account_uuid(), ", insuff_id = ", insuff_id);
		DEBUG_THROW(Exception, sslit("Insufficient items in item box"));
	}
}

void ItemBox::synchronize_with_player(const boost::shared_ptr<PlayerSession> &session) const {
	PROFILE_ME;

	for(auto it = m_items.begin(); it != m_items.end(); ++it){
		Msg::SC_ItemChanged msg;
		fill_item_message(msg, it->second);
		session->send(msg);
	}
}

}
