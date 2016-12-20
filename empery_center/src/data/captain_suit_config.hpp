#ifndef EMPERY_CENTER_DATA_CAPTAIN_SUIT_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_SUIT_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
        class CaptainSuitConfig
        {

            public:

				static const boost::shared_ptr<const CaptainSuitConfig> get(std::uint64_t set_id);

            public:
				std::uint64_t  set_id;

				boost::container::flat_map<std::uint64_t, std::uint64_t>   set_equip;

				boost::container::flat_map<std::uint64_t, std::uint64_t>   set_effect;
        };
    }
}
#endif //
