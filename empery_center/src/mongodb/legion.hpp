#ifndef EMPERY_CENTER_MONGODB_LEGION_HPP_
#define EMPERY_CENTER_MONGODB_LEGION_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {
#define MONGODB_OBJECT_NAME   Center_Legion
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_STRING            (name)	\
    FIELD_UUID            	(creater_uuid)	\
    FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid,legion_attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_SIGNED            (legion_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_Legion_Member
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID            	(account_uuid)	\
    FIELD_UUID              (legion_uuid)	\
    FIELD_DATETIME          (join_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionMemberAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,legion_member_attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (account_uuid)	\
    FIELD_SIGNED            (legion_member_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionApplyJoin
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,legion_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (account_uuid)	\
    FIELD_UUID  			(legion_uuid)	\
    FIELD_DATETIME          (apply_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionInviteJoin
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid,account_uuid,invited_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (account_uuid)	\
    FIELD_UUID  			(invited_uuid)	\
    FIELD_DATETIME          (invite_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_Legion_Package_Share
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(share_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (share_uuid)	\
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (account_uuid)	\
    FIELD_UNSIGNED            (task_id)	\
    FIELD_UNSIGNED            (task_package_item_id)	\
    FIELD_UNSIGNED            (share_package_item_id)	\
    FIELD_DATETIME          (share_package_time) \
    FIELD_DATETIME          (share_package_expire_time)

#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_Legion_Package_Pick_Share
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(share_uuid,account_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (share_uuid)	\
    FIELD_UUID              (account_uuid)	\
    FIELD_UNSIGNED            (share_package_status) \
    FIELD_DATETIME          (share_package_pick_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionBuilding
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_building_uuid,legion_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_building_uuid)	\
    FIELD_UUID              (legion_uuid)	\
	FIELD_UUID              (map_object_uuid)	\
    FIELD_SIGNED            (ntype)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionBuildingAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_building_uuid,legion_building_attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_building_uuid)	\
    FIELD_SIGNED            (legion_building_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionFinancial
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(financial_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID               (financial_uuid)  \
    FIELD_UUID               (legion_uuid)  \
    FIELD_UUID               (account_uuid)  \
    FIELD_UNSIGNED           (item_id) \
    FIELD_UNSIGNED           (old_count) \
    FIELD_UNSIGNED           (new_count) \
    FIELD_SIGNED             (delta_count) \
    FIELD_UNSIGNED           (action_id)   \
    FIELD_UNSIGNED           (action_count) \
    FIELD_DATETIME           (created_time)
#include <poseidon/mongodb/object_generator.hpp>
}
}

#endif
