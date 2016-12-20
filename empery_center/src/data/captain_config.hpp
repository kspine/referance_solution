#ifndef EMPERY_CENTER_DATA_CAPTAIN_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
        class CaptainConfig
        {

            public:

				static const boost::shared_ptr<const CaptainConfig> get(std::uint64_t general_id);

            public:
				std::uint64_t  general_id;

				std::uint64_t  begin_quality;

				Poseidon::JsonObject basic_properties_array;

				boost::container::flat_map<std::uint64_t, boost::container::flat_map<std::string, std::uint64_t> >   quality_need;

				boost::container::flat_map<std::uint64_t, boost::container::flat_map<std::string, double> >   quality;

				boost::container::flat_map<std::uint64_t, boost::container::flat_map<std::string, double> >   star;
        };
    }
}
#endif //
