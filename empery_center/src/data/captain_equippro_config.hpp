#ifndef EMPERY_CENTER_DATA_CAPTAIN_EQUIPPRO_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_EQUIPPRO_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
		class CaptainEquipproConfig{
		public:
			static const boost::shared_ptr<const CaptainEquipproConfig> get(std::uint64_t properties_id);
		public:
			std::uint64_t  properties_id;
			std::uint64_t  type;
			std::uint64_t  arm_id;
			std::uint64_t  attr_id;
        };
    }
}
#endif //
