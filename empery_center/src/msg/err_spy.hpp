#ifndef EMPERY_CENTER_MSG_ERR_SPY_HPP_
#define EMPERY_CENTER_MSG_ERR_SPY_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter
{
	    namespace Msg
		{
			using namespace Poseidon::Cbpp::StatusCodes;
			enum
			{
				ERR_SPY_PROTECTION_PERIOD_OWNER = 81000,

				ERR_SPY_PROTECTIOM_PERIOD_OTHER = 81001,

				ERR_SPY_QUEUE_FULL              = 81002,
			};
		}
}

#endif
