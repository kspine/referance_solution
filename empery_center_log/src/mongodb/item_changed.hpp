#ifndef EMPERY_CENTER_LOG_MONGODB_ITEM_CHANGED_HPP_
#define EMPERY_CENTER_LOG_MONGODB_ITEM_CHANGED_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_ItemChanged
#define MONGODB_OBJECT_PRIMARY_KEY 
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_SIGNED            (item_id)	\
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
