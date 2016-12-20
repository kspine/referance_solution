#include "../precompiled.hpp"
#include "../data_session.hpp"
#include "captain_equipaffix_config.hpp"
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>
#include <poseidon/multi_index_map.hpp>


namespace EmperyCenter
{
    namespace
    {
		struct CaptainEquipaffixConfigElement {
			boost::shared_ptr<Data::CaptainEquipaffixConfig> affix;

			std::uint64_t  affix_id;
			std::uint64_t position;
			std::uint64_t level;

			explicit CaptainEquipaffixConfigElement(boost::shared_ptr<Data::CaptainEquipaffixConfig> affix_)
				: affix(std::move(affix_))
				, affix_id(affix->affix_id)
				, position(affix->position)
				, level(affix->level)
			{
			}
		};

		MULTI_INDEX_MAP(CaptainEquipaffixConfigElementContainer, CaptainEquipaffixConfigElement,
			UNIQUE_MEMBER_INDEX(affix_id)
			MULTI_MEMBER_INDEX(position)
			MULTI_MEMBER_INDEX(level)
			)

		boost::shared_ptr<CaptainEquipaffixConfigElementContainer> g_CaptainEquipaffixConfigElement_map;

	//	using CaptainEquipaffixConfigContainer = boost::container::flat_map<std::uint64_t, Data::CaptainEquipaffixConfig>;
	//	boost::weak_ptr<const CaptainEquipaffixConfigContainer> g_CaptainEquipaffixConfig_container;
		const char CAPTAIN_EQUIPAFFIX_CONFIG_FILE[] = "general_equipaffix";

        MODULE_RAII_PRIORITY(handles, 900)
        {
			struct TempCaptainElement {
				boost::shared_ptr<Data::CaptainEquipaffixConfig> obj;
			};
			std::map<std::uint64_t, TempCaptainElement> temp_captain_map;


			const auto CaptainEquipaffixConfig_container = boost::make_shared<CaptainEquipaffixConfigElementContainer>();

			auto csv = Data::sync_load_data(CAPTAIN_EQUIPAFFIX_CONFIG_FILE);
		//	const auto CaptainEquipaffixConfig_container = boost::make_shared<CaptainEquipaffixConfigContainer>();
			while (csv.fetch_row())
			{
				/*
				Data::CaptainEquipaffixConfig elem = {};

				csv.get(elem.affix_id, "affix_id");
				csv.get(elem.properties_id, "properties_id");
				csv.get(elem.level, "level");
				csv.get(elem.position, "position");
				csv.get(elem.probability, "probability");

				Poseidon::JsonArray array;
				csv.get(array, "properties_range");
				elem.properties_range_min = static_cast<boost::uint32_t>(array.at(0).get<double>());
				elem.properties_range_max = static_cast<boost::uint32_t>(array.at(1).get<double>());

				if (!CaptainEquipaffixConfig_container->emplace(elem.affix_id, std::move(elem)).second)
                {
					LOG_EMPERY_CENTER_ERROR("Duplicate CaptainEquipaffixConfig config: affix_id = ", elem.affix_id);
                    DEBUG_THROW(Exception, sslit("Duplicate CaptainEquipaffixConfig config"));
                }

				*/

				auto obj = boost::make_shared<Data::CaptainEquipaffixConfig>();

				csv.get(obj->affix_id, "affix_id");
				csv.get(obj->properties_id, "properties_id");
				csv.get(obj->level, "level");
				csv.get(obj->position, "position");
				csv.get(obj->probability, "probability");

				Poseidon::JsonArray array;
				csv.get(array, "properties_range");
				obj->properties_range_min = static_cast<boost::uint32_t>(array.at(0).get<double>());
				obj->properties_range_max = static_cast<boost::uint32_t>(array.at(1).get<double>());

				/*
				obj->affix_id = elem.affix_id;
				obj->level = elem.level;
				obj->properties_id = elem.properties_id;
				obj->position = elem.position;
				obj->probability = elem.probability;
				obj->properties_range_min = elem.properties_range_min;
				obj->properties_range_max = elem.properties_range_max;
				*/
				CaptainEquipaffixConfig_container->insert(CaptainEquipaffixConfigElement(std::move(obj)));
            }

			g_CaptainEquipaffixConfigElement_map = CaptainEquipaffixConfig_container;

		//	g_CaptainEquipaffixConfig_container = CaptainEquipaffixConfig_container;
			handles.push(CaptainEquipaffixConfig_container);
			auto servlet = DataSession::create_servlet(CAPTAIN_EQUIPAFFIX_CONFIG_FILE, Data::encode_csv_as_json(csv, "affix_id"));
            handles.push(std::move(servlet));
        }
    }

    namespace Data
    {

		const boost::shared_ptr<const CaptainEquipaffixConfig> CaptainEquipaffixConfig::get(std::uint64_t affix_id)
        {
            PROFILE_ME;

			const auto& CaptainEquipaffixConfigElement_map = g_CaptainEquipaffixConfigElement_map;
			if (!CaptainEquipaffixConfigElement_map)
			{
                    LOG_EMPERY_CENTER_WARNING("CaptainEquipaffixConfig config map has not been loaded.");
                    return { };
             }

			const auto it = CaptainEquipaffixConfigElement_map->find<0>(affix_id);
			if (it == CaptainEquipaffixConfigElement_map->end<0>()){
				LOG_EMPERY_CENTER_WARNING("CaptainEquipaffixConfig config not found: slot = ", affix_id);
                return { };
             }

			return it->affix;

        }

		void CaptainEquipaffixConfig::get_affix_by_level_pos(std::vector<boost::shared_ptr<CaptainEquipaffixConfig> > &ret, std::uint64_t level, std::uint64_t position, std::uint64_t count)
		{
			PROFILE_ME;

			const auto& CaptainEquipaffixConfigElement_map = g_CaptainEquipaffixConfigElement_map;
			if (!CaptainEquipaffixConfigElement_map)
			{
				LOG_EMPERY_CENTER_WARNING("CaptainEquipaffixConfig config map has not been loaded.");
				return;
			}

			std::vector<boost::shared_ptr<CaptainEquipaffixConfig> > temp;
			const auto range = CaptainEquipaffixConfigElement_map->equal_range<1>(position);
			boost::uint32_t taotal_probability = 0;
			for (auto it = range.first; it != range.second; ++it){
				if (it->affix->level == level)
				{
					temp.emplace_back(it->affix);
					taotal_probability += it->affix->probability;
				}

			}

			LOG_EMPERY_CENTER_WARNING("get_affix_by_level_pos size===============", temp.size(), ",level=", level, ",position=", position, ",count=", count);
			unsigned ncount = 0;
			if (temp.size() > count)
			{
				// 打乱vector
				random_shuffle(temp.begin(), temp.end());
				for (auto it = temp.begin(); it != temp.end(); ++it)
				{
					if (ncount == count)
						break;

					auto nrand = Poseidon::rand32(boost::lexical_cast<boost::uint32_t>(1), boost::lexical_cast<boost::uint32_t>(100));
					LOG_EMPERY_CENTER_WARNING("random select size===============", ret.size(), ",nrand=", nrand, ",taotal_probability=", taotal_probability, ",probability=", (*it)->probability, ",ncount=", ncount);
					if (nrand <= (*it)->probability * 100 / taotal_probability)
					{
						ret.emplace_back(*it);
						ncount += 1;
					}
				}

				ret.reserve(ncount);
			}
			else
			{
				for (auto it = temp.begin(); it != temp.end(); ++it)
				{
					ret.emplace_back(*it);
					ncount += 1;
				}

				ret.reserve(ncount);
			}
		}

	}
}