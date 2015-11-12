#ifndef EMPERY_CENTER_MSG_CS_CASTLE_HPP_
#define EMPERY_CENTER_MSG_CS_CASTLE_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME    CS_CastleQueryInfo
#define MESSAGE_ID      400
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleCreateBuilding
#define MESSAGE_ID      401
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)	\
	FIELD_VUINT         (buildingId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleCancelBuildingMission
#define MESSAGE_ID      402
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleUpgradeBuilding
#define MESSAGE_ID      403
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleDestroyBuilding
#define MESSAGE_ID      404
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleCompleteBuildingImmediately
#define MESSAGE_ID      405
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleQueryIndividualBuildingInfo
#define MESSAGE_ID      406
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (buildingBaseId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleUpgradeTech
#define MESSAGE_ID      407
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (techId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleCancelTechMission
#define MESSAGE_ID      408
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (techId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleCompleteTechImmediately
#define MESSAGE_ID      409
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (techId)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_CastleQueryIndividualTechInfo
#define MESSAGE_ID      410
#define MESSAGE_FIELDS  \
	FIELD_STRING        (mapObjectUuid)	\
	FIELD_VUINT         (techId)
#include <poseidon/cbpp/message_generator.hpp>

}

}

#endif
