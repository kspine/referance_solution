#ifndef EMPERY_CENTER_MONGODB_ACTIVITY_HPP_
#define EMPERY_CENTER_MONGODB_ACTIVITY_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_MapActivityAccumulate
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenID_DateTime(account_uuid,map_activity_id,avaliable_since);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	 \
	FIELD_UNSIGNED          (map_activity_id)\
	FIELD_DATETIME          (avaliable_since) \
	FIELD_DATETIME          (avaliable_util) \
	FIELD_UNSIGNED          (accumulate_value) \
	FIELD_STRING            (target_reward)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapActivityRank
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenID_DateTime(account_uuid,map_activity_id,settle_date);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	 \
	FIELD_UNSIGNED          (map_activity_id)\
	FIELD_DATETIME          (settle_date) \
	FIELD_UNSIGNED          (rank) \
	FIELD_UNSIGNED          (accumulate_value)\
	FIELD_DATETIME          (process_date) \
	FIELD_BOOLEAN           (rewarded)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapWorldActivity
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenCID_DateTime(activity_id,cluster_x,cluster_y,since);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UNSIGNED          (activity_id)\
	FIELD_SIGNED            (cluster_x)	\
	FIELD_SIGNED            (cluster_y)	\
	FIELD_DATETIME          (since)\
	FIELD_DATETIME          (sub_since)\
	FIELD_DATETIME          (sub_until)\
	FIELD_UNSIGNED          (accumulate_value)\
	FIELD_BOOLEAN           (finish)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapWorldActivityAccumulate
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenCID_DateTime(account_uuid,activity_id,cluster_x,cluster_y,since);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	 \
	FIELD_UNSIGNED          (activity_id)\
	FIELD_SIGNED            (cluster_x)	\
	FIELD_SIGNED            (cluster_y)	\
	FIELD_DATETIME          (since) \
	FIELD_UNSIGNED          (accumulate_value) \
	FIELD_BOOLEAN           (rewarded)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapWorldActivityRank
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenCID_DateTime(account_uuid,cluster_x,cluster_y,since);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	 \
	FIELD_SIGNED            (cluster_x)	\
	FIELD_SIGNED            (cluster_y)	\
	FIELD_DATETIME          (since) \
	FIELD_UNSIGNED          (rank) \
	FIELD_UNSIGNED          (accumulate_value)\
	FIELD_DATETIME          (process_date) \
	FIELD_BOOLEAN           (rewarded)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapWorldActivityBoss
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenCID_DateTime(cluster_x,cluster_y,since);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (cluster_x) \
	FIELD_SIGNED            (cluster_y) \
	FIELD_DATETIME          (since) \
	FIELD_UUID              (boss_uuid) \
	FIELD_DATETIME          (create_date) \
	FIELD_DATETIME          (delete_date) \
	FIELD_UNSIGNED          (hp_total) \
	FIELD_UNSIGNED          (hp_die)
#include <poseidon/mongodb/object_generator.hpp>

}
}

#endif
