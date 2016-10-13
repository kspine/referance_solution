#ifndef EMPERY_LEAGUE_MONGODB_LEAGUE_HPP_
#define EMPERY_LEAGUE_MONGODB_LEAGUE_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyLeague {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   League_Info
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(league_uuid);} 
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (league_uuid)	\
    FIELD_STRING            (name)	\
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (creater_uuid)	\
    FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_LeagueAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(league_uuid,league_attribute_id);} 
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (league_uuid)	\
    FIELD_SIGNED            (league_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_Member
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid);} 
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID            	(legion_uuid)	\
    FIELD_UUID              (league_uuid)	\
    FIELD_DATETIME          (join_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_MemberAttribute
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid,league_member_attribute_id);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_SIGNED            (league_member_attribute_id)	\
    FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   League_LeagueApplyJoin
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid,league_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID  			(league_uuid)	\
    FIELD_UUID  			(account_uuid)	\
    FIELD_DATETIME          (apply_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   League_LeagueInviteJoin
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(legion_uuid,league_uuid);}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (legion_uuid)	\
    FIELD_UUID              (league_uuid)	\
    FIELD_UUID  			(account_uuid)	\
    FIELD_DATETIME          (invite_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
