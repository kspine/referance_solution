#include "../precompiled.hpp"
#include "common.hpp"
#include "../../../empery_center/src/msg/sl_dungeon.hpp"
#include "../mongodb/dungeon_created.hpp"
#include "../mongodb/dungeon_deleted.hpp"

namespace EmperyCenterLog {

LOG_SERVLET(Msg::SL_DungeonCreated, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_DungeonCreated>(
		Poseidon::get_utc_time(),
		Poseidon::Uuid(req.account_uuid), req.dungeon_type_id);
	obj->async_save(false, true);
}

LOG_SERVLET(Msg::SL_DungeonDeleted, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_DungeonDeleted>(
		Poseidon::get_utc_time(),
		Poseidon::Uuid(req.account_uuid), req.dungeon_type_id, req.finished);
	obj->async_save(false, true);
}

}
