#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_star_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainStarConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainStarConfig>;
		boost::weak_ptr<const CaptainStarConfigContainer> g_CaptainStarConfig_container;
        const char CAPTAIN_CONFIG_FILE[] = "general_star";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_CONFIG_FILE);
			const auto CaptainStarConfig_container = boost::make_shared<CaptainStarConfigContainer>();
            while(csv.fetch_row())
            {
				Data::CaptainStarConfig elem = {};

				csv.get(elem.star, "star");

				Poseidon::JsonObject object;

				// 升星所需条件
				csv.get(object, "star_resource");
				elem.star_resource.reserve(object.size());

				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto item_id = std::string(it->first.get());
					const auto count = static_cast<std::uint64_t>(it->second.get<double>());
					if (!elem.star_resource.emplace(std::move(item_id), count).second)
					{
						LOG_EMPERY_CENTER_ERROR("Duplicate CaptainStarConfig map: item_id = ", item_id);
						DEBUG_THROW(Exception, sslit("Duplicate CaptainStarConfig map"));
					}
				}

				if (!CaptainStarConfig_container->emplace(elem.star, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainStarConfig config: star = ", elem.star);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainStarConfig config"));
                }
            }
			g_CaptainStarConfig_container = CaptainStarConfig_container;
			handles.push(CaptainStarConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_CONFIG_FILE, Data::encode_csv_as_json(csv, "star"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainStarConfig> CaptainStarConfig::get(std::uint64_t star)
        {
            PROFILE_ME;

			const auto CaptainStarConfig_container = g_CaptainStarConfig_container.lock();
			if (!CaptainStarConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainStarConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainStarConfig_container->find(star);
			if (it == CaptainStarConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainStarConfig config not found: slot = ", star);
                return { };
             }

			return boost::shared_ptr<const CaptainStarConfig>(CaptainStarConfig_container, &(it->second));
        }
    }
}