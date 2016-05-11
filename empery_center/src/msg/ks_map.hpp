#ifndef EMPERY_CENTER_MSG_KS_MAP_HPP_
#define EMPERY_CENTER_MSG_KS_MAP_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME    KS_MapRegisterCluster
#define MESSAGE_ID      32300
#define MESSAGE_FIELDS  \
	FIELD_VINT          (numerical_x)	\
	FIELD_VINT          (numerical_y)	\
	FIELD_STRING        (name)	\
	FIELD_VUINT         (created_time)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    KS_MapUpdateMapObjectAction
#define MESSAGE_ID      32301
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_VINT          (x)	\
	FIELD_VINT          (y)	\
	FIELD_VUINT         (action)	\
	FIELD_STRING        (param)
#include <poseidon/cbpp/message_generator.hpp>
/*
#define MESSAGE_NAME    KS_MapRemoveMapObject
#define MESSAGE_ID      32302
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)
#include <poseidon/cbpp/message_generator.hpp>
*/
#define MESSAGE_NAME    KS_MapHarvestOverlay
#define MESSAGE_ID      32303
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_VUINT         (interval)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    KS_MapDeployImmigrants
#define MESSAGE_ID      32304
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_STRING        (castle_name)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    KS_MapEnterCastle
#define MESSAGE_ID      32305
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_STRING        (castle_uuid)
#include <poseidon/cbpp/message_generator.hpp>

// 32306

#define MESSAGE_NAME    KS_MapHarvestStrategicResource
#define MESSAGE_ID      32307
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_VUINT         (interval)
#include <poseidon/cbpp/message_generator.hpp>

// 32308

#define MESSAGE_NAME    KS_MapObjectAttackAction
#define MESSAGE_ID      32309
#define MESSAGE_FIELDS  \
	FIELD_STRING        (attacking_account_uuid)	\
	FIELD_STRING        (attacking_object_uuid)	\
	FIELD_VUINT         (attacking_object_type_id)	\
	FIELD_VINT          (attacking_coord_x)	\
	FIELD_VINT          (attacking_coord_y)	\
	FIELD_STRING        (attacked_account_uuid)	\
	FIELD_STRING        (attacked_object_uuid)	\
	FIELD_VUINT         (attacked_object_type_id)	\
	FIELD_VINT          (attacked_coord_x)	\
	FIELD_VINT          (attacked_coord_y)	\
	FIELD_VINT          (result_type)	\
	FIELD_VINT          (result_param1)	\
	FIELD_VINT          (result_param2)	\
	FIELD_VUINT         (soldiers_damaged)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    KS_MapHealMonster
#define MESSAGE_ID      32310
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    KS_MapHarvestResourceCrate
#define MESSAGE_ID      32311
#define MESSAGE_FIELDS  \
	FIELD_STRING        (attacking_account_uuid)	\
	FIELD_STRING        (attacking_object_uuid)	\
	FIELD_VUINT         (attacking_object_type_id)	\
	FIELD_VINT          (attacking_coord_x)	\
	FIELD_VINT          (attacking_coord_y)	\
	FIELD_STRING        (resource_crate_uuid)	\
	FIELD_VUINT         (amount_harvested)
#include <poseidon/cbpp/message_generator.hpp>

}

}

#endif
