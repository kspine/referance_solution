#ifndef EMPERY_CENTER_MONGODB_ANNOUNCEMENT_HPP_
#define EMPERY_CENTER_MONGODB_ANNOUNCEMENT_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Announcement
#define MONGODB_OBJECT_PRIMARY_KEY announcement_uuid
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (announcement_uuid)  \
	FIELD_UNSIGNED            (language_id)   \
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UNSIGNED            (period)	\
	FIELD_UNSIGNED            (type)	\
	FIELD_STRING            (segments)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
