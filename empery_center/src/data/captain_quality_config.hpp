#ifndef EMPERY_CENTER_DATA_CAPTAIN_QUALITY_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_QUALITY_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
		class CaptainQualityConfig
        {

            public:

				static const boost::shared_ptr<const CaptainQualityConfig> get(std::uint64_t quality);

            public:
				std::uint64_t  quality;
				std::uint64_t  feature_num;
		//		boost::container::flat_map<std::string, std::uint64_t> quality_resource;
        };
    }
}
#endif //
