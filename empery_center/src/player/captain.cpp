#include "../precompiled.hpp"
#include "common.hpp"
#include "../mongodb/captain.hpp"
#include "../msg/cs_captain.hpp"
#include "../msg/err_captain.hpp"
#include "../msg/sc_captain.hpp"
#include "../account.hpp"
#include "../account_attribute_ids.hpp"
#include "../id_types.hpp"
#include "../captain.hpp"
#include "../captain_attribute_ids.hpp"
#include "../singletons/captain_map.hpp"
#include "../singletons/captain_item_map.hpp"
#include "../singletons/world_map.hpp"
#include "../msg/err_map.hpp"
#include "../map_object.hpp"
#include "../castle.hpp"
#include "../msg/err_castle.hpp"
#include "../attribute_ids.hpp"
#include "../captain_item.hpp"
#include "../data/captain_item_config.hpp"
#include "../data/captain_level_config.hpp"
#include "../data/captain_star_config.hpp"
#include "../data/captain_quality_config.hpp"
#include "../captain_item_attribute_ids.hpp"
#include "../singletons/item_box_map.hpp"
#include "../item_box.hpp"
#include "../data/item.hpp"
#include "../msg/err_item.hpp"
#include "../reason_ids.hpp"
#include "../transaction_element.hpp"
#include "../data/captain_config.hpp"
#include <poseidon/json.hpp>
#include <poseidon/async_job.hpp>
#include <string>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/singletons/mongodb_daemon.hpp>


namespace EmperyCenter {

PLAYER_SERVLET(Msg::CS_AddCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_AddCaptainMessage base_id:");

	const auto item_box = ItemBoxMap::require(account->get_account_uuid());

	const auto item_id = ItemId(req.base_id);
	const auto item_data = Data::Item::require(item_id);
	if (item_data->type.first != Data::Item::CAT_CREATE_CAPTAIN){
		return Response(Msg::ERR_ITEM_TYPE_MISMATCH) << (unsigned)Data::Item::CAT_CREATE_CAPTAIN;
	}

	const auto base_id = item_data->value;

	LOG_EMPERY_CENTER_DEBUG("CS_AddCaptainMessage base_id:", base_id);

	std::vector<ItemTransactionElement> transaction;
	transaction.emplace_back(ItemTransactionElement::OP_REMOVE, item_id, 1,
		ReasonIds::ID_EXPAND_CREATE_CAPTAIN, 0, item_id.get(), 1);
	const auto insuff_item_id = item_box->commit_transaction_nothrow(transaction, false,
		[&]{
		const auto utc_now = Poseidon::get_utc_time();

		// 创建将领并插入到将领列表内存中
		const auto captain_uuid = CaptainUuid(Poseidon::Uuid::random());
		auto pair = Captain::async_create(captain_uuid, account_uuid, utc_now);
		Poseidon::JobDispatcher::yield(pair.first, true);

		auto captain = std::move(pair.second);
		// 初始化将领的一些属性
		captain->InitAttributes(base_id);

		CaptainMap::insert(captain);
	});
	if (insuff_item_id){
		return Response(Msg::ERR_NO_ENOUGH_ITEMS) << insuff_item_id;
	}

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_AddCaptainItemMessage, account, session, req){
	PROFILE_ME;

	const auto item_base_id = req.item_base_id;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_AddCaptainItemMessage account_uuid:", account_uuid);

	CaptainItemMap::account_add_captain_item(account_uuid, item_base_id);

	return Response(Msg::ST_OK);
}


PLAYER_SERVLET(Msg::CS_GetCaptainsMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_GetCaptainsMessage account_uuid ================================ ",account_uuid);

	CaptainMap::synchronize_with_player(account_uuid, session);

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_SetCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_SetCaptainMessage account_uuid ================================ ", account_uuid);

	// 先判断是不是自己的将领
	const auto captain_uuid = CaptainUuid(req.captain_uuid);
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	if (captaininfo->get_attribute(CaptainAttributeIds::ID_CURSTATUS) == "1")
	{
		return Response(Msg::ERR_CAPTAIN_IN_USE);
	}

	const auto map_object_uuid = MapObjectUuid(req.map_object_uuid);
	const auto& map_object = WorldMap::get_map_object(map_object_uuid);
	if (!map_object){
		return Response(Msg::ERR_NO_SUCH_MAP_OBJECT) << map_object_uuid;
	}
	if (map_object->get_owner_uuid() != account->get_account_uuid()){
		return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT) << map_object->get_owner_uuid();
	}

	const auto castle_uuid = map_object->get_parent_object_uuid();
	const auto castle = boost::dynamic_pointer_cast<Castle>(WorldMap::get_map_object(castle_uuid));
	if (!castle){
		return Response(Msg::ERR_NO_SUCH_CASTLE) << castle_uuid;
	}
	if (!map_object->is_idle()){
		return Response(Msg::ERR_MAP_OBJECT_IS_NOT_GARRISONED);
	}


	if (map_object->get_attribute(AttributeIds::ID_CAPTAIN_BASEID) != 0)
	{
		// 已经有值，说明是替换，需要把原来的数据清理下
		const auto &old_captaininfo = CaptainMap::find_by_relation_map_object_uuid(account_uuid, req.map_object_uuid);
		if (old_captaininfo)
		{
			// 设置将领的当前状态
			boost::container::flat_map<CaptainAttributeId, std::string> Attributes;
			Attributes[CaptainAttributeIds::ID_CURSTATUS] = "0";
			Attributes[CaptainAttributeIds::ID_RELATION_OBJECT_UUID] = "";
			old_captaininfo->set_attributes(std::move(Attributes));
		}
	}

	// 设置部队的将领情况
	boost::container::flat_map<AttributeId, std::int64_t> modifiers;
	modifiers[AttributeIds::ID_CAPTAIN_BASEID] = boost::lexical_cast<std::int64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_BASEID));
	map_object->set_attributes(modifiers);

	// 设置将领的当前状态
	boost::container::flat_map<CaptainAttributeId, std::string> Attributes;
	Attributes[CaptainAttributeIds::ID_CURSTATUS] = "1";
	Attributes[CaptainAttributeIds::ID_RELATION_OBJECT_UUID] = req.map_object_uuid;
	captaininfo->set_attributes(std::move(Attributes));
	
	// 重新计算属性
	map_object->recalculate_captain_attributes(captaininfo,true);

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_UnloadCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_UnloadCaptainMessage account_uuid ================================ ", account_uuid);

	const auto map_object_uuid = MapObjectUuid(req.map_object_uuid);
	const auto& map_object = WorldMap::get_map_object(map_object_uuid);
	if (!map_object){
		return Response(Msg::ERR_NO_SUCH_MAP_OBJECT) << map_object_uuid;
	}
	if (map_object->get_owner_uuid() != account->get_account_uuid()){
		return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT) << map_object->get_owner_uuid();
	}

	const auto castle_uuid = map_object->get_parent_object_uuid();
	const auto castle = boost::dynamic_pointer_cast<Castle>(WorldMap::get_map_object(castle_uuid));
	if (!castle){
		return Response(Msg::ERR_NO_SUCH_CASTLE) << castle_uuid;
	}
	if (!map_object->is_idle()){
		return Response(Msg::ERR_MAP_OBJECT_IS_NOT_GARRISONED);
	}

	const auto captain_baseid = map_object->get_attribute(AttributeIds::ID_CAPTAIN_BASEID);
	if (captain_baseid != 0)
	{
		// 设置部队的将领情况
		boost::container::flat_map<AttributeId, std::int64_t> modifiers;
		modifiers[AttributeIds::ID_CAPTAIN_BASEID] = 0;
		map_object->set_attributes(modifiers);

		// 根据map_object_uuid查找到对应的将领
		const auto &captaininfo = CaptainMap::find_by_relation_map_object_uuid(account_uuid,req.map_object_uuid);
		if (captaininfo)
		{
			// 设置将领的当前状态
			boost::container::flat_map<CaptainAttributeId, std::string> Attributes;
			Attributes[CaptainAttributeIds::ID_CURSTATUS] = "0";
			Attributes[CaptainAttributeIds::ID_RELATION_OBJECT_UUID] = "";
			captaininfo->set_attributes(std::move(Attributes));

			// 重新计算属性
			map_object->recalculate_captain_attributes(captaininfo, false);
		}
	}
	else
	{
		return Response(Msg::ERR_CAPTAIN_CUR_NOT_IN_USE);
	}

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_ReclaimCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_ReclaimCaptainMessage account_uuid ================================ ", account_uuid);

	// 先判断是不是自己的将领
	const auto captain_uuid = CaptainUuid(req.captain_uuid);
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	if (captaininfo->get_attribute(CaptainAttributeIds::ID_CURSTATUS) == "1")
	{
		return Response(Msg::ERR_CAPTAIN_IN_USE);
	}

	// TODO: 回收后获得道具逻辑


	CaptainMap::deletemember(captain_uuid, account_uuid, true);

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_GetCaptainItemsMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_GetCaptainItemsMessage account_uuid ================================ ", account_uuid);

	CaptainItemMap::synchronize_with_player(account_uuid, session);

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_EquipCaptainItemsMessage, account, session, req){
	PROFILE_ME;

	const auto captain_uuid = CaptainUuid(req.captain_uuid);

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_EquipCaptainItemsMessage account_uuid:", account_uuid);

	// 先判断是不是自己的将领
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	
	// 查看道具信息
	const auto &captain_item_info = CaptainItemMap::get(CaptainItemUuid(req.captain_item_uuid));
	if (!captain_item_info)
	{
		return Response(Msg::ERR_CAPTAIN_CAN_NOT_FIND_ITEM);
	}
	if (captain_item_info->get_account_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_ISNOT_YOURS) << captain_item_info->get_account_uuid();
	}
	if (captain_item_info->get_attribute(CaptainItemAttributeIds::ID_INUSE) != "0")
	{
		if (req.ntype == 1)  // 穿
			return Response(Msg::ERR_CAPTAIN_ITEM_IN_USE);
	}
	else
	{
		if (req.ntype == 0) // 脱
			return Response(Msg::ERR_CAPTAIN_ITEM_NOT_IN_USE);
	}

	const auto& item = Data::CaptainItemConfig::get(boost::lexical_cast<std::uint64_t>(captain_item_info->get_attribute(CaptainItemAttributeIds::ID_BASEID)));
	if (item)
	{
		if (req.ntype == 1)  // 穿
		{
			// 查看等级是否满足
			if (item->level <= boost::lexical_cast<std::uint64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_LEVEL)))
			{
				// 查看原来是否已经在该部位有装备了，如果有，则先脱再穿
				if (item->position == 4)  // 宝物
				{
					const auto& old_equip = captaininfo->get_equip_by_pos(req.npos);
					if (old_equip)
					{
						captaininfo->unload_equip(old_equip);
					}
				}
				else
				{
					const auto& old_equip = captaininfo->get_equip_by_pos(item->position);
					if (old_equip)
					{
						captaininfo->unload_equip(old_equip);
					}
				}

				if (item->position == 4)  // 宝物
				{
					captaininfo->equip(captain_item_info, req.npos);
				}
				else
				{
					captaininfo->equip(captain_item_info, item->position);
				}
			}
			else
				return Response(Msg::ERR_CAPTAIN_ITEM_LEVEL_LIMIT);
		}
		else  // 脱
		{
			captaininfo->unload_equip(captain_item_info);
		}
	}


	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_UpgradeCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();
	const auto captain_uuid = CaptainUuid(req.captain_uuid);

	LOG_EMPERY_CENTER_DEBUG("CS_UpgradeCaptainMessage account_uuid ================================ ", account_uuid);
	// 先判断是不是自己的将领
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	
	// 已经满级就不再升级
	const auto curlevel = boost::lexical_cast<std::uint64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_LEVEL));
	const auto& nextlevelinfo = Data::CaptainLevelConfig::get(curlevel + 1);
	if (!nextlevelinfo)
	{
		return Response(Msg::ERR_CAPTAIN_FULL_LEVEL);
	}
	// 使用道具
	const auto item_id = ItemId(req.itemid);
	const auto& item = Data::Item::get(item_id);
	if (item)
	{
		// 检查类型对不
		if (item->type.first != Data::Item::CAT_CAPTAIN_ADD_EXP){
			return Response(Msg::ERR_ITEM_TYPE_MISMATCH) << (unsigned)Data::Item::CAT_CAPTAIN_ADD_EXP;
		}

		const auto item_box = ItemBoxMap::require(account_uuid);
		std::vector<ItemTransactionElement> transaction;
		transaction.emplace_back(ItemTransactionElement::OP_REMOVE, item_id, req.num,
			ReasonIds::ID_EXPAND_CAPTAIN_LEVEL, 0, 0, req.num);

		// 扣除道具，改变经验值
		const auto insuff_item_id = item_box->commit_transaction_nothrow(transaction, true,
			[&]{
			const auto addvalue = item->value * req.num;
			captaininfo->Addexp(addvalue);

		});

		if (insuff_item_id)
		{
			return Response(Msg::ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH);
		}
	}
	else
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH);
	}

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_UpgradeStarCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();
	const auto captain_uuid = CaptainUuid(req.captain_uuid);

	LOG_EMPERY_CENTER_DEBUG("CS_UpgradeStarCaptainMessage account_uuid ================================ ", account_uuid);

	// 先判断是不是自己的将领
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	if (captaininfo->get_attribute(CaptainAttributeIds::ID_CURSTATUS) == "1")
	{
	return Response(Msg::ERR_CAPTAIN_IN_USE);
	}


	// 已经满级就不再升级
	const auto curstarlevel = boost::lexical_cast<std::uint64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_STARLEVEL));
	const auto& nextstarlevelinfo = Data::CaptainStarConfig::get(curstarlevel + 1);
	if (!nextstarlevelinfo)
	{
		return Response(Msg::ERR_CAPTAIN_FULL_STAR_LEVEL);
	}
	// TODO 检查升星所需道具是否足够，够了就扣除道具，修改将领星级
	const auto item_box = ItemBoxMap::require(account_uuid);
	std::vector<ItemTransactionElement> transaction;

	for (auto it = nextstarlevelinfo->star_resource.begin(); it != nextstarlevelinfo->star_resource.end(); ++it)
	{
		const auto item_id = ItemId(boost::lexical_cast<std::uint64_t>(it->first));

		transaction.emplace_back(ItemTransactionElement::OP_REMOVE, item_id, it->second,
			ReasonIds::ID_EXPAND_CAPTAIN_STARLEVEL, 0, 0, it->second);
	}


	// 扣除道具，升星
	const auto insuff_item_id = item_box->commit_transaction_nothrow(transaction, true,
		[&]{
		captaininfo->UpgradeStar(curstarlevel + 1);
	});

	if (insuff_item_id)
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH);
	}


	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_UpgradeQualityCaptainMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();
	const auto captain_uuid = CaptainUuid(req.captain_uuid);

	LOG_EMPERY_CENTER_DEBUG("CS_UpgradeQualityCaptainMessage account_uuid ================================ ", account_uuid);

	// 先判断是不是自己的将领
	const auto &captaininfo = CaptainMap::get(captain_uuid);
	if (!captaininfo)
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}
	if (captaininfo->get_owner_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_NOT_YOURS) << captaininfo->get_owner_uuid();
	}
	if (captaininfo->get_attribute(CaptainAttributeIds::ID_CURSTATUS) == "1")
	{
	return Response(Msg::ERR_CAPTAIN_IN_USE);
	}

	// 已经满级就不再升级
	const auto curlevel = boost::lexical_cast<std::uint64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_QUALITYLEVEL));
	const auto& nextlevelinfo = Data::CaptainQualityConfig::get(curlevel + 1);
	if (!nextlevelinfo)
	{
		return Response(Msg::ERR_CAPTAIN_FULL_QUALITY_LEVEL);
	}

	// TODO 检查升星所需道具是否足够，够了就扣除道具，修改将领星级
	const auto& captain_data = Data::CaptainConfig::get(boost::lexical_cast<std::uint64_t>(captaininfo->get_attribute(CaptainAttributeIds::ID_BASEID)));
	if (captain_data)
	{
		auto qit = captain_data->quality_need.find(curlevel + 1);
		if (qit != captain_data->quality_need.end())
		{
			const auto item_box = ItemBoxMap::require(account_uuid);
			std::vector<ItemTransactionElement> transaction;

			const auto& quality_need = qit->second;
			for (auto it = quality_need.begin(); it != quality_need.end(); ++it)
			{
				const auto item_id = ItemId(boost::lexical_cast<std::uint64_t>(it->first));

				LOG_EMPERY_CENTER_DEBUG("CS_UpgradeQualityCaptainMessage account_uuid ================================ ", item_id, ",num:", it->second);

				transaction.emplace_back(ItemTransactionElement::OP_REMOVE, item_id, it->second,
					ReasonIds::ID_EXPAND_CAPTAIN_QUALITYLEVEL, 0, 0, it->second);
			}


			// 扣除道具，升品
			const auto insuff_item_id = item_box->commit_transaction_nothrow(transaction, true,
				[&]{
				captaininfo->UpgradeQuality(curlevel + 1);
			});

			if (insuff_item_id)
			{
				return Response(Msg::ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH);
			}
		}
		else
			return Response(Msg::ERR_CAPTAIN_FULL_QUALITY_LEVEL);
	}
	else
	{
		return Response(Msg::ERR_CAPTAIN_CANNOTFIND_BASEDATA);
	}

	return Response(Msg::ST_OK);
}


PLAYER_SERVLET(Msg::CS_ReclaimCaptainItemMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_ReclaimCaptainItemMessage account_uuid ================================ ", account_uuid);

	// 查看道具信息
	const auto &captain_item_info = CaptainItemMap::get(CaptainItemUuid(req.captain_item_uuid));
	if (!captain_item_info)
	{
		return Response(Msg::ERR_CAPTAIN_CAN_NOT_FIND_ITEM);
	}
	if (captain_item_info->get_account_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_ISNOT_YOURS) << captain_item_info->get_account_uuid();
	}
	if (captain_item_info->get_attribute(CaptainItemAttributeIds::ID_INUSE) != "0")
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_IN_USE);
	}

	return Response(CaptainItemMap::reclaim_item(account_uuid, CaptainItemUuid(req.captain_item_uuid)));
}

PLAYER_SERVLET(Msg::CS_OnekeyReclaimCaptainItemsMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_OnekeyReclaimCaptainItemsMessage account_uuid ================================ ", account_uuid);

	// TODO 回收道具获得东西
	// 回收白色
	CaptainItemMap::reclaim_buy_quality(account_uuid,1);
	// 回收绿色
	CaptainItemMap::reclaim_buy_quality(account_uuid,2);

	return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg::CS_ForgingCaptainItemMessage, account, session, req){
	PROFILE_ME;

	const auto account_uuid = account->get_account_uuid();

	LOG_EMPERY_CENTER_DEBUG("CS_ForgingCaptainItemMessage account_uuid ================================ ", account_uuid);

	// 查看道具信息
	const auto &captain_item_info = CaptainItemMap::get(CaptainItemUuid(req.captain_item_uuid));
	if (!captain_item_info)
	{
		return Response(Msg::ERR_CAPTAIN_CAN_NOT_FIND_ITEM);
	}
	if (captain_item_info->get_account_uuid() != account->get_account_uuid())
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_ISNOT_YOURS) << captain_item_info->get_account_uuid();
	}
	if (captain_item_info->get_attribute(CaptainItemAttributeIds::ID_INUSE) != "0")
	{
		return Response(Msg::ERR_CAPTAIN_ITEM_IN_USE);
	}

	// TODO 判断是否满足锻造升级条件，满足条件则扣除所需物品，修改道具属性

	return Response(Msg::ST_OK);
}


}
