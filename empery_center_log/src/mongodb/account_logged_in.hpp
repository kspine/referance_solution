#ifndef EMPERY_CENTER_LOG_MONGODB_ACCOUNT_LOGGED_IN_HPP_
#define EMPERY_CENTER_LOG_MONGODB_ACCOUNT_LOGGED_IN_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_AccountLoggedIn
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_STRING            (remote_ip)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
