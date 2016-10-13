#ifndef EMPERY_CENTER_MONGODB_ACCOUNT_HPP_
#define EMPERY_CENTER_MONGODB_ACCOUNT_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Account
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED          (platform_id)	\
	FIELD_STRING            (login_name)	\
	FIELD_UUID              (referrer_uuid)	\
	FIELD_UNSIGNED            (promotion_level)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_STRING            (nick)	\
	FIELD_BOOLEAN           (activated)	\
	FIELD_DATETIME          (banned_until)	\
	FIELD_DATETIME          (quieted_until)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_AccountAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,account_attribute_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_SIGNED            (account_attribute_id)	\
	FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
