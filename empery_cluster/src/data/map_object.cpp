#include "../precompiled.hpp"
#include "map_object.hpp"
#include <poseidon/multi_index_map.hpp>
#include <string.h>
#include <poseidon/csv_parser.hpp>
#include <poseidon/json.hpp>

namespace EmperyCluster {

namespace {
	MULTI_INDEX_MAP(MapObjectTypeMap, Data::MapObjectType,
		UNIQUE_MEMBER_INDEX(map_object_type_id)
	)
	boost::weak_ptr<const MapObjectTypeMap> g_map_object_map;
	const char MAP_OBJECT_TYPE_FILE[] = "Arm";

	MULTI_INDEX_MAP(MapObjectRelativeMap, Data::MapObjectRelative,
		UNIQUE_MEMBER_INDEX(category_id)
	)
	boost::weak_ptr<const MapObjectRelativeMap> g_map_object_relative_map;
	const char MAP_OBJECT_RELATIVE_FILE[] = "Arm_relative";

	const char MAP_OBJECT_TYPE_MONSTER_FILE[] = "monster";

	MULTI_INDEX_MAP(MapObjectTypeMonsterMap, Data::MapObjectTypeMonster,
	UNIQUE_MEMBER_INDEX(map_object_type_id)
	)

	boost::weak_ptr<const MapObjectTypeMonsterMap> g_map_object_type_monster_map;
	MODULE_RAII_PRIORITY(handles, 1000){
		auto csv = Data::sync_load_data(MAP_OBJECT_TYPE_FILE);
		const auto map_object_map         =  boost::make_shared<MapObjectTypeMap>();
		const auto map_object_monster_map =  boost::make_shared<MapObjectTypeMonsterMap>();
		while(csv.fetch_row()){
			Data::MapObjectType elem = { };
			csv.get(elem.map_object_type_id,                "arm_id");
			csv.get(elem.category_id,                       "arm_type");
			csv.get(elem.attack,                            "attack");
			csv.get(elem.defence,                           "defence");
			csv.get(elem.speed,                             "speed");
			csv.get(elem.shoot_range,                       "shoot_range");
			csv.get(elem.attack_speed,                      "attack_speed");
			csv.get(elem.attack_plus,                      "attack_plus");
			csv.get(elem.doge_rate,                         "arm_dodge");
			csv.get(elem.critical_rate,                     "arm_crit");
			csv.get(elem.critical_damage_plus_rate,         "arm_crit_damege");

			if(!map_object_map->insert(std::move(elem)).second){
				LOG_EMPERY_CLUSTER_ERROR("Duplicate MapObjectType: map_object_type_id = ", elem.map_object_type_id);
				DEBUG_THROW(Exception, sslit("Duplicate MapObjectType"));
			}
		}

		auto csvMonster = Data::sync_load_data(MAP_OBJECT_TYPE_MONSTER_FILE);
		while(csvMonster.fetch_row()){
			Data::MapObjectType elem = { };
			csvMonster.get(elem.map_object_type_id,                "arm_id");
			csvMonster.get(elem.category_id,                       "arm_type");
			csvMonster.get(elem.attack,                            "attack");
			csvMonster.get(elem.defence,                           "defence");
			csvMonster.get(elem.speed,                             "speed");
			csvMonster.get(elem.shoot_range,                       "shoot_range");
			csvMonster.get(elem.attack_speed,                      "attack_speed");
			csvMonster.get(elem.attack_plus,                      "attack_plus");
			csvMonster.get(elem.doge_rate,                         "arm_dodge");
			csvMonster.get(elem.critical_rate,                     "arm_crit");
			csvMonster.get(elem.critical_damage_plus_rate,         "arm_crit_damege");

			if(!map_object_map->insert(std::move(elem)).second){
				LOG_EMPERY_CLUSTER_ERROR("Duplicate MapObjectType: map_object_type_id = ", elem.map_object_type_id);
				DEBUG_THROW(Exception, sslit("Duplicate MapObjectType"));
			}
			Data::MapObjectTypeMonster monster_elem = {};
			csvMonster.get(monster_elem.map_object_type_id,                "arm_id");
			if(!map_object_monster_map->insert(std::move(monster_elem)).second){
				LOG_EMPERY_CLUSTER_ERROR("Duplicate MapObjectTypeMonster: map_object_type_id = ", monster_elem.map_object_type_id);
				DEBUG_THROW(Exception, sslit("Duplicate MapObjectTypeMonster"));
			}
		}

		g_map_object_map = map_object_map;
		g_map_object_type_monster_map = map_object_monster_map;
		handles.push(map_object_map);
		handles.push(map_object_monster_map);


		auto csvRelative = Data::sync_load_data(MAP_OBJECT_RELATIVE_FILE);
		const auto map_object_relative_map = boost::make_shared<MapObjectRelativeMap>();
		while(csvRelative.fetch_row()){
			Data::MapObjectRelative elem = { };
			csvRelative.get(elem.category_id,         "arm_type");

			Poseidon::JsonObject object;
			csvRelative.get(object, "anti_relative");
			elem.arm_relative.reserve(object.size());
			for(auto it = object.begin(); it != object.end(); ++it){
				const auto relative_category_id = boost::lexical_cast<MapObjectWeaponId>(it->first);
				const auto relateive = it->second.get<double>();
				if(!elem.arm_relative.emplace(relative_category_id, relateive).second){
					LOG_EMPERY_CLUSTER_ERROR("Duplicate arm  relateive: category_id = ", elem.category_id , "relative_category_id =",relative_category_id);
					DEBUG_THROW(Exception, sslit("Duplicate category_id"));
				}
			}

			if(!map_object_relative_map->insert(std::move(elem)).second){
				LOG_EMPERY_CLUSTER_ERROR("Duplicate arm relative: map_object_category_id = ", elem.category_id);
				DEBUG_THROW(Exception, sslit("Duplicate MapObjectType"));
			}
		}
		g_map_object_relative_map = map_object_relative_map;
		handles.push(map_object_relative_map);
	}
}

namespace Data {
	boost::shared_ptr<const MapObjectType> MapObjectType::get(MapObjectTypeId map_object_type_id){
		PROFILE_ME;

		const auto map_object_map = g_map_object_map.lock();
		if(!map_object_map){
			LOG_EMPERY_CLUSTER_WARNING("MapObjectTypeMap has not been loaded.");
			return { };
		}

		const auto it = map_object_map->find<0>(map_object_type_id);
		if(it == map_object_map->end<0>()){
			LOG_EMPERY_CLUSTER_DEBUG("MapObjectType not found: map_object_type_id = ", map_object_type_id);
			return { };
		}
		return boost::shared_ptr<const MapObjectType>(map_object_map, &*it);
	}

	boost::shared_ptr<const MapObjectType> MapObjectType::require(MapObjectTypeId map_object_type_id){
		PROFILE_ME;

		auto ret = get(map_object_type_id);
		if(!ret){
			DEBUG_THROW(Exception, sslit("MapObjectType not found"));
		}
		return ret;
	}

	double MapObjectRelative::get_relative(MapObjectWeaponId map_object_category_id,MapObjectWeaponId relateive_category_id){
		PROFILE_ME;
		const auto map_object_relative_map = g_map_object_relative_map.lock();
		if(!map_object_relative_map){
			return 1.0;
		}

		const auto it = map_object_relative_map->find<0>(map_object_category_id);
		if(it == map_object_relative_map->end<0>()){
			return 1.0;
		}
		const auto relatives_map = (*it).arm_relative;
		const auto relative_it = relatives_map.find(relateive_category_id);
		if(relative_it == relatives_map.end()){
			return 1.0;
		}
		return (*relative_it).second;
	}

	boost::shared_ptr<const MapObjectTypeMonster> MapObjectTypeMonster::get(MapObjectTypeId map_object_type_id){
		PROFILE_ME;

		const auto map_object_monster_map = g_map_object_type_monster_map.lock();
		if(!map_object_monster_map){
			LOG_EMPERY_CLUSTER_WARNING("MapObjectTypeMonsterMap has not been loaded.");
			return { };
		}

		const auto it = map_object_monster_map->find<0>(map_object_type_id);
		if(it == map_object_monster_map->end<0>()){
			return { };
		}
		return boost::shared_ptr<const MapObjectTypeMonster>(map_object_monster_map, &*it);
	}

}

}
