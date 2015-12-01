#ifndef EMPERY_CENTER_MSG_ERR_CASTLE_HPP_
#define EMPERY_CENTER_MSG_ERR_CASTLE_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter {

namespace Msg {
	using namespace Poseidon::Cbpp::StatusCodes;

	enum {
		ERR_NO_SUCH_CASTLE                  = 71401,
		ERR_NOT_CASTLE_OWNER                = 71402,
		ERR_NO_SUCH_CASTLE_BASE             = 71403,
		ERR_ANOTHER_BUILDING_THERE          = 71404,
		ERR_BUILDING_NOT_ALLOWED            = 71405,
		ERR_NO_BUILDING_MISSION             = 71406,
		ERR_BUILDING_MISSION_CONFLICT       = 71407,
		ERR_CASTLE_NO_ENOUGH_RESOURCES      = 71408,
		ERR_BUILDING_UPGRADE_MAX            = 71409,
		ERR_NO_BUILDING_THERE               = 71410,
		ERR_BUILDING_NOT_DESTRUCTIBLE       = 71411,
		ERR_PREREQUISITE_NOT_MET            = 71412,
		ERR_NO_TECH_MISSION                 = 71413,
		ERR_TECH_MISSION_CONFLICT           = 71414,
		ERR_TECH_UPGRADE_MAX                = 71415,
		ERR_NO_SUCH_BUILDING                = 71416,
		ERR_NO_SUCH_TECH                    = 71417,
		ERR_DISPLAY_PREREQUISITE_NOT_MET    = 71418,
		ERR_BUILD_LIMIT_EXCEEDED            = 71419,
		ERR_CASTLE_HAS_NO_MAP_CELL          = 71420,
		ERR_CASTLE_LEVEL_TOO_LOW            = 71421,
		ERR_MAX_NUMBER_OF_IMMIGRANT_GROUPS  = 71422,
		ERR_NO_ROOM_FOR_NEW_UNIT            = 71423,
	};
}

}

#endif
