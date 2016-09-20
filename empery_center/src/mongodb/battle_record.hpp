#ifndef EMPERY_CENTER_MONGODB_BATTLE_RECORD_HPP_
#define EMPERY_CENTER_MONGODB_BATTLE_RECORD_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_BattleRecord
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (auto_uuid)	\
	FIELD_UUID              (first_account_uuid)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_UNSIGNED            (first_object_type_id)	\
	FIELD_SIGNED            (first_coord_x)	\
	FIELD_SIGNED            (first_coord_y)	\
	FIELD_UUID              (second_account_uuid)	\
	FIELD_UNSIGNED            (second_object_type_id)	\
	FIELD_SIGNED            (second_coord_x)	\
	FIELD_SIGNED            (second_coord_y)	\
	FIELD_SIGNED            (result_type)	\
	FIELD_UNSIGNED            (soldiers_wounded)	\
	FIELD_UNSIGNED            (soldiers_wounded_added)	\
	FIELD_UNSIGNED            (soldiers_damaged)	\
	FIELD_UNSIGNED            (soldiers_remaining)	\
	FIELD_BOOLEAN           (deleted)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_BattleRecordCrate
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (auto_uuid)	\
	FIELD_UUID              (first_account_uuid)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_UNSIGNED            (first_object_type_id)	\
	FIELD_SIGNED            (first_coord_x)	\
	FIELD_SIGNED            (first_coord_y)	\
	FIELD_SIGNED            (second_coord_x)	\
	FIELD_SIGNED            (second_coord_y)	\
	FIELD_UNSIGNED            (resource_id)	\
	FIELD_UNSIGNED            (resource_harvested)	\
	FIELD_UNSIGNED            (resource_gained)	\
	FIELD_UNSIGNED            (resource_remaining)	\
	FIELD_BOOLEAN           (deleted)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
