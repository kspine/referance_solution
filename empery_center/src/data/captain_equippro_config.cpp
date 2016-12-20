#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_equippro_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainEquipproConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainEquipproConfig>;
		boost::weak_ptr<const CaptainEquipproConfigContainer> g_CaptainEquipproConfig_container;
        const char CAPTAIN_EQUIPPRO_CONFIG_FILE[] = "general_equippro";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_EQUIPPRO_CONFIG_FILE);
			const auto CaptainEquipproConfig_container = boost::make_shared<CaptainEquipproConfigContainer>();
			while (csv.fetch_row())
			{
				Data::CaptainEquipproConfig elem = {};

				csv.get(elem.properties_id, "properties_id");
				csv.get(elem.type, "type");
				csv.get(elem.arm_id, "arm_id");
				csv.get(elem.attr_id, "effect");

				if (!CaptainEquipproConfig_container->emplace(elem.properties_id, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainEquipproConfig config: properties_id = ", elem.properties_id);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainEquipproConfig config"));
                }

            }

			g_CaptainEquipproConfig_container = CaptainEquipproConfig_container;
			handles.push(CaptainEquipproConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_EQUIPPRO_CONFIG_FILE, Data::encode_csv_as_json(csv, "properties_id"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainEquipproConfig> CaptainEquipproConfig::get(std::uint64_t properties_id)
        {
            PROFILE_ME;

			const auto CaptainEquipproConfig_container = g_CaptainEquipproConfig_container.lock();
			if (!CaptainEquipproConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainEquipproConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainEquipproConfig_container->find(properties_id);
			if (it == CaptainEquipproConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainEquipproConfig config not found: slot = ", properties_id);
                return { };
             }

			return boost::shared_ptr<const CaptainEquipproConfig>(CaptainEquipproConfig_container, &(it->second));
        }

    }
}