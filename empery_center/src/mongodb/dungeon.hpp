#ifndef EMPERY_CENTER_MONGODB_DUNGEON_HPP_
#define EMPERY_CENTER_MONGODB_DUNGEON_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Dungeon
#define MONGODB_OBJECT_PRIMARY_KEY account_uuid dungeon_type_id 
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED            (dungeon_type_id)	\
	FIELD_UNSIGNED            (entry_count)	\
	FIELD_UNSIGNED            (finish_count)	\
	FIELD_STRING            (tasks_finished)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
