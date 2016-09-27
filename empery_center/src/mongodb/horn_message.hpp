#ifndef EMPERY_CENTER_MONGODB_HORN_MESSAGE_HPP_
#define EMPERY_CENTER_MONGODB_HORN_MESSAGE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_HornMessage
#define MONGODB_OBJECT_PRIMARY_KEY horn_message_uuid
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (horn_message_uuid)	\
	FIELD_SIGNED            (item_id)	\
	FIELD_SIGNED            (language_id)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UUID              (from_account_uuid)	\
	FIELD_STRING            (segments)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
