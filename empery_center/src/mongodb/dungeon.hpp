#ifndef EMPERY_CENTER_MONGODB_DUNGEON_HPP_
#define EMPERY_CENTER_MONGODB_DUNGEON_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Dungeon
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_SIGNED            (dungeon_type_id)	\
	FIELD_SIGNED            (entry_count)	\
	FIELD_SIGNED            (finish_count)	\
	FIELD_STRING            (tasks_finished)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
