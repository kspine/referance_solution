#ifndef EMPERY_CENTER_MONGODB_CASTLE_HPP_
#define EMPERY_CENTER_MONGODB_CASTLE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_CastleBuildingBase
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (building_base_id)	\
	FIELD_SIGNED            (building_id)	\
	FIELD_SIGNED            (building_level)	\
	FIELD_SIGNED            (mission)	\
	FIELD_SIGNED            (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleTech
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (tech_id)	\
	FIELD_SIGNED            (tech_level)	\
	FIELD_SIGNED            (mission)	\
	FIELD_SIGNED            (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleResource
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (resource_id)	\
	FIELD_SIGNED            (amount)	\
	FIELD_DATETIME          (updated_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleBattalion
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_SIGNED            (count)	\
	FIELD_BOOLEAN           (enabled)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleBattalionProduction
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (building_base_id)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_SIGNED            (count)	\
	FIELD_SIGNED            (production_duration)	\
	FIELD_DATETIME          (production_time_begin)	\
	FIELD_DATETIME          (production_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleWoundedSoldier
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_SIGNED            (count)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleTreatment
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_SIGNED            (count)	\
	FIELD_SIGNED            (duration)	\
	FIELD_DATETIME          (time_begin)	\
	FIELD_DATETIME          (time_end)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
