#ifndef EMPERY_CENTER_MONGODB_MAP_OBJECT_HPP_
#define EMPERY_CENTER_MONGODB_MAP_OBJECT_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {
	namespace MongoDb {
#define MONGODB_OBJECT_NAME   Center_MapObject
#define MONGODB_OBJECT_PRIMARY_KEY map_object_uuid
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (map_object_type_id)	\
	FIELD_UUID              (owner_uuid)	\
	FIELD_UUID              (parent_object_uuid)	\
	FIELD_STRING            (name)	\
	FIELD_SIGNED			(x)	\
	FIELD_SIGNED			(y)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_BOOLEAN           (garrisoned)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapObjectAttribute
#define MONGODB_OBJECT_PRIMARY_KEY map_object_uuid attribute_id
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED          (attribute_id)	\
	FIELD_SIGNED            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapObjectBuff
#define MONGODB_OBJECT_PRIMARY_KEY map_object_uuid buff_id
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_SIGNED            (buff_id)	\
	FIELD_UNSIGNED          (duration)	\
	FIELD_DATETIME          (time_begin)	\
	FIELD_DATETIME          (time_end)
#include <poseidon/mongodb/object_generator.hpp>
	}
}

#endif