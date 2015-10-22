#ifndef EMPERY_CENTER_MSG_SC_MAP_HPP_
#define EMPERY_CENTER_MSG_SC_MAP_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME	SC_MapWorldMapList
#define MESSAGE_ID		399
#define MESSAGE_FIELDS	\
	FIELD_ARRAY			(maps,	\
		FIELD_VINT			(mapX)	\
		FIELD_VINT			(mapY)	\
	)	\
	FIELD_VUINT			(mapWidth)	\
	FIELD_VUINT			(mapHeight)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME	SC_MapCellInfo
#define MESSAGE_ID		301
#define MESSAGE_FIELDS	\
	FIELD_VINT			(x)	\
	FIELD_VINT			(y)	\
	FIELD_VUINT			(terrainId)	\
	FIELD_VUINT			(overlayId)	\
	FIELD_VUINT			(resourceId)	\
	FIELD_VUINT			(resourceAmount)	\
	FIELD_STRING		(ownerUuid)	\
	FIELD_ARRAY			(buffSet,	\
		FIELD_VUINT			(buffId)	\
		FIELD_STRING		(buffOwner)	\
		FIELD_VUINT			(buffValue)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME	SC_MapObjectInfo
#define MESSAGE_ID		302
#define MESSAGE_FIELDS	\
	FIELD_STRING		(objectUuid)	\
	FIELD_VUINT			(objectTypeId)	\
	FIELD_STRING		(ownerUuid)	\
	FIELD_VINT			(x)	\
	FIELD_VINT			(y)	\
	FIELD_ARRAY			(attributes,	\
		FIELD_VUINT			(slot)	\
		FIELD_STRING		(value)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

}

}

#endif
