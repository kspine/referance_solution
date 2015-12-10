#include "../precompiled.hpp"
#include "map_object_type.hpp"
#include <poseidon/multi_index_map.hpp>
#include <string.h>
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>
#include "../data_session.hpp"

namespace EmperyCenter {

namespace {
	MULTI_INDEX_MAP(MapObjectTypeMap, Data::MapObjectType,
		UNIQUE_MEMBER_INDEX(map_object_type_id)
	)
	boost::weak_ptr<const MapObjectTypeMap> g_map_object_type_map;
	const char MAP_OBJECT_TYPE_FILE[] = "Arm";

	MODULE_RAII_PRIORITY(handles, 1000){
		auto csv = Data::sync_load_data(MAP_OBJECT_TYPE_FILE);
		const auto map_object_type_map = boost::make_shared<MapObjectTypeMap>();
		while(csv.fetch_row()){
			Data::MapObjectType elem = { };

			csv.get(elem.map_object_type_id, "arm_id");

			double speed = 0;
			csv.get(speed,                   "speed");
			if(speed > 0){
				elem.ms_per_cell = std::round(1000.0 / speed);
			}

			if(!map_object_type_map->insert(std::move(elem)).second){
				LOG_EMPERY_CENTER_ERROR("Duplicate MapObjectType: map_object_type_id = ", elem.map_object_type_id);
				DEBUG_THROW(Exception, sslit("Duplicate MapObjectType"));
			}
		}
		g_map_object_type_map = map_object_type_map;
		handles.push(map_object_type_map);
		auto servlet = DataSession::create_servlet(MAP_OBJECT_TYPE_FILE, Data::encode_csv_as_json(csv, "arm_id"));
		handles.push(std::move(servlet));
	}
}

namespace Data {
	boost::shared_ptr<const MapObjectType> MapObjectType::get(MapObjectTypeId map_object_type_id){
		PROFILE_ME;

		const auto map_object_type_map = g_map_object_type_map.lock();
		if(!map_object_type_map){
			LOG_EMPERY_CENTER_WARNING("MapObjectTypeMap has not been loaded.");
			return { };
		}

		const auto it = map_object_type_map->find<0>(map_object_type_id);
		if(it == map_object_type_map->end<0>()){
			LOG_EMPERY_CENTER_DEBUG("MapObjectType not found: map_object_type_id = ", map_object_type_id);
			return { };
		}
		return boost::shared_ptr<const MapObjectType>(map_object_type_map, &*it);
	}
	boost::shared_ptr<const MapObjectType> MapObjectType::require(MapObjectTypeId map_object_type_id){
		PROFILE_ME;

		auto ret = get(map_object_type_id);
		if(!ret){
			DEBUG_THROW(Exception, sslit("MapObjectType not found"));
		}
		return ret;
	}
}

}
