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
	FIELD_UNSIGNED          (category)	\
	FIELD_STRING            (metadata)	\
	FIELD_DATETIME          (updated_time) \
	FIELD_UNSIGNED          (relation)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_FriendRecord
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(auto_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (auto_uuid)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (friend_account_uuid)	\
	FIELD_SIGNED            (result_type)	\
	FIELD_BOOLEAN           (deleted)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_FriendPrivateMsgData
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(msg_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (msg_uuid)	\
	FIELD_UNSIGNED          (language_id)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_UUID              (from_account_uuid)	\
	FIELD_STRING            (segments)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_FriendPrivateMsgRecent
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(auto_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (auto_uuid)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (friend_account_uuid)	\
	FIELD_UUID              (msg_uuid)	\
	FIELD_BOOLEAN           (sender)	\
	FIELD_BOOLEAN           (read)	\
	FIELD_BOOLEAN           (deleted)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
