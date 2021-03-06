#ifndef EMPERY_CENTER_LEGION_MEMBER_ATTRIBUTE_IDS_HPP_
#define EMPERY_CENTER_LEGION_MEMBER_ATTRIBUTE_IDS_HPP_

#include "id_types.hpp"

namespace EmperyCenter {
	namespace LegionMemberAttributeIds {
		constexpr LegionMemberAttributeId

			ID_BEGIN(0),
			ID_CUSTOM_PUBLIC_END(100),
			ID_CUSTOM_END(200),
			ID_PUBLIC_END(300),
			ID_END(500),

			ID_TITLEID(101),
			ID_DONATE(102),
			ID_SPEAKFLAG(103),
			ID_QUITWAITTIME(104),
			ID_KICKWAITTIME(105),
			ID_WEEKDONATE(106),

			ID_LEGION_PACKAGE_EXPIRE_FILTER(107),
			ID_LEGION_STORE_EXCHANGE_RECORD(108),

			ID_KICK_MANDATOR(109);
	}
}

#endif