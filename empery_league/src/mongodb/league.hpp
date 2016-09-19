#ifndef EMPERY_LEAGUE_MONGODB_LEAGUE_HPP_
#define EMPERY_LEAGUE_MONGODB_LEAGUE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyLeague {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   League_Info
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (league_uuid)	\
    FIELD_STRING            (name)	\
	FIELD_UUID            	(legion_uuid)	\
    FIELD_UUID            	(creater_uuid)	\
    FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_LeagueAttribute
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (league_uuid)	\
    FIELD_SIGNED            (league_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_Member
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID            	(legion_uuid)	\
    FIELD_UUID              (league_uuid)	\
    FIELD_DATETIME          (join_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_MemberAttribute
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_SIGNED            (league_member_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_LeagueApplyJoin
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID  			(league_uuid)	\
    FIELD_UUID  			(account_uuid)	\
    FIELD_DATETIME          (apply_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   League_LeagueInviteJoin
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (league_uuid)	\
    FIELD_UUID  			(account_uuid)	\
    FIELD_DATETIME          (invite_time)
#include <poseidon/mongodb/object_generator.hpp>

/*
#define MONGODB_OBJECT_NAME   Center_Legion_Package_Share
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (share_uuid)	\
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (account_uuid)	\
    FIELD_SIGNED            (task_id)	\
    FIELD_SIGNED            (task_package_item_id)	\
    FIELD_SIGNED            (share_package_item_id)	\
    FIELD_DATETIME          (share_package_time) \
    FIELD_DATETIME          (share_package_expire_time)

#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   Center_Legion_Package_Pick_Share
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (share_uuid)	\
    FIELD_UUID              (account_uuid)	\
    FIELD_SIGNED            (share_package_status)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   Center_LegionBuilding
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_building_uuid)	\
    FIELD_UUID              (legion_uuid)	\
	FIELD_UUID              (map_object_uuid)	\
    FIELD_SIGNED            (ntype)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionBuildingAttribute
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_building_uuid)	\
    FIELD_SIGNED            (legion_building_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

*/
}

}

#endif
