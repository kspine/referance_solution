#ifndef EMPERY_CENTER_MONGODB_CAPTAIN_HPP_
#define EMPERY_CENTER_MONGODB_CAPTAIN_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {
	namespace MongoDb  {
#define  MONGODB_OBJECT_NAME   Center_Captain
#define  MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(captain_uuid);}
#define  MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (captain_uuid)	\
    FIELD_UUID            	(own_uuid)	\
    FIELD_DATETIME          (add_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CaptainAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(captain_uuid,attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (captain_uuid)	\
    FIELD_UNSIGNED          (attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CaptainItem
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(captain_item_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (captain_item_uuid)	\
	FIELD_UUID              (account_uuid)	\
    FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CaptainItemAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(captain_item_uuid,attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (captain_item_uuid)	\
    FIELD_UNSIGNED          (attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CaptainEquip
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(captain_uuid,captain_item_uuid);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (captain_uuid)	\
    FIELD_UUID              (captain_item_uuid)
#include <poseidon/mongodb/object_generator.hpp>

	}
}

#endif