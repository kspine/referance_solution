#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_item_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainItemConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainItemConfig>;
		boost::weak_ptr<const CaptainItemConfigContainer> g_CaptainItemConfig_container;
        const char CAPTAIN_ITEM_CONFIG_FILE[] = "general_equipmodel";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_ITEM_CONFIG_FILE);
			const auto CaptainItemConfig_container = boost::make_shared<CaptainItemConfigContainer>();
            while(csv.fetch_row())
            {
				Data::CaptainItemConfig elem = {};

				csv.get(elem.model_id, "model_id");
				csv.get(elem.quality, "quality");
				csv.get(elem.level, "level");

				csv.get(elem.position, "position");
				csv.get(elem.set_id, "set_id");
				csv.get(elem.affix_num, "affix_num");

				Poseidon::JsonObject object;

				// 装备拆解
				csv.get(object, "resolve");
				elem.equipment_resolve.reserve(object.size());

				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto collection_name = boost::lexical_cast<std::uint64_t>(it->first.get());

					auto elem1 = it->second.get<Poseidon::JsonArray>();

					boost::container::flat_map<std::string, std::uint64_t> map2;
					for (auto pit = elem1.begin(); pit != elem1.end(); ++pit)
					{

						auto elem2 = pit->get<Poseidon::JsonObject>();

						for (auto oit = elem2.begin(); oit != elem2.end(); ++oit)
						{
							auto name = oit->first.get();

							auto value = boost::lexical_cast<std::uint64_t>(oit->second.dump());

							if (!map2.emplace(std::move(name), std::move(value)).second)
							{
								LOG_EMPERY_CENTER_ERROR("equipment_resolve map: name = ", name);
								DEBUG_THROW(Exception, sslit("Duplicate equipment_resolve map"));
							}

						}
					}

					if (!map2.empty())
					{
						if (!elem.equipment_resolve.emplace(boost::lexical_cast<std::uint64_t>(std::move(collection_name)), std::move(map2)).second)
						{
							LOG_EMPERY_CENTER_ERROR("equipment_resolve map: collection_name = ", collection_name);
							DEBUG_THROW(Exception, sslit("Duplicate equipment_resolve map"));
						}
					}

				}

				// 基础属性
				csv.get(object, "basic_properties");
				csv.get(elem.basic_properties_array, "basic_properties");

				elem.basic_properties.reserve(object.size());
				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto collection_name = std::string(it->first.get());
					const auto count = static_cast<std::uint64_t>(it->second.get<double>());
					if (!elem.basic_properties.emplace(std::move(collection_name), count).second)
					{
						LOG_EMPERY_CENTER_ERROR("Duplicate basic_properties map: collection_name = ", collection_name);
						DEBUG_THROW(Exception, sslit("Duplicate basic_properties map"));
					}
				}


				if (!CaptainItemConfig_container->emplace(elem.model_id, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainItemConfig config: model_id = ", elem.model_id);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainItemConfig config"));
                }
            }
			g_CaptainItemConfig_container = CaptainItemConfig_container;
			handles.push(CaptainItemConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_ITEM_CONFIG_FILE, Data::encode_csv_as_json(csv, "model_id"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainItemConfig> CaptainItemConfig::get(std::uint64_t model_id)
        {
            PROFILE_ME;

			const auto CaptainItemConfig_container = g_CaptainItemConfig_container.lock();
			if (!CaptainItemConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainItemConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainItemConfig_container->find(model_id);
			if (it == CaptainItemConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainItemConfig config not found: slot = ", model_id);
                return { };
             }

			return boost::shared_ptr<const CaptainItemConfig>(CaptainItemConfig_container, &(it->second));
        }

    }
}