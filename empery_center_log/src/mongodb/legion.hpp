#ifndef EMPERY_CENTER_LOG_MONGODB_LEGION_HPP_
#define EMPERY_CENTER_LOG_MONGODB_LEGION_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_LegionDisband
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_STRING  			(legion_name) \
	FIELD_DATETIME          (disband_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_LeagueDisband
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_STRING  		    (league_name) \
	FIELD_DATETIME          (disband_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_LegionMemberTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_UUID              (action_uuid)	\
	FIELD_UNSIGNED          (action_type)	\
	FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_LeagueLegionTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (legion_uuid)	\
	FIELD_UUID              (league_uuid)	\
	FIELD_UUID              (action_uuid)	\
	FIELD_UNSIGNED          (action_type)	\
	FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_PersonalDonateChanged
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED          (item_id)	\
	FIELD_UNSIGNED          (old_count)	\
	FIELD_UNSIGNED          (new_count)	\
	FIELD_SIGNED            (delta_count)	\
	FIELD_UNSIGNED          (reason)	\
	FIELD_UNSIGNED          (param1)	\
	FIELD_UNSIGNED          (param2)	\
	FIELD_UNSIGNED          (param3)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_LegionMoneyChanged
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_UNSIGNED          (item_id)	\
	FIELD_UNSIGNED          (old_count)	\
	FIELD_UNSIGNED          (new_count)	\
	FIELD_SIGNED            (delta_count)	\
	FIELD_UNSIGNED          (reason)	\
	FIELD_SIGNED            (param1)	\
	FIELD_SIGNED            (param2)	\
	FIELD_SIGNED            (param3)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_CreateWarehouseBuildingTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   CenterLog_OpenWarehouseBuildingTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_UNSIGNED          (level)	\
	FIELD_DATETIME          (open_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   CenterLog_RobWarehouseBuildingTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_UNSIGNED          (level)	\
	FIELD_UNSIGNED          (ntype)	\
	FIELD_UNSIGNED           (amount)	\
	FIELD_DATETIME          (rob_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
