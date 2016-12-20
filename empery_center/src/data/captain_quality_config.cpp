#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_quality_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainQualityConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainQualityConfig>;
		boost::weak_ptr<const CaptainQualityConfigContainer> g_CaptainQualityConfig_container;
        const char CAPTAIN_QUALITY_CONFIG_FILE[] = "general_quality";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_QUALITY_CONFIG_FILE);
			const auto CaptainQualityConfig_container = boost::make_shared<CaptainQualityConfigContainer>();
            while(csv.fetch_row())
            {
				Data::CaptainQualityConfig elem = {};

				csv.get(elem.quality, "quality");
				csv.get(elem.feature_num, "feature_num");
				
				/*
				Poseidon::JsonObject object;

				// 升星所需条件
				csv.get(object, "quality_resource");
				elem.quality_resource.reserve(object.size());

				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto item_id = std::string(it->first.get());
					const auto count = static_cast<std::uint64_t>(it->second.get<double>());
					if (!elem.quality_resource.emplace(std::move(item_id), count).second)
					{
						LOG_EMPERY_CENTER_ERROR("Duplicate CaptainQualityConfig map: item_id = ", item_id);
						DEBUG_THROW(Exception, sslit("Duplicate CaptainQualityConfig map"));
					}
				}
				*/

				if (!CaptainQualityConfig_container->emplace(elem.quality, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainQualityConfig config: quality = ", elem.quality);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainQualityConfig config"));
                }
            }
			g_CaptainQualityConfig_container = CaptainQualityConfig_container;
			handles.push(CaptainQualityConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_QUALITY_CONFIG_FILE, Data::encode_csv_as_json(csv, "quality"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainQualityConfig> CaptainQualityConfig::get(std::uint64_t quality)
        {
            PROFILE_ME;

			const auto CaptainQualityConfig_container = g_CaptainQualityConfig_container.lock();
			if (!CaptainQualityConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainQualityConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainQualityConfig_container->find(quality);
			if (it == CaptainQualityConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainQualityConfig config not found: slot = ", quality);
                return { };
             }

			return boost::shared_ptr<const CaptainQualityConfig>(CaptainQualityConfig_container, &(it->second));
        }
    }
}