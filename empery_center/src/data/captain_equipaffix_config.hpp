#ifndef EMPERY_CENTER_DATA_CAPTAIN_EQUIPAFFIX_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_EQUIPAFFIX_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
		class CaptainEquipaffixConfig
        {

            public:

				static const boost::shared_ptr<const CaptainEquipaffixConfig> get(std::uint64_t affix_id);

            public:
				std::uint64_t  affix_id;

				std::uint64_t  properties_id;

				std::uint64_t  level;

				std::uint64_t  position;

				std::uint64_t  probability;

				boost::uint32_t  properties_range_min;

				boost::uint32_t  properties_range_max;

			public:
				static void get_affix_by_level_pos(std::vector<boost::shared_ptr<CaptainEquipaffixConfig> > &ret, std::uint64_t  level, std::uint64_t  position, std::uint64_t count);

        };
    }
}
#endif //
