#ifndef EMPERY_CENTER_LOG_MONGODB_ACCOUNT_NUMBER_ONLINE_HPP_
#define EMPERY_CENTER_LOG_MONGODB_ACCOUNT_NUMBER_ONLINE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_AccountNumberOnline
#define MONGODB_OBJECT_PRIMARY_KEY 
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_SIGNED            (interval)	\
	FIELD_SIGNED            (account_count)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
