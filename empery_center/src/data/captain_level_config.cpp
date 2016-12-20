#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_level_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainLevelConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainLevelConfig>;
		boost::weak_ptr<const CaptainLevelConfigContainer> g_CaptainLevelConfig_container;
        const char CAPTAIN_LEVEL_CONFIG_FILE[] = "general_level";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_LEVEL_CONFIG_FILE);
			const auto CaptainLevelConfig_container = boost::make_shared<CaptainLevelConfigContainer>();
            while(csv.fetch_row())
            {
				Data::CaptainLevelConfig elem = {};

				csv.get(elem.level, "level");
				csv.get(elem.level_resource, "level_resource");

				if (!CaptainLevelConfig_container->emplace(elem.level, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainLevelConfig config: level = ", elem.level);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainLevelConfig config"));
                }
            }
			g_CaptainLevelConfig_container = CaptainLevelConfig_container;
			handles.push(CaptainLevelConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_LEVEL_CONFIG_FILE, Data::encode_csv_as_json(csv, "level"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainLevelConfig> CaptainLevelConfig::get(std::uint64_t level)
        {
            PROFILE_ME;

			const auto CaptainLevelConfig_container = g_CaptainLevelConfig_container.lock();
			if (!CaptainLevelConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainLevelConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainLevelConfig_container->find(level);
			if (it == CaptainLevelConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainLevelConfig config not found: slot = ", level);
                return { };
             }

			return boost::shared_ptr<const CaptainLevelConfig>(CaptainLevelConfig_container, &(it->second));
        }

    }
}