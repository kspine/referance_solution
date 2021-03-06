#include "precompiled.hpp"
#include "crate_record_box.hpp"
#include "mongodb/battle_record.hpp"
#include "data/global.hpp"
#include "msg/sc_battle_record.hpp"
#include "singletons/player_session_map.hpp"
#include "player_session.hpp"

namespace EmperyCenter {

namespace {
	void fill_record_info(CrateRecordBox::RecordInfo &info, const boost::shared_ptr<MongoDb::Center_BattleRecordCrate> &obj){
		PROFILE_ME;

		info.auto_uuid              = obj->unlocked_get_auto_uuid();
		info.timestamp              = obj->get_timestamp();
		info.first_object_type_id   = MapObjectTypeId(obj->get_first_object_type_id());
		info.first_coord            = Coord(obj->get_first_coord_x(), obj->get_first_coord_y());
		info.second_coord           = Coord(obj->get_second_coord_x(), obj->get_second_coord_y());
		info.resource_id            = ResourceId(obj->get_resource_id());
		info.resource_harvested     = obj->get_resource_harvested();
		info.resource_gained        = obj->get_resource_gained();
		info.resource_remaining     = obj->get_resource_remaining();
	}
}

CrateRecordBox::CrateRecordBox(AccountUuid account_uuid,
	const std::vector<boost::shared_ptr<MongoDb::Center_BattleRecordCrate>> &records)
	: m_account_uuid(account_uuid)
{
	for(auto it = records.begin(); it != records.end(); ++it){
		const auto &record = *it;
		m_records.emplace_back(record);
	}
	std::sort(m_records.begin(), m_records.end(),
		[](const boost::shared_ptr<MongoDb::Center_BattleRecordCrate> &lhs, const boost::shared_ptr<MongoDb::Center_BattleRecordCrate> &rhs){
			return lhs->unlocked_get_auto_uuid() < rhs->unlocked_get_auto_uuid();
		});
}
CrateRecordBox::~CrateRecordBox(){
}

void CrateRecordBox::pump_status(){
	PROFILE_ME;

	const auto expiry_days = Data::Global::as_unsigned(Data::Global::SLOT_BATTLE_RECORD_EXPIRY_DAYS);
	const auto expired_before = Poseidon::get_utc_time_from_local((Poseidon::get_local_time() / 86400000 - expiry_days) * 86400000);
	for(;;){
		if(m_records.empty()){
			break;
		}
		const auto obj = m_records.front();
		if(obj->get_timestamp() >= expired_before){
			break;
		}
		LOG_EMPERY_CENTER_DEBUG("Removing expired crate record: account_uuid = ", get_account_uuid(),
			", auto_uuid = ", obj->unlocked_get_auto_uuid());
		m_records.pop_front();
		obj->set_deleted(true);
	}
}

void CrateRecordBox::get_all(std::vector<CrateRecordBox::RecordInfo> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_records.size());
	for(auto it = m_records.begin(); it != m_records.end(); ++it){
		RecordInfo info;
		fill_record_info(info, *it);
		ret.emplace_back(std::move(info));
	}
}
void CrateRecordBox::push(std::uint64_t timestamp, MapObjectTypeId first_object_type_id, Coord first_coord, Coord second_coord,
	ResourceId resource_id, std::uint64_t resource_harvested, std::uint64_t resource_gained, std::uint64_t resource_remaining)
{
	PROFILE_ME;

	const auto obj = boost::make_shared<MongoDb::Center_BattleRecordCrate>(Poseidon::Uuid::random(),
		get_account_uuid().get(), timestamp, first_object_type_id.get(), first_coord.x(), first_coord.y(),
		second_coord.x(), second_coord.y(), resource_id.get(), resource_harvested, resource_gained, resource_remaining, false);
	obj->async_save(true);
	m_records.emplace_back(obj);

	const auto max_record_count = Data::Global::as_unsigned(Data::Global::SLOT_MAX_BATTLE_RECORD_COUNT);
	while(m_records.size() > max_record_count){
		const auto obj = std::move(m_records.front());
		m_records.pop_front();
		obj->set_deleted(true);
	}

/*	const auto session = PlayerSessionMap::get(get_account_uuid());
	if(session){
		try {
			Msg::SC_BattleRecordNewCrateReceived msg;
			session->send(msg);
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			session->shutdown(e.what());
		}
	}*/
}

}
