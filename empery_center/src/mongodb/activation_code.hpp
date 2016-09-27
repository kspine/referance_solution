#ifndef EMPERY_CENTER_MONGODB_ACTIVATION_CODE_HPP_
#define EMPERY_CENTER_MONGODB_ACTIVATION_CODE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_ActivationCode
#define MONGODB_OBJECT_PRIMARY_KEY code
#define MONGODB_OBJECT_FIELDS \
	FIELD_STRING            (code)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UUID              (used_by_account)	\
	FIELD_DATETIME          (used_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
