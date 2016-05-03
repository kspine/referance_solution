#include "precompiled.hpp"
#include "map_utilities_center.hpp"
#include "map_utilities.hpp"
#include "cbpp_response.hpp"
#include "data/global.hpp"
#include "data/map.hpp"
#include "map_cell.hpp"
#include "overlay.hpp"
#include "map_object.hpp"
#include "strategic_resource.hpp"
#include "singletons/world_map.hpp"
#include "msg/err_map.hpp"
#include "map_object_type_ids.hpp"
#include "resource_crate.hpp"
#include <poseidon/json.hpp>

namespace EmperyCenter {

std::pair<long, std::string> can_place_defense_building_at(Coord coord){
	PROFILE_ME;

	// 检测阻挡。
	const auto map_cell = WorldMap::get_map_cell(coord);
	if(map_cell){
		const auto cell_owner_uuid = map_cell->get_owner_uuid();
		if(cell_owner_uuid){
			LOG_EMPERY_CENTER_TRACE("Blocked by a cell owned by another player's territory: cell_owner_uuid = ", cell_owner_uuid);
			return CbppResponse(Msg::ERR_BLOCKED_BY_OTHER_TERRITORY) <<cell_owner_uuid;
		}
	}

	const auto cluster_scope = WorldMap::get_cluster_scope(coord);
	const auto map_x = static_cast<unsigned>(coord.x() - cluster_scope.left());
	const auto map_y = static_cast<unsigned>(coord.y() - cluster_scope.bottom());
	const auto cell_data = Data::MapCellBasic::require(map_x, map_y);
	const auto terrain_id = cell_data->terrain_id;
	const auto terrain_data = Data::MapTerrain::require(terrain_id);
	if(!terrain_data->passable){
		LOG_EMPERY_CENTER_TRACE("Blocked by terrain: terrain_id = ", terrain_id);
		return CbppResponse(Msg::ERR_BLOCKED_BY_IMPASSABLE_MAP_CELL) <<terrain_id;
	}
	const unsigned border_thickness = Data::Global::as_unsigned(Data::Global::SLOT_MAP_BORDER_THICKNESS);
	if((map_x < border_thickness) || (map_x >= cluster_scope.width() - border_thickness) ||
		(map_y < border_thickness) || (map_y >= cluster_scope.height() - border_thickness))
	{
		LOG_EMPERY_CENTER_TRACE("Blocked by map border: coord = ", coord);
		return CbppResponse(Msg::ERR_BLOCKED_BY_IMPASSABLE_MAP_CELL) <<coord;
	}

	std::vector<boost::shared_ptr<MapObject>> adjacent_objects;
	WorldMap::get_map_objects_by_rectangle(adjacent_objects,
		Rectangle(Coord(coord.x() - 3, coord.y() - 3), Coord(coord.x() + 4, coord.y() + 4)));
	std::vector<Coord> foundation;
	for(auto it = adjacent_objects.begin(); it != adjacent_objects.end(); ++it){
		const auto &other_object = *it;
		const auto other_map_object_uuid = other_object->get_map_object_uuid();
		const auto other_coord = other_object->get_coord();
		if(coord == other_coord){
			LOG_EMPERY_CENTER_TRACE("Blocked by another map object: other_map_object_uuid = ", other_map_object_uuid);
//			if(wait_for_moving_objects && other_object->is_moving()){
//				return CbppResponse(Msg::ERR_BLOCKED_BY_TROOPS_TEMPORARILY) <<other_map_object_uuid;
//			}
			return CbppResponse(Msg::ERR_BLOCKED_BY_TROOPS) <<other_map_object_uuid;
		}
		const auto other_object_type_id = other_object->get_map_object_type_id();
		if(other_object_type_id == MapObjectTypeIds::ID_CASTLE){
			foundation.clear();
			get_castle_foundation(foundation, other_coord, false);
			for(auto fit = foundation.begin(); fit != foundation.end(); ++fit){
				if(coord == *fit){
					LOG_EMPERY_CENTER_TRACE("Blocked by castle: other_map_object_uuid = ", other_map_object_uuid);
					return CbppResponse(Msg::ERR_BLOCKED_BY_CASTLE) <<other_map_object_uuid;
				}
			}
		}
	}

	return CbppResponse();
}

std::pair<long, std::string> can_deploy_castle_at(Coord coord, MapObjectUuid excluding_map_object_uuid){
	PROFILE_ME;

	using Response = CbppResponse;

	std::vector<boost::shared_ptr<MapCell>> map_cells;
	std::vector<boost::shared_ptr<Overlay>> overlays;
	std::vector<boost::shared_ptr<MapObject>> map_objects;
	std::vector<boost::shared_ptr<StrategicResource>> strategic_resources;

	std::vector<Coord> foundation;
	get_castle_foundation(foundation, coord, true);
	for(auto it = foundation.begin(); it != foundation.end(); ++it){
		const auto &foundation_coord = *it;
		const auto cluster_scope = WorldMap::get_cluster_scope(foundation_coord);
		const auto map_x = static_cast<unsigned>(foundation_coord.x() - cluster_scope.left());
		const auto map_y = static_cast<unsigned>(foundation_coord.y() - cluster_scope.bottom());
		LOG_EMPERY_CENTER_DEBUG("Castle foundation: foundation_coord = ", foundation_coord, ", cluster_scope = ", cluster_scope,
			", map_x = ", map_x, ", map_y = ", map_y);
		const auto basic_data = Data::MapCellBasic::require(map_x, map_y);
		const auto terrain_data = Data::MapTerrain::require(basic_data->terrain_id);
		if(!terrain_data->buildable){
			return Response(Msg::ERR_CANNOT_DEPLOY_IMMIGRANTS_HERE) <<foundation_coord;
		}

		map_cells.clear();
		WorldMap::get_map_cells_by_rectangle(map_cells, Rectangle(foundation_coord, 1, 1));
		for(auto it = map_cells.begin(); it != map_cells.end(); ++it){
			const auto &map_cell = *it;
			if(!map_cell->is_virtually_removed()){
				return Response(Msg::ERR_CANNOT_DEPLOY_ON_TERRITORY) <<foundation_coord;
			}
		}

		overlays.clear();
		WorldMap::get_overlays_by_rectangle(overlays, Rectangle(foundation_coord, 1, 1));
		for(auto it = overlays.begin(); it != overlays.end(); ++it){
			const auto &overlay = *it;
			if(!overlay->is_virtually_removed()){
				return Response(Msg::ERR_CANNOT_DEPLOY_ON_OVERLAY) <<foundation_coord;
			}
		}

		map_objects.clear();
		WorldMap::get_map_objects_by_rectangle(map_objects, Rectangle(foundation_coord, 1, 1));
		for(auto it = map_objects.begin(); it != map_objects.end(); ++it){
			const auto &other_object = *it;
			if(other_object->get_map_object_uuid() == excluding_map_object_uuid){
				continue;
			}
			if(!other_object->is_virtually_removed()){
				return Response(Msg::ERR_CANNOT_DEPLOY_ON_TROOPS) <<foundation_coord;
			}
		}

		strategic_resources.clear();
		WorldMap::get_strategic_resources_by_rectangle(strategic_resources, Rectangle(foundation_coord, 1, 1));
		for(auto it = strategic_resources.begin(); it != strategic_resources.end(); ++it){
			const auto &strategic_resource = *it;
			if(!strategic_resource->is_virtually_removed()){
				return Response(Msg::ERR_CANNOT_DEPLOY_ON_STRATEGIC_RESOURCE) <<foundation_coord;
			}
		}
	}
	// 检测与其他城堡距离。
	const auto min_distance  = static_cast<std::uint32_t>(Data::Global::as_unsigned(Data::Global::SLOT_MINIMUM_DISTANCE_BETWEEN_CASTLES));

	const auto cluster_scope = WorldMap::get_cluster_scope(coord);
	const auto other_left    = std::max(coord.x() - (min_distance - 1), cluster_scope.left());
	const auto other_bottom  = std::max(coord.y() - (min_distance - 1), cluster_scope.bottom());
	const auto other_right   = std::min(coord.x() + (min_distance + 2), cluster_scope.right());
	const auto other_top     = std::min(coord.y() + (min_distance + 2), cluster_scope.top());
	map_objects.clear();
	WorldMap::get_map_objects_by_rectangle(map_objects, Rectangle(Coord(other_left, other_bottom), Coord(other_right, other_top)));
	for(auto it = map_objects.begin(); it != map_objects.end(); ++it){
		const auto &other_object = *it;
		const auto other_object_type_id = other_object->get_map_object_type_id();
		if(other_object_type_id != MapObjectTypeIds::ID_CASTLE){
			continue;
		}
		const auto other_coord = other_object->get_coord();
		const auto other_object_uuid = other_object->get_map_object_uuid();
		LOG_EMPERY_CENTER_DEBUG("Checking distance: other_coord = ", other_coord, ", other_object_uuid = ", other_object_uuid);
		const auto distance = get_distance_of_coords(other_coord, coord);
		if(distance <= min_distance){
			return Response(Msg::ERR_TOO_CLOSE_TO_ANOTHER_CASTLE) <<other_object_uuid;
		}
	}

	return Response();
}

void create_resource_crates(Coord origin, ResourceId resource_id, std::uint64_t amount,
	unsigned radius_inner, unsigned radius_outer)
{
	PROFILE_ME;
	LOG_EMPERY_CENTER_DEBUG("Creating resource crates: origin = ", origin, ", resource_id = ", resource_id, ", amount = ", amount,
		", radius_inner = ", radius_inner, ", radius_outer = ", radius_outer);

	const auto border_thickness            = Data::Global::as_unsigned(Data::Global::SLOT_MAP_BORDER_THICKNESS);
	const auto inner_amount_ratio          = Data::Global::as_double(Data::Global::SLOT_RESOURCE_CRATE_AMOUNT_INNER_RATIO);
	const auto separation_amount_threshold = Data::Global::as_unsigned(Data::Global::SLOT_RESOURCE_CRATE_SEPARATION_AMOUNT_THRESHOLD);
	const auto number_limits               = Data::Global::as_array(Data::Global::SLOT_RESOURCE_CRATE_NUMBER_LIMITS);
	const auto expiry_duration             = Data::Global::as_double(Data::Global::SLOT_RESOURCE_CRATE_RADIUS_EXPIRY_DURATION);

	const auto really_create_crates = [&](std::uint64_t &amount_remaining,
		unsigned radius_begin, unsigned radius_end, unsigned number_limit)
	{
		if(amount_remaining == 0){
			return;
		}
		unsigned crate_count;
		if(amount_remaining < separation_amount_threshold){
			crate_count = 1;
		} else {
			crate_count = Poseidon::rand32(2, number_limit);
		}
		LOG_EMPERY_CENTER_DEBUG("> amount_remaining = ", amount_remaining, ", crate_count = ", crate_count);
		if(crate_count == 0){
			LOG_EMPERY_CENTER_WARNING("Crate count is zero?");
			return;
		}
		const auto resource_amount_per_crate = amount_remaining / crate_count;

		std::vector<Coord> coords;
		coords.reserve(256);
		for(unsigned i = radius_begin; i < radius_end; ++i){
			get_surrounding_coords(coords, origin, i);
		}
		coords.erase(
			std::remove_if(coords.begin(), coords.end(),
				[&](Coord coord){
					const auto cluster_scope = WorldMap::get_cluster_scope(coord);
					const auto map_x = static_cast<unsigned>(coord.x() - cluster_scope.left());
					const auto map_y = static_cast<unsigned>(coord.y() - cluster_scope.bottom());
					const auto cell_data = Data::MapCellBasic::require(map_x, map_y);
					const auto terrain_id = cell_data->terrain_id;
					const auto terrain_data = Data::MapTerrain::require(terrain_id);
					if(!terrain_data->passable){
						return true;
					}
					if((map_x < border_thickness) || (map_x >= cluster_scope.width() - border_thickness) ||
						(map_y < border_thickness) || (map_y >= cluster_scope.height() - border_thickness))
					{
						return true;
					}

					std::vector<boost::shared_ptr<MapObject>> map_objects;
					WorldMap::get_map_objects_by_rectangle(map_objects, Rectangle(coord.x(), coord.y(), 2, 1));
					for(auto it = map_objects.begin(); it != map_objects.end(); ++it){
						const auto &other_object = *it;
						const auto other_object_type_id = other_object->get_map_object_type_id();
						if(other_object_type_id == MapObjectTypeIds::ID_CASTLE){
							return true;
						}
					}

					return false;
				}),
			coords.end());
		if(coords.empty()){
			LOG_EMPERY_CENTER_DEBUG("> No coords available.");
			return;
		}

		const auto utc_now = Poseidon::get_utc_time();

		for(unsigned i = 0; i < crate_count; ++i){
			try {
				const auto resource_crate_uuid = ResourceCrateUuid(Poseidon::Uuid::random());
				const auto coord = coords.at(Poseidon::rand32(0, coords.size()));
				const auto expiry_time = saturated_add(utc_now, static_cast<std::uint64_t>(expiry_duration * 60000));

				auto resource_crate = boost::make_shared<ResourceCrate>(resource_crate_uuid,
					resource_id, resource_amount_per_crate, coord, utc_now, expiry_time);
				WorldMap::insert_resource_crate(std::move(resource_crate));
				LOG_EMPERY_CENTER_DEBUG("> Created resource crate: resource_crate_uuid = ", resource_crate_uuid,
					", coord = ", coord, ", resource_id = ", resource_id, ", resource_amount_per_crate = ", resource_amount_per_crate);
				amount_remaining -= resource_amount_per_crate;
			} catch(std::exception &e){
				LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			}
		}
		// 消除误差。
		if(amount_remaining < crate_count){
			amount_remaining = 0;
		}
	};

	std::uint64_t amount_remaining = amount * (1 - inner_amount_ratio);
	really_create_crates(amount_remaining, radius_inner, radius_inner + radius_outer, number_limits.at(1).get<double>());
	LOG_EMPERY_CENTER_DEBUG("Outer crate creation complete: resource_id = ", resource_id, ", amount_remaining = ", amount_remaining);
	amount_remaining += amount * inner_amount_ratio;
	really_create_crates(amount_remaining, 0, radius_inner, number_limits.at(0).get<double>());
	LOG_EMPERY_CENTER_DEBUG("Inner crate creation complete: resource_id = ", resource_id, ", amount_remaining = ", amount_remaining);
}

}