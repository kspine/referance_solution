#ifndef EMPERY_CENTER_MONGODB_DEFENSE_BUILDING_HPP_
#define EMPERY_CENTER_MONGODB_DEFENSE_BUILDING_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_DefenseBuilding
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED          (building_level)	\
	FIELD_UNSIGNED          (mission)	\
	FIELD_UNSIGNED          (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)	\
	FIELD_UUID              (garrisoning_object_uuid)	\
	FIELD_DATETIME          (last_self_healed_time)
#include <poseidon/mongodb/object_generator.hpp>


#define MONGODB_OBJECT_NAME   Center_WarehouseBuilding
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UUID              (legion_uuid)	\
	FIELD_UNSIGNED          (building_level)	\
	FIELD_UNSIGNED          (mission)	\
	FIELD_UNSIGNED          (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)	\
	FIELD_UUID              (garrisoning_object_uuid)	\
	FIELD_DATETIME          (last_self_healed_time)	\
	FIELD_UNSIGNED          (output_type)	\
	FIELD_UNSIGNED          (output_amount)	\
	FIELD_DATETIME          (effect_time)	\
	FIELD_DATETIME          (cd_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
