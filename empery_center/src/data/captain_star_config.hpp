#ifndef EMPERY_CENTER_DATA_CAPTAIN_STAR_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_STAR_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
        class CaptainStarConfig
        {

            public:

				static const boost::shared_ptr<const CaptainStarConfig> get(std::uint64_t star);

            public:
				std::uint64_t  star;

				boost::container::flat_map<std::string, std::uint64_t> star_resource;
        };
    }
}
#endif //
