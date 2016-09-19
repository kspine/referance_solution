#ifndef EMPERY_CENTER_MONGODB_MAP_CELL_HPP_
#define EMPERY_CENTER_MONGODB_MAP_CELL_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_MapCell
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_UUID              (parent_object_uuid)	\
	FIELD_BOOLEAN           (acceleration_card_applied)	\
	FIELD_SIGNED            (ticket_item_id)	\
	FIELD_SIGNED            (production_resource_id)	\
	FIELD_DATETIME          (last_production_time)	\
	FIELD_SIGNED            (resource_amount)	\
	FIELD_UUID              (occupier_object_uuid)	\
	FIELD_UUID              (occupier_owner_uuid)	\
	FIELD_DATETIME          (last_self_healed_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapCellAttribute
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_SIGNED            (attribute_id)	\
	FIELD_SIGNED            (value)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapCellBuff
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_SIGNED            (buff_id)   \
	FIELD_SIGNED            (duration)  \
	FIELD_DATETIME          (time_begin)    \
	FIELD_DATETIME          (time_end)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
