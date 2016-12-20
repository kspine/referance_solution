#ifndef EMPERY_CENTER_LOG_MONGODB_DUNGEON_FINISH_HPP_
#define EMPERY_CENTER_LOG_MONGODB_DUNGEON_FINISH_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_DungeonFinish
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED          (dungeon_type_id)	\
	FIELD_DATETIME          (begin_time)	\
	FIELD_DATETIME          (finish_time)	\
	FIELD_UNSIGNED          (finished)

#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
