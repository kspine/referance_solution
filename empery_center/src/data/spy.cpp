#include "../precompiled.hpp"
#include "spy.hpp"
#include <poseidon/multi_index_map.hpp>
#include <string.h>
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>
#include "../data_session.hpp"

namespace EmperyCenter
{
	namespace
	{
		MULTI_INDEX_MAP(SpyDataContainer,Data::SpyData,
					UNIQUE_MEMBER_INDEX(spy_id))
	    
		boost::weak_ptr<const SpyDataContainer> g_spyData_container;
		
		const char SPY_DATA_FILE[]	= "spy_time";
		MODULE_RAII_PRIORITY(handles,100)
		{
			auto csv = Data::sync_load_data(SPY_DATA_FILE);
			const auto spyData_container =boost::make_shared<SpyDataContainer>();
			while (csv.fetch_row())
			{
				Data::SpyData elem = {};

				csv.get(elem.spy_id, "spy_id");

				csv.get(elem.spy_range_min, "spy_rangemin");

				csv.get(elem.spy_range_max, "spy_rangemax");

				csv.get(elem.spy_time, "spy_time");

				csv.get(elem.spy_front_time,"spy_fronttime");

				if (!spyData_container->insert(std::move(elem)).second)
				{
					DEBUG_THROW(Exception, sslit("Duplicate SpyDataContainer"));					
				}
			}
			g_spyData_container = spyData_container;
			handles.push(spyData_container);
			auto servlet = DataSession::create_servlet(SPY_DATA_FILE, Data::encode_csv_as_json(csv, "spy_id"));
			handles.push(std::move(servlet));
		}
	}

    namespace Data
	{
		boost::shared_ptr<const SpyData> SpyData::get (SpyId spy_id)
		{

		PROFILE_ME;

		const auto spyData_container = g_spyData_container.lock();
		if (!spyData_container)
		{
			LOG_EMPERY_CENTER_WARNING("SpyDataContainer has not been loaded.");
			return{};
		}

		const auto it = spyData_container->find<0>(spy_id);
		if (it == spyData_container->end<0>())
		{
			LOG_EMPERY_CENTER_TRACE("spy  not found: spy_id = ", spy_id);
			return{};
		}
		return boost::shared_ptr<const SpyData>(spyData_container, &*it);
	}

	boost::shared_ptr<const SpyData> SpyData::require (SpyId spy_id)
	{
		PROFILE_ME;
		auto ret = get(spy_id);
		if (!ret)
		{
			LOG_EMPERY_CENTER_WARNING("spy  not found: spy_id = ", spy_id);
			DEBUG_THROW(Exception, sslit("spy not found"));
		}
		return ret;
	}

	std::uint64_t SpyData::get_spy_time(std::uint64_t spy_distance)
	{
		const auto spyData_container = g_spyData_container.lock();
		if (!spyData_container)
		{
			LOG_EMPERY_CENTER_WARNING("SpyDataContainer has not been loaded.");
			return 0;
		}
		for(auto it = spyData_container->begin<0>();it != spyData_container->end<0>();++it)
		{
		  if(it->spy_range_min == 0)
			return (spy_distance - 0) * it->spy_time + it->spy_front_time;
		  else if(spy_distance >= it->spy_range_min && spy_distance <= it->spy_range_max)
			return (spy_distance - it->spy_range_min) * it->spy_time + it->spy_front_time;
		  else if(spy_distance >= it->spy_range_min)
			return (spy_distance - it->spy_range_min) * it->spy_time + it->spy_front_time;
		}
	  return 0;
	}
  }
}

