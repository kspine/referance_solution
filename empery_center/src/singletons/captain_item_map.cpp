#include "../precompiled.hpp"
#include "../mmain.hpp"
#include "captain_item_map.hpp"
#include "../mongodb/captain.hpp"
#include "../player_session.hpp"
#include "../account.hpp"
#include "../captain_item.hpp"
#include "../msg/sc_captain.hpp"
#include "../msg/err_captain.hpp"
#include "../captain_item_attribute_ids.hpp"
#include "../data/captain_item_config.hpp"
#include "../data/captain_config.hpp"
#include "../singletons/item_box_map.hpp"
#include "../item_box.hpp"
#include "../data/item.hpp"
#include "../msg/err_item.hpp"
#include "../reason_ids.hpp"
#include "../transaction_element.hpp"
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/timer_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/job_promise.hpp>
#include <tuple>
#include <poseidon/async_job.hpp>

namespace EmperyCenter {

namespace {
	struct CaptainItemElement {
		boost::shared_ptr<CaptainItem> captain_item;

		CaptainItemUuid captain_item_uuid;
		AccountUuid account_uuid;

		explicit CaptainItemElement(boost::shared_ptr<CaptainItem> captain_item_)
			: captain_item(std::move(captain_item_))
			, captain_item_uuid(captain_item->get_captain_item_uuid())
			, account_uuid(captain_item->get_account_uuid())
		{
		}
	};

	MULTI_INDEX_MAP(CaptainItemContainer, CaptainItemElement,
		UNIQUE_MEMBER_INDEX(captain_item_uuid)
		MULTI_MEMBER_INDEX(account_uuid)
	)

	boost::shared_ptr<CaptainItemContainer> g_captainitem_map;


	MODULE_RAII_PRIORITY(handles, 5000){
		const auto conn = Poseidon::MongoDbDaemon::create_connection();

		struct TempCaptainItemElement {
			boost::shared_ptr<MongoDb::Center_CaptainItem> obj;
			std::vector<boost::shared_ptr<MongoDb::Center_CaptainItemAttribute>> attributes;
		};
		std::map<CaptainItemUuid, TempCaptainItemElement> temp_captain_item_map;


		LOG_EMPERY_CENTER_INFO("Loading Center_CaptainItem...");
	
		conn->execute_query("Center_CaptainItem", { }, 0, INT32_MAX);
		while(conn->fetch_next()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainItem>();
			obj->fetch(conn);
			obj->enable_auto_saving();
			const auto captain_item_uuid = CaptainItemUuid(obj->get_captain_item_uuid());
			temp_captain_item_map[captain_item_uuid].obj = std::move(obj);
		}


		LOG_EMPERY_CENTER_INFO("Done loading temp_captain_item_map.", temp_captain_item_map.size());

		LOG_EMPERY_CENTER_INFO("Loading Center_CaptainItemAttribute attributes...");
		
	    conn->execute_query("Center_CaptainItemAttribute", { }, 0, INT32_MAX);

		while(conn->fetch_next()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainItemAttribute>();
			obj->fetch(conn);
			const auto captain_item_uuid = CaptainItemUuid(obj->unlocked_get_captain_item_uuid());
			const auto it = temp_captain_item_map.find(captain_item_uuid);
			if (it == temp_captain_item_map.end()){
				continue;
			}
			obj->enable_auto_saving();
			it->second.attributes.emplace_back(std::move(obj));
		}

		LOG_EMPERY_CENTER_INFO("Done loading Center_CaptainItemAttribute attributes.");

		const auto captain_item_map = boost::make_shared<CaptainItemContainer>();
		for (auto it = temp_captain_item_map.begin(); it != temp_captain_item_map.end(); ++it){
			auto captainitem = boost::make_shared<CaptainItem>(std::move(it->second.obj), it->second.attributes);

			captain_item_map->insert(CaptainItemElement(std::move(captainitem)));
		}
		g_captainitem_map = captain_item_map;
		handles.push(captain_item_map);

	}
}

boost::shared_ptr<CaptainItem> CaptainItemMap::get(CaptainItemUuid captain_item_uuid){
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("CaptainItem map not loaded.");
		return { };
	}

	const auto it = captain_item_map->find<0>(captain_item_uuid);
	if (it == captain_item_map->end<0>()){
		LOG_EMPERY_CENTER_TRACE("CaptainItem not found: captain_item_uuid = ", captain_item_uuid);
		return { };
	}
	return it->captain_item;
}
boost::shared_ptr<CaptainItem> CaptainItemMap::require(CaptainItemUuid captain_item_uuid){
	PROFILE_ME;

	auto captain_item = get(captain_item_uuid);
	if (!captain_item){
		LOG_EMPERY_CENTER_WARNING("Captain not found: captain_item_uuid = ", captain_item_uuid);
		DEBUG_THROW(Exception, sslit("Captain not found"));
	}
	return captain_item;
}

void CaptainItemMap::get_by_account_uuid(std::vector<boost::shared_ptr<CaptainItem> > &ret, AccountUuid account_uuid)
{
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("CaptainItem map not loaded.");
		return;
	}

	const auto range = captain_item_map->equal_range<1>(account_uuid);
	ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
	for(auto it = range.first; it != range.second; ++it){
		ret.emplace_back(it->captain_item);
	}
}

void CaptainItemMap::insert(const boost::shared_ptr<CaptainItem> &captainitem){
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("CaptainItem map not loaded.");
		DEBUG_THROW(Exception, sslit("CaptainItem map not loaded"));
	}


	const auto captain_item_uuid = captainitem->get_captain_item_uuid();

	// 判断是否已存在
	const auto info = get(captain_item_uuid);
	if (info)
	{
		LOG_EMPERY_CENTER_WARNING("CaptainItem already exists: captain_item_uuid = ", captain_item_uuid);
		DEBUG_THROW(Exception, sslit("CaptainItem already exists"));
	}
	else
	{
		captain_item_map->insert(CaptainItemElement(captainitem));

		LOG_EMPERY_CENTER_DEBUG("Inserting captain: captain_item_uuid = ", captain_item_uuid);
	}
}

void CaptainItemMap::deletemember(CaptainItemUuid captain_item_uuid, bool bdeletemap)
{
	// 先从内存中删，然后删数据库的
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("Captain map not loaded.");
		DEBUG_THROW(Exception, sslit("Captain map not loaded"));
	}

	const auto it = captain_item_map->find<0>(captain_item_uuid);
	if (it != captain_item_map->end<0>()){

		it->captain_item->delete_data();

		if(bdeletemap)
			captain_item_map->erase<0>(it);


		// 从数据库中删除该成员
		Poseidon::MongoDb::BsonBuilder query;
		query.append_string(sslit("_id"),PRIMERY_KEYGEN::GenIDS::GenId(captain_item_uuid.get()));
		
        LOG_EMPERY_CENTER_ERROR("query _id:captain_item_uuid=",captain_item_uuid.get());

		Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_CaptainItem", query, true);

	}
}

void CaptainItemMap::update(const boost::shared_ptr<CaptainItem> &captainitem, bool throws_if_not_exists){
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("CaptainItem map not loaded.");
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("CaptainItem map not loaded"));
		}
		return;
	}

	const auto captain_item_uuid = captainitem->get_captain_item_uuid();

	const auto it = captain_item_map->find<0>(captain_item_uuid);
	if (it == captain_item_map->end<0>()){
		LOG_EMPERY_CENTER_WARNING("CaptainItem not found: account_uuid = ", captain_item_uuid);
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("CaptainItem not found"));
		}
		return;
	}
	if (it->captain_item != captainitem){
		LOG_EMPERY_CENTER_DEBUG("CaptainItem expired: captain_uuid = ", captain_item_uuid);
		return;
	}

	LOG_EMPERY_CENTER_DEBUG("Updating CaptainItem: captain_item_uuid = ", captain_item_uuid);
	captain_item_map->replace<0>(it, CaptainItemElement(captainitem));
}

void CaptainItemMap::synchronize_with_player(AccountUuid account_uuid, const boost::shared_ptr<PlayerSession> &session) {
	PROFILE_ME;

	const auto &captain_item_map = g_captainitem_map;
	if (!captain_item_map){
		LOG_EMPERY_CENTER_WARNING("captainitem map not loaded.");
		Msg::SC_CaptainItemsInfo msg;
		msg.items.reserve(0);
		session->send(msg);
		return;
	}

	Msg::SC_CaptainItemsInfo msg;
	const auto range = captain_item_map->equal_range<1>(account_uuid);
	unsigned count = 0;
	for (auto it = range.first; it != range.second; ++it)
	{
		const auto info = it->captain_item;
	//	if (info->get_attribute(CaptainItemAttributeIds::ID_INUSE) == "1") 
	//		continue;

		auto &elem = *msg.items.emplace(msg.items.end());

		elem.captain_item_uuid = CaptainItemUuid(info->get_captain_item_uuid()).str();
		elem.baseid = info->get_attribute(CaptainItemAttributeIds::ID_BASEID);
		elem.level = info->get_attribute(CaptainItemAttributeIds::ID_LEVEL);
		elem.quality = info->get_attribute(CaptainItemAttributeIds::ID_QUALITY);
		elem.attr = info->get_attribute(CaptainItemAttributeIds::ID_ATTRIBUTE);
		elem.curstatus = info->get_attribute(CaptainItemAttributeIds::ID_INUSE);
		elem.affixattr = info->get_attribute(CaptainItemAttributeIds::ID_AFFIX_ATTRIBUTE);

		count += 1;

	}

	msg.items.reserve(count);

	session->send(msg);
}

int CaptainItemMap::reclaim_item(AccountUuid account_uuid, CaptainItemUuid captain_item_uuid)
{
	PROFILE_ME;
	// 回收道具获得东西
	const auto &captain_item_info = CaptainItemMap::get(captain_item_uuid);
	if (!captain_item_info)
	{
		return Msg::ERR_CAPTAIN_CAN_NOT_FIND_ITEM;
	}

	const auto& item = Data::CaptainItemConfig::get(boost::lexical_cast<std::uint64_t>(captain_item_info->get_attribute(CaptainItemAttributeIds::ID_BASEID)));
	if (item)
	{

		if (!item->equipment_resolve.empty())
		{
			const auto item_box = ItemBoxMap::require(account_uuid);
			std::vector<ItemTransactionElement> transaction;

			for (auto it = item->equipment_resolve.begin(); it != item->equipment_resolve.end(); ++it)
			{
				const auto item_id = ItemId(boost::lexical_cast<std::uint64_t>(it->first));

				boost::container::flat_map<std::string, std::uint64_t> map2 = it->second;

				auto min = 0;
				auto max = 0;
				auto sit = map2.find("min");
				if (sit != map2.end())
				{
					min = sit->second;
				}
				sit = map2.find("max");
				if (sit != map2.end())
				{
					max = sit->second;
				}

				auto count = Poseidon::rand32(boost::lexical_cast<boost::uint32_t>(min), boost::lexical_cast<boost::uint32_t>(max));
				if (count > 0)
					transaction.emplace_back(ItemTransactionElement::OP_ADD, item_id, count,
						ReasonIds::ID_EXPAND_CAPTAIN_RESOLVE_EQUIP, 0, 0, count);
			}

			// 增加道具
			const auto insuff_item_id = item_box->commit_transaction_nothrow(transaction, true,
				[&]{
				CaptainItemMap::deletemember(captain_item_uuid, true);
			});

			if (insuff_item_id)
			{
				return Msg::ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH;
			}

			return Msg::ST_OK;
		}
		else
		{
			CaptainItemMap::deletemember(captain_item_uuid, true);
		}

		return Msg::ST_OK;
	}

	return Msg::ST_OK;
}

void CaptainItemMap::reclaim_buy_quality(AccountUuid account_uuid, std::uint64_t quality)
{
	PROFILE_ME;

	std::vector<boost::shared_ptr<CaptainItem> > items;

	get_by_account_uuid(items, account_uuid);

	for (auto it = items.begin(); it != items.end(); ++it)
	{
		// 根据装备品质来做回收逻辑
		const auto captain_item_uuid = CaptainItemUuid((*it)->get_captain_item_uuid()).str();

		const auto& equip_info = CaptainItemMap::get(CaptainItemUuid(captain_item_uuid));
		if (equip_info && quality == boost::lexical_cast<std::uint64_t>(equip_info->get_attribute(CaptainItemAttributeIds::ID_QUALITY)))
		{
			reclaim_item(account_uuid, CaptainItemUuid(captain_item_uuid));
		}
	}
}

void CaptainItemMap::account_add_captain_item(AccountUuid account_uuid, std::uint64_t baseid)
{
	PROFILE_ME;

	const auto utc_now = Poseidon::get_utc_time();
	const auto config = Data::CaptainConfig::get(baseid);
	if(!config){
		LOG_EMPERY_CENTER_WARNING("account_add_captain_item no baseid, account_uuid = ",account_uuid," baseid = ",baseid);
		return;
	}

	// 创建将领道具并插入到将领道具列表内存中
	const auto captain_item_uuid = CaptainItemUuid(Poseidon::Uuid::random());
	auto pair = CaptainItem::async_create(captain_item_uuid, account_uuid, utc_now);
	Poseidon::JobDispatcher::yield(pair.first, true);

	auto captainitem = std::move(pair.second);
	// 初始化将领装备的一些属性
	captainitem->InitAttributes(baseid);

	CaptainItemMap::insert(captainitem);
}


}
