#ifndef EMPERY_CENTER_MSG_CS_CAPTAIN_HPP_
#define EMPERY_CENTER_MSG_CS_CAPTAIN_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME    CS_GetCaptainsMessage
#define MESSAGE_ID      1900
#define MESSAGE_FIELDS  \

#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SetCaptainMessage
#define MESSAGE_ID      1901
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)	\
	FIELD_STRING        (captain_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_UnloadCaptainMessage
#define MESSAGE_ID      1902
#define MESSAGE_FIELDS  \
	FIELD_STRING        (map_object_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_ReclaimCaptainMessage
#define MESSAGE_ID      1903
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_UpgradeCaptainMessage
#define MESSAGE_ID      1904
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)	\
	FIELD_VUINT         (itemid)	\
	FIELD_VUINT         (num)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_UpgradeStarCaptainMessage
#define MESSAGE_ID      1905
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_UpgradeQualityCaptainMessage
#define MESSAGE_ID      1906
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_GetCaptainItemsMessage
#define MESSAGE_ID      1907
#define MESSAGE_FIELDS  \

#include <poseidon/cbpp/message_generator.hpp>


#define MESSAGE_NAME    CS_EquipCaptainItemsMessage
#define MESSAGE_ID      1908
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)	\
	FIELD_STRING        (captain_item_uuid)	\
	FIELD_VUINT         (ntype)	\
	FIELD_VUINT         (npos)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_ReclaimCaptainItemMessage
#define MESSAGE_ID      1909
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_item_uuid)

#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_OnekeyReclaimCaptainItemsMessage
#define MESSAGE_ID      1910
#define MESSAGE_FIELDS  \

#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_ForgingCaptainItemMessage
#define MESSAGE_ID      1911
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_item_uuid)

#include <poseidon/cbpp/message_generator.hpp>

/*ÒÔÏÂÎªÁÙÊ±²âÊÔÓÃ´úÂë*/
#define MESSAGE_NAME    CS_AddCaptainMessage
#define MESSAGE_ID      1940
#define MESSAGE_FIELDS  \
	FIELD_VUINT         (base_id)

#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_AddCaptainItemMessage
#define MESSAGE_ID      1941
#define MESSAGE_FIELDS  \
	FIELD_VUINT         (item_base_id)	\
	FIELD_VUINT         (num)

#include <poseidon/cbpp/message_generator.hpp>


}

}

#endif
