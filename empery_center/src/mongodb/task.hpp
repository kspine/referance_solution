#ifndef EMPERY_CENTER_MONGODB_TASK_HPP_
#define EMPERY_CENTER_MONGODB_TASK_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Task
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED          (task_id)	\
	FIELD_UNSIGNED          (category)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_STRING            (progress)	\
	FIELD_BOOLEAN           (rewarded)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionTask
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (legion_uuid) \
	FIELD_UNSIGNED          (task_id)	\
	FIELD_UNSIGNED          (category)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_STRING            (progress)	\
	FIELD_BOOLEAN           (rewarded)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionTaskReward
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid) \
	FIELD_UNSIGNED          (task_type_id)	\
	FIELD_STRING            (progress)	\
	FIELD_DATETIME          (created_time) \
	FIELD_DATETIME          (last_reward_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_LegionTaskContribution
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (legion_uuid) \
	FIELD_UUID              (account_uuid) \
	FIELD_UNSIGNED          (day_contribution)	\
	FIELD_UNSIGNED          (week_contribution)	\
	FIELD_UNSIGNED          (total_contribution)	\
	FIELD_DATETIME          (last_update_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
