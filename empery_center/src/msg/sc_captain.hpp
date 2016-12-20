#ifndef EMPERY_CENTER_MSG_SC_CAPTAIN_HPP_
#define EMPERY_CENTER_MSG_SC_CAPTAIN_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME    SC_CaptainsInfo
#define MESSAGE_ID      1950
#define MESSAGE_FIELDS  \
	FIELD_ARRAY         (captains,	\
		FIELD_STRING        (captain_uuid)	\
		FIELD_STRING        (baseid)	\
		FIELD_STRING        (curstatus)	\
		FIELD_STRING        (relation_object_uuid)	\
		FIELD_STRING        (level)	\
		FIELD_STRING        (starlevel)	\
		FIELD_STRING        (curexp)	\
		FIELD_STRING        (qualitylevel)	\
		FIELD_STRING        (base_attribute)	\
		FIELD_STRING        (special_attribute) \
		FIELD_STRING        (equip_attribute) \
		FIELD_ARRAY         (equips,	\
			FIELD_STRING         (captain_item_uuid)	\
			FIELD_STRING         (baseid)	\
			FIELD_VUINT          (pos)		\
		)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    SC_CaptainItemsInfo
#define MESSAGE_ID      1951
#define MESSAGE_FIELDS  \
	FIELD_ARRAY         (items,	\
		FIELD_STRING        (captain_item_uuid)	\
		FIELD_STRING        (baseid)	\
		FIELD_STRING        (level)	\
		FIELD_STRING        (quality)	\
		FIELD_STRING        (attr)	\
		FIELD_STRING        (curstatus)	\
		FIELD_STRING        (affixattr)	\
	)
#include <poseidon/cbpp/message_generator.hpp>


#define MESSAGE_NAME    SC_CaptainChange
#define MESSAGE_ID      1952
#define MESSAGE_FIELDS  \
	FIELD_STRING        (captain_uuid)	\
	FIELD_STRING        (baseid)	\
	FIELD_VUINT         (ntype)

#include <poseidon/cbpp/message_generator.hpp>
}

}

#endif
