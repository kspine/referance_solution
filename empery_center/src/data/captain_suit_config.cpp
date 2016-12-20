#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_suit_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainSuitConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainSuitConfig>;
		boost::weak_ptr<const CaptainSuitConfigContainer> g_CaptainSuitConfig_container;
        const char CAPTAIN_SUIT_CONFIG_FILE[] = "general_set";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_SUIT_CONFIG_FILE);
			const auto CaptainSuitConfig_container = boost::make_shared<CaptainSuitConfigContainer>();
			while (csv.fetch_row())
			{
				Data::CaptainSuitConfig elem = {};

				csv.get(elem.set_id, "set_id");

				Poseidon::JsonArray arr;

				// 套装装备id
				csv.get(arr, "set_equip");
				elem.set_equip.reserve(arr.size());
				for (auto it = arr.begin(); it != arr.end(); ++it)
				{
					auto collection_name = boost::lexical_cast<std::uint64_t>(it->get<double>());
					if (!elem.set_equip.emplace(std::move(collection_name), collection_name).second)
					{
						LOG_EMPERY_CENTER_ERROR("Duplicate set_equip map: collection_name = ", collection_name);
						DEBUG_THROW(Exception, sslit("Duplicate set_equip map"));
					}
				}

				Poseidon::JsonObject object;
				// 套装装备效果
				csv.get(object, "set_effect");
				elem.set_effect.reserve(object.size());
				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto collection_name = boost::lexical_cast<std::uint64_t>(it->first.get());
					const auto count = boost::lexical_cast<std::uint64_t>(it->second.get<std::string>());
					if (!elem.set_effect.emplace(std::move(collection_name), count).second)
					{
						LOG_EMPERY_CENTER_ERROR("Duplicate set_effect map: collection_name = ", collection_name);
						DEBUG_THROW(Exception, sslit("Duplicate set_effect map"));
					}
				}

				if (!CaptainSuitConfig_container->emplace(elem.set_id, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainSuitConfig config: set_id = ", elem.set_id);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainSuitConfig config"));
                }

            }

			g_CaptainSuitConfig_container = CaptainSuitConfig_container;
			handles.push(CaptainSuitConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_SUIT_CONFIG_FILE, Data::encode_csv_as_json(csv, "set_id"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainSuitConfig> CaptainSuitConfig::get(std::uint64_t set_id)
        {
            PROFILE_ME;

			const auto CaptainSuitConfig_container = g_CaptainSuitConfig_container.lock();
			if (!CaptainSuitConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainSuitConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainSuitConfig_container->find(set_id);
			if (it == CaptainSuitConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainSuitConfig config not found: slot = ", set_id);
                return { };
             }

			return boost::shared_ptr<const CaptainSuitConfig>(CaptainSuitConfig_container, &(it->second));
        }

    }
}