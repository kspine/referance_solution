#ifndef EMPERY_CENTER_CAPTAIN_ATTRIBUTE_IDS_HPP_
#define EMPERY_CENTER_CAPTAIN_ATTRIBUTE_IDS_HPP_

#include "id_types.hpp"

namespace EmperyCenter {

	namespace CaptainAttributeIds {

		constexpr CaptainAttributeId

			ID_BEGIN(0),
			ID_CUSTOM_PUBLIC_END(100),
			ID_CUSTOM_END(200),
			ID_PUBLIC_END(300),
			ID_END(500),

			ID_BASEID(101),
			ID_LEVEL(102),
			ID_STARLEVEL(103),
			ID_CUREXP(104),
			ID_QUALITYLEVEL(105),
			ID_BASEATTRIBUTE(106),
			ID_SPECIALATTRIBUTE(107),
			ID_CURSTATUS(108),
			ID_RELATION_OBJECT_UUID(109),
			ID_EQUIP_ATTRIBUTE(110);

}

}

#endif