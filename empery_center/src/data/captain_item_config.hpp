#ifndef EMPERY_CENTER_DATA_CAPTAIN_ITEM_CONFIG_IN_HPP_
#define EMPERY_CENTER_DATA_CAPTAIN_ITEM_CONFIG_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <string>
#include <poseidon/json.hpp>

namespace EmperyCenter
{
    namespace Data
    {
        class CaptainItemConfig
        {

            public:

				static const boost::shared_ptr<const CaptainItemConfig> get(std::uint64_t model_id);

            public:
				std::uint64_t  model_id;

				std::uint64_t  quality;

				std::uint64_t  level;

				std::uint64_t  position;

				std::uint64_t  set_id;

				std::uint64_t  affix_num;

				boost::container::flat_map<std::uint64_t, boost::container::flat_map<std::string, std::uint64_t> >   equipment_resolve;

				boost::container::flat_map<std::string, std::uint64_t> basic_properties;

				Poseidon::JsonObject basic_properties_array;

        };
    }
}
#endif //
