#include "../precompiled.hpp"
#include "../mmain.hpp"
#include "captain_map.hpp"
#include "../mongodb/captain.hpp"
#include "../player_session.hpp"
#include "../account.hpp"
#include "../captain.hpp"
#include "../msg/sc_captain.hpp"
#include "../captain_attribute_ids.hpp"
#include "../captain_item.hpp"
#include "../captain_item_attribute_ids.hpp"
#include "captain_item_map.hpp"
#include "../data/captain_item_config.hpp"
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/timer_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/job_promise.hpp>
#include <tuple>
#include <poseidon/async_job.hpp>

namespace EmperyCenter {

namespace {
	struct CaptainElement {
		boost::shared_ptr<Captain> captain;

		CaptainUuid captain_uuid;
		AccountUuid account_uuid;

		explicit CaptainElement(boost::shared_ptr<Captain> captain_)
			: captain(std::move(captain_))
			, captain_uuid(captain->get_captain_uuid())
			, account_uuid(captain->get_owner_uuid())
		{
		}
	};

	MULTI_INDEX_MAP(CaptainContainer, CaptainElement,
		UNIQUE_MEMBER_INDEX(captain_uuid)
		MULTI_MEMBER_INDEX(account_uuid)
	)

	boost::shared_ptr<CaptainContainer> g_captain_map;


	MODULE_RAII_PRIORITY(handles, 5000){
		const auto conn = Poseidon::MongoDbDaemon::create_connection();

		struct TempCaptainElement {
			boost::shared_ptr<MongoDb::Center_Captain> obj;
			std::vector<boost::shared_ptr<MongoDb::Center_CaptainAttribute>> attributes;
			std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> equips;
		};
		std::map<CaptainUuid, TempCaptainElement> temp_captain_map;


		LOG_EMPERY_CENTER_INFO("Loading Captains...");
		conn->execute_query("Center_Captain", { }, 0, INT32_MAX);
		while(conn->fetch_next()){
			auto obj = boost::make_shared<MongoDb::Center_Captain>();
			obj->fetch(conn);
			obj->enable_auto_saving();
			const auto captain_uuid = CaptainUuid(obj->get_captain_uuid());
			temp_captain_map[captain_uuid].obj = std::move(obj);
		}


		LOG_EMPERY_CENTER_INFO("Done loading captains.", temp_captain_map.size());

		LOG_EMPERY_CENTER_INFO("Loading captains attributes...");
		conn->execute_query("Center_CaptainAttribute", { }, 0, INT32_MAX);
		while(conn->fetch_next()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainAttribute>();
			obj->fetch(conn);
			const auto captain_uuid = CaptainUuid(obj->unlocked_get_captain_uuid());
			const auto it = temp_captain_map.find(captain_uuid);
			if (it == temp_captain_map.end()){
				continue;
			}
			obj->enable_auto_saving();
			it->second.attributes.emplace_back(std::move(obj));
		}

		LOG_EMPERY_CENTER_INFO("Done loading captains attributes.");

		LOG_EMPERY_CENTER_INFO("Loading Center_CaptainEquip...");
	
		conn->execute_query("Center_CaptainEquip", { }, 0, INT32_MAX);
		while (conn->fetch_next()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainEquip>();
			obj->fetch(conn);
			const auto captain_uuid = CaptainUuid(obj->unlocked_get_captain_uuid());
			const auto it = temp_captain_map.find(captain_uuid);
			if (it == temp_captain_map.end()){
				continue;
			}
			obj->enable_auto_saving();
			it->second.equips.emplace_back(std::move(obj));
		}
		LOG_EMPERY_CENTER_INFO("Done loading captains attributes.");
		

		const auto captain_map = boost::make_shared<CaptainContainer>();
		for(auto it = temp_captain_map.begin(); it != temp_captain_map.end(); ++it){
			auto captain = boost::make_shared<Captain>(std::move(it->second.obj), it->second.attributes, it->second.equips);

			captain_map->insert(CaptainElement(std::move(captain)));
		}
		g_captain_map = captain_map;
		handles.push(captain_map);

	}
}

boost::shared_ptr<Captain> CaptainMap::get(CaptainUuid captain_uuid){
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if(!captain_map){
		LOG_EMPERY_CENTER_WARNING("legion map not loaded.");
		return { };
	}

	const auto it = captain_map->find<0>(captain_uuid);
	if(it == captain_map->end<0>()){
		LOG_EMPERY_CENTER_TRACE("legion not found: captain_uuid = ", captain_uuid);
		return { };
	}
	return it->captain;
}
boost::shared_ptr<Captain> CaptainMap::require(CaptainUuid captain_uuid){
	PROFILE_ME;

	auto captain = get(captain_uuid);
	if (!captain){
		LOG_EMPERY_CENTER_WARNING("Captain not found: captain_uuid = ", captain_uuid);
		DEBUG_THROW(Exception, sslit("Captain not found"));
	}
	return captain;
}

void CaptainMap::get_by_account_uuid(std::vector<boost::shared_ptr<Captain> > &ret, AccountUuid account_uuid)
{
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if(!captain_map){
		LOG_EMPERY_CENTER_WARNING("Legion map not loaded.");
		return;
	}

	const auto range = captain_map->equal_range<1>(account_uuid);
	ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
	for(auto it = range.first; it != range.second; ++it){
		ret.emplace_back(it->captain);
	}
}

void CaptainMap::insert(const boost::shared_ptr<Captain> &captain){
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if (!captain_map){
		LOG_EMPERY_CENTER_WARNING("Captain map not loaded.");
		DEBUG_THROW(Exception, sslit("Captain map not loaded"));
	}


	const auto captain_uuid = captain->get_captain_uuid();

	// 判断是否已存在
	const auto member = get(captain_uuid);
	if(member)
	{
		LOG_EMPERY_CENTER_WARNING("Captain already exists: captain_uuid = ", captain_uuid);
		DEBUG_THROW(Exception, sslit("Captain already exists"));
	}
	else
	{
		captain_map->insert(CaptainElement(captain));

		LOG_EMPERY_CENTER_DEBUG("Inserting captain: captain_uuid = ", captain_uuid);
	}
}

void CaptainMap::deletemember(CaptainUuid captain_uuid, AccountUuid account_uuid, bool bdeletemap)
{
	// 先从内存中删，然后删数据库的
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if (!captain_map){
		LOG_EMPERY_CENTER_WARNING("Captain map not loaded.");
		DEBUG_THROW(Exception, sslit("Captain map not loaded"));
	}

	const auto it = captain_map->find<0>(captain_uuid);
	if (it != captain_map->end<0>()){

		it->captain->delete_data();

		if(bdeletemap)
			captain_map->erase<0>(it);


		// 从数据库中删除该成员
		Poseidon::MongoDb::BsonBuilder query;
		query.append_string(sslit("_id"),PRIMERY_KEYGEN::GenIDS::GenId(captain_uuid.get()));
		
        LOG_EMPERY_CENTER_ERROR("query _id:captain_uuid=",captain_uuid.get());

		Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_Captain", query, true);

	}
}

void CaptainMap::update(const boost::shared_ptr<Captain> &captain, bool throws_if_not_exists){
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if(!captain_map){
		LOG_EMPERY_CENTER_WARNING("Captain map not loaded.");
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Captain map not loaded"));
		}
		return;
	}


	const auto captain_uuid = captain->get_captain_uuid();

	const auto it = captain_map->find<0>(captain_uuid);
	if(it == captain_map->end<0>()){
		LOG_EMPERY_CENTER_WARNING("Captain not found: account_uuid = ", captain_uuid);
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Captain not found"));
		}
		return;
	}
	if (it->captain != captain){
		LOG_EMPERY_CENTER_DEBUG("Captain expired: captain_uuid = ", captain_uuid);
		return;
	}

	LOG_EMPERY_CENTER_DEBUG("Updating Captain: captain_uuid = ", captain_uuid);
	captain_map->replace<0>(it, CaptainElement(captain));
}

void CaptainMap::synchronize_with_player(AccountUuid account_uuid, const boost::shared_ptr<PlayerSession> &session) {
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if (!captain_map){
		LOG_EMPERY_CENTER_WARNING("captain map not loaded.");
		Msg::SC_CaptainsInfo msg;
		msg.captains.reserve(0);
		session->send(msg);
		return;
	}

	Msg::SC_CaptainsInfo msg;
	const auto range = captain_map->equal_range<1>(account_uuid);
	msg.captains.reserve(static_cast<std::uint64_t>(std::distance(range.first, range.second)));
	for (auto it = range.first; it != range.second; ++it)
	{
		auto &elem = *msg.captains.emplace(msg.captains.end());
		const auto info = it->captain;

		elem.captain_uuid = CaptainUuid(info->get_captain_uuid()).str();
		elem.baseid = info->get_attribute(CaptainAttributeIds::ID_BASEID);
		elem.curstatus = info->get_attribute(CaptainAttributeIds::ID_CURSTATUS);
		elem.relation_object_uuid = info->get_attribute(CaptainAttributeIds::ID_RELATION_OBJECT_UUID);
		elem.level = info->get_attribute(CaptainAttributeIds::ID_LEVEL);
		elem.starlevel = info->get_attribute(CaptainAttributeIds::ID_STARLEVEL);
		elem.curexp = info->get_attribute(CaptainAttributeIds::ID_CUREXP);
		elem.qualitylevel = info->get_attribute(CaptainAttributeIds::ID_QUALITYLEVEL);
		elem.base_attribute = info->get_attribute(CaptainAttributeIds::ID_BASEATTRIBUTE);
		elem.special_attribute = info->get_attribute(CaptainAttributeIds::ID_SPECIALATTRIBUTE);
		elem.equip_attribute = info->get_attribute(CaptainAttributeIds::ID_EQUIP_ATTRIBUTE);
		
		std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> equips;
		info->get_equips(equips);
		LOG_EMPERY_CENTER_WARNING("================================equips size:", equips.size());
		elem.equips.reserve(equips.size());
		for (auto eit = equips.begin(); eit != equips.end(); ++eit)
		{
			auto &equip_elem = *elem.equips.emplace(elem.equips.end());
			auto equip = *eit;

			equip_elem.captain_item_uuid = CaptainItemUuid(equip->get_captain_item_uuid()).str();

			const auto& equip_info = CaptainItemMap::get(CaptainItemUuid(equip->get_captain_item_uuid()));
			if (equip_info)
			{
				equip_elem.baseid = equip_info->get_attribute(CaptainItemAttributeIds::ID_BASEID);

				equip_elem.pos = boost::lexical_cast<std::uint64_t>(equip_info->get_attribute(CaptainItemAttributeIds::ID_INUSE));

			}
				
		}
	}

	session->send(msg);
}

boost::shared_ptr<Captain> CaptainMap::find_by_relation_map_object_uuid(AccountUuid account_uuid, std::string map_object_uuid)
{
	PROFILE_ME;

	const auto &captain_map = g_captain_map;
	if (!captain_map){
		LOG_EMPERY_CENTER_WARNING("captain map not loaded.");
		return{};
	}

	const auto range = captain_map->equal_range<1>(account_uuid);
	for (auto it = range.first; it != range.second; ++it)
	{
		const auto& info = it->captain;
		if (info->get_attribute(CaptainAttributeIds::ID_RELATION_OBJECT_UUID) == map_object_uuid)
			return it->captain;
	}

	return{};
}

}
