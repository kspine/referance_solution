#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>


namespace EmperyCenter
{
    namespace
    {
		using CaptainConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainConfig>;
		boost::weak_ptr<const CaptainConfigContainer> g_captainConfig_container;
        const char CAPTAIN_CONFIG_FILE[] = "general";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			auto csv = Data::sync_load_data(CAPTAIN_CONFIG_FILE);
			const auto captainConfig_container = boost::make_shared<CaptainConfigContainer>();
			while (csv.fetch_row())
			{
				Data::CaptainConfig elem = {};

				csv.get(elem.general_id, "general_id");
				csv.get(elem.basic_properties_array, "basic_properties");

				csv.get(elem.begin_quality, "begin_quality");

				Poseidon::JsonObject object;
				// 品质属性
				csv.get(object, "quality");

				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto collection_name = boost::lexical_cast<std::uint64_t>(it->first.get());

					auto elem1 = it->second.get<Poseidon::JsonArray>();

					boost::container::flat_map<std::string, double> map2;
					for (auto pit = elem1.begin(); pit != elem1.end(); ++pit)
					{

						auto elem2 = pit->get<Poseidon::JsonObject>();

						for (auto oit = elem2.begin(); oit != elem2.end(); ++oit)
						{
							auto name = oit->first.get();

							auto value = boost::lexical_cast<double>(oit->second.dump());

							if (!map2.emplace(boost::lexical_cast<std::string>(std::move(name)), boost::lexical_cast<double>(std::move(value))).second)
							{
								LOG_EMPERY_CENTER_ERROR("effect map: name = ", name);
								DEBUG_THROW(Exception, sslit("Duplicate open_effect map"));
							}

						}
					}

					if (!map2.empty())
					{
						if (!elem.quality.emplace(boost::lexical_cast<std::uint64_t>(std::move(collection_name)), std::move(map2)).second)
						{
							LOG_EMPERY_CENTER_ERROR("quality map: collection_name = ", collection_name);
							DEBUG_THROW(Exception, sslit("Duplicate quality map"));
						}
					}

				}

				// 星级属性
				csv.get(object, "star");

				for (auto it = object.begin(); it != object.end(); ++it)
				{
					auto collection_name = boost::lexical_cast<std::uint64_t>(it->first.get());

					auto elem1 = it->second.get<Poseidon::JsonArray>();

					boost::container::flat_map<std::string, double> map2;
					for (auto pit = elem1.begin(); pit != elem1.end(); ++pit)
					{

						auto elem2 = pit->get<Poseidon::JsonObject>();

						for (auto oit = elem2.begin(); oit != elem2.end(); ++oit)
						{
							auto name = oit->first.get();

							auto value = boost::lexical_cast<double>(oit->second.dump());

							if (!map2.emplace(boost::lexical_cast<std::string>(std::move(name)), boost::lexical_cast<double>(std::move(value))).second)
							{
								LOG_EMPERY_CENTER_ERROR("star map: name = ", name);
								DEBUG_THROW(Exception, sslit("Duplicate star map"));
							}

						}
					}

					if (!map2.empty())
					{
						if (!elem.star.emplace(boost::lexical_cast<std::uint64_t>(std::move(collection_name)), std::move(map2)).second)
						{
							LOG_EMPERY_CENTER_ERROR("star map: collection_name = ", collection_name);
							DEBUG_THROW(Exception, sslit("Duplicate star map"));
						}
					}

				}

				// 升品消耗
				csv.get(object, "quality_need");

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

							if (!map2.emplace(boost::lexical_cast<std::string>(std::move(name)), boost::lexical_cast<std::uint64_t>(std::move(value))).second)
							{
								LOG_EMPERY_CENTER_ERROR("quality_need map: name = ", name);
								DEBUG_THROW(Exception, sslit("Duplicate quality_need map"));
							}

						}
					}

					if (!map2.empty())
					{
						if (!elem.quality_need.emplace(boost::lexical_cast<std::uint64_t>(std::move(collection_name)), std::move(map2)).second)
						{
							LOG_EMPERY_CENTER_ERROR("quality_need map: collection_name = ", collection_name);
							DEBUG_THROW(Exception, sslit("Duplicate quality_need map"));
						}
					}

				}

				if (!captainConfig_container->emplace(elem.general_id, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainConfig config: general_id = ", elem.general_id);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainConfig config"));
                }

            }

			g_captainConfig_container = captainConfig_container;
			handles.push(captainConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_CONFIG_FILE, Data::encode_csv_as_json(csv, "general_id"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainConfig> CaptainConfig::get(std::uint64_t general_id)
        {
            PROFILE_ME;

			const auto captainConfig_container = g_captainConfig_container.lock();
			if (!captainConfig_container)
             {
                    LOG_EMPERY_CENTER_WARNING("CaptainConfig config map has not been loaded.");
                    return { };
             }

			const auto it = captainConfig_container->find(general_id);
			if (it == captainConfig_container->end())
            {
				LOG_EMPERY_CENTER_WARNING("CaptainConfig config not found: slot = ", general_id);
                return { };
             }

			return boost::shared_ptr<const CaptainConfig>(captainConfig_container, &(it->second));
        }

    }
}