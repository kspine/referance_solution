#ifndef EMPERY_CENTER_LOG_MONGODB_DUNGEON_DELETED_HPP_
#define EMPERY_CENTER_LOG_MONGODB_DUNGEON_DELETED_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_DungeonDeleted
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_SIGNED            (dungeon_type_id)	\
	FIELD_SIGNED            (finished)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
