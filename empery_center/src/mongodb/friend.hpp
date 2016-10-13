#ifndef EMPERY_CENTER_MONGODB_FRIEND_HPP_
#define EMPERY_CENTER_MONGODB_FRIEND_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Friend
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,friend_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (friend_uuid)	\
	FIELD_UNSIGNED            (category)	\
	FIELD_STRING            (metadata)	\
	FIELD_DATETIME          (updated_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
