#ifndef EMPERY_CENTER_MSG_KILL_HPP_
#define EMPERY_CENTER_MSG_KILL_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter {

namespace Msg {
	using namespace Poseidon::Cbpp::StatusCodes;

	enum {
		KILL_SESSION_GHOSTED        = -5001,
		KILL_OPERATOR_COMMAND       = -5002,
	};
}

}

#endif
