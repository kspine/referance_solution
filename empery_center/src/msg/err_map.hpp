#ifndef EMPERY_CENTER_MSG_ERR_MAP_HPP_
#define EMPERY_CENTER_MSG_ERR_MAP_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter {

namespace Msg {
	using namespace Poseidon::Cbpp::StatusCodes;

	enum {
		ERR_BROKEN_PATH                         = 71301,
		ERR_MAP_OBJECT_INVALIDATED              = 71302,
		ERR_NO_SUCH_MAP_OBJECT                  = 71303,
		ERR_NOT_YOUR_MAP_OBJECT                 = 71304,
		ERR_NOT_MOVABLE_MAP_OBJECT              = 71305,
		ERR_CLUSTER_CONNECTION_LOST             = 71306,
		ERR_MAP_COORD_MISMATCH                  = 71307,
		ERR_MAP_OBJECT_ON_ANOTHER_CLUSTER       = 71308,
		ERR_MAP_OBJECT_IS_NOT_A_CASTLE          = 71309,
		ERR_MAP_CELL_ALREADY_HAS_AN_OWNER       = 71310,
		ERR_MAP_CELL_IS_TOO_FAR_AWAY            = 71311,
		ERR_TOO_MANY_MAP_CELLS                  = 71312,
		ERR_LAND_PURCHASE_TICKET_NOT_FOUND      = 71313,
		ERR_BAD_LAND_PURCHASE_TICKET_TYPE       = 71314,
		ERR_NO_LAND_PURCHASE_TICKET             = 71315,
		ERR_MAX_MAP_CELL_LEVEL_EXCEEDED         = 71316,
		ERR_NO_LAND_UPGRADE_TICKET              = 71317,
		ERR_RESOURCE_NOT_PRODUCIBLE             = 71318,
		ERR_NO_TICKET_ON_MAP_CELL               = 71319,
		ERR_NOT_YOUR_MAP_CELL                   = 71320,
		ERR_MAP_OBJECT_IS_NOT_IMMIGRANTS        = 71321,
		ERR_CANNOT_DEPLOY_IMMIGRANTS_HERE       = 71322,
		ERR_TOO_CLOSE_TO_ANOTHER_CASTLE         = 71323,
		ERR_NOT_ON_THE_SAME_MAP_SERVER          = 71324,
//		ERR_CANNOT_DEPLOY_ON_OVERLAY            = 71325,
//		ERR_UNPURCHASABLE_WITH_OVERLAY          = 71326,
//		ERR_OVERLAY_ALREADY_REMOVED             = 71327,
		ERR_ZERO_HARVEST_SPEED                  = 71328,
		ERR_UNKNOWN_MAP_OBJECT_ACTION           = 71329,
		ERR_BLOCKED_BY_OTHER_TERRITORY          = 71330,
		ERR_BLOCKED_BY_IMPASSABLE_MAP_CELL      = 71331,
		ERR_BLOCKED_BY_TROOPS                   = 71332,
		ERR_BLOCKED_BY_CASTLE                   = 71333,
		ERR_INVALID_ACTION_PARAM                = 71334,
		ERR_MAP_OBJECT_PARENT_GONE              = 71335,
		ERR_ACCELERATION_CARD_APPLIED           = 71336,
		ERR_CANNOT_DEPLOY_ON_TROOPS             = 71337,
		ERR_CANNOT_DEPLOY_ON_TERRITORY          = 71338,
		ERR_NOT_AT_MAP_EDGE                     = 71339,
		ERR_SWITCHED_CLUSTER                    = 71340,
		ERR_TOO_FAR_FROM_CASTLE                 = 71341,
		ERR_MAP_OBJECT_IS_NOT_GARRISONED        = 71342,
//		ERR_NO_SUCH_MAP_OBJECT_TYPE             = 71343,
		ERR_ATTACK_TARGET_LOST                  = 71344,
		ERR_MAP_OBJECT_IS_GARRISONED            = 71345,
		ERR_STRATEGIC_RESOURCE_ALREADY_REMOVED  = 71346,
		ERR_BLOCKED_BY_TROOPS_TEMPORARILY       = 71347,
		ERR_CARRIABLE_RESOURCE_LIMIT_EXCEEDED   = 71348,
		ERR_RESOURCE_NOT_HARVESTABLE            = 71349,
		ERR_CANNOT_DEPLOY_ON_STRATEGIC_RESOURCE = 71350,
		ERR_RESOURCE_CRATE_NOT_FOUND            = 71351,
		ERR_RESOURCE_CRATE_EMPTY                = 71352,
		ERR_DEFENSE_BUILDING_IS_TOO_FAR_AWAY    = 71353,
		ERR_SELF_UNDER_PROTECTION               = 71354,
		ERR_MAP_CELL_OCCUPIED                   = 71355,
		ERR_MAP_CELL_NOT_OCCUPIED               = 71356,
		ERR_MAP_CELL_UNDER_PROTECTION           = 71357,
		ERR_MAP_CELL_OCCUPATION_PROTECTION      = 71358,
		ERR_NOT_A_BUNKER                        = 71359,
		ERR_ARCHER_REQUIRED                     = 71360,
		ERR_BUNKER_FULL                         = 71361,
		ERR_BUNKER_EMPTY                        = 71362,
		ERR_BATTALION_IN_ANOTHER_BUNKER         = 71363,
		ERR_CANNOT_ATTACK_FRIENDLY_OBJECTS      = 71364,
		ERR_TEMPORARILY_INVULNERABLE            = 71365,
		ERR_CANNOT_DEPLOY_ON_RESOURCE_CRATES    = 71366,
		ERR_CANNOT_PLACE_DEFENSE_ON_CRATES      = 71367,
		ERR_MAP_SERVER_CONFLICT                 = 71368,
	};
}

}

#endif
