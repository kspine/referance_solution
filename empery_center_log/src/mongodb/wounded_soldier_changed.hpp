#ifndef EMPERY_CENTER_LOG_MONGODB_WOUNDED_SOLDIER_CHANGED_HPP_
#define EMPERY_CENTER_LOG_MONGODB_WOUNDED_SOLDIER_CHANGED_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_WoundedSoldierChanged
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UUID              (owner_uuid)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_SIGNED            (old_count)	\
	FIELD_SIGNED            (new_count)	\
	FIELD_SIGNED            (delta_count)	\
	FIELD_SIGNED            (reason)	\
	FIELD_SIGNED            (param1)	\
	FIELD_SIGNED            (param2)	\
	FIELD_SIGNED            (param3)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
