#ifndef EMPERY_CENTER_DATA_CAPTAIN_LEVEL_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_LEVEL_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
        class CaptainLevelConfig
        {

            public:

				static const boost::shared_ptr<const CaptainLevelConfig> get(std::uint64_t level);

            public:
				std::uint64_t  level;

				std::uint64_t  level_resource;
        };
    }
}
#endif //
