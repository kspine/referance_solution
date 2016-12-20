#ifndef EMPERY_CENTER_MONGODB_ACTIVITY_CONFIG_HPP_
#define EMPERY_CENTER_MONGODB_ACTIVITY_CONFIG_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Activitys
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(activity_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UNSIGNED          (activity_id)\
	FIELD_STRING            (name) \
	FIELD_DATETIME          (begin_time) \
	FIELD_DATETIME          (end_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_ActivitysMap
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(activity_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UNSIGNED          (activity_id)\
	FIELD_STRING            (name) \
	FIELD_UNSIGNED          (type) \
	FIELD_UNSIGNED          (duration) \
	FIELD_STRING            (target)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_ActivitysRankAward
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UNSIGNED           (id)\
	FIELD_UNSIGNED           (activity_id)\
	FIELD_UNSIGNED           (rank_begin) \
	FIELD_UNSIGNED           (rank_end) \
	FIELD_STRING             (reward)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_ActivitysWorld
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(activity_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UNSIGNED           (activity_id)\
	FIELD_STRING             (name) \
	FIELD_UNSIGNED           (pro_activity_id)\
	FIELD_UNSIGNED           (own_activity_id)\
	FIELD_UNSIGNED          (type) \
	FIELD_STRING             (target) \
	FIELD_STRING             (drop)
#include <poseidon/mongodb/object_generator.hpp>
}

}

#endif
