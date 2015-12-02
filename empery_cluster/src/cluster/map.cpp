#include "../precompiled.hpp"
#include "common.hpp"
#include "../../../empery_center/src/msg/sk_map.hpp"
#include "../../../empery_center/src/msg/err_map.hpp"
#include "../singletons/world_map.hpp"
#include "../map_cell.hpp"
#include "../map_object.hpp"

namespace EmperyCluster {

CLUSTER_SERVLET(Msg::SK_MapClusterRegistrationSucceeded, cluster, req){
	const auto scope = Rectangle(req.cluster_x, req.cluster_y, req.width, req.height);
	LOG_EMPERY_CLUSTER_INFO("Cluster server registered successfully: scope = ", scope);

	WorldMap::set_cluster(cluster, scope);

	return Response();
}

CLUSTER_SERVLET(Msg::SK_MapAddMapCell, cluster, req){
	const auto coord              = Coord(req.x, req.y);
	const auto parent_object_uuid = MapObjectUuid(req.parent_object_uuid);
	const auto owner_uuid         = AccountUuid(req.owner_uuid);

	boost::container::flat_map<AttributeId, boost::int64_t> attributes;
	attributes.reserve(req.attributes.size());
	for(auto it = req.attributes.begin(); it != req.attributes.end(); ++it){
		attributes.emplace(AttributeId(it->attribute_id), it->value);
	}

	LOG_EMPERY_CLUSTER_TRACE("Creating map cell: coord = ", coord,
		", parent_object_uuid = ", parent_object_uuid, ", owner_uuid = ", owner_uuid);
	const auto map_cell = boost::make_shared<MapCell>(coord, parent_object_uuid, owner_uuid, std::move(attributes));
	WorldMap::replace_map_cell_no_synchronize(cluster, map_cell);

	return Response();
}

CLUSTER_SERVLET(Msg::SK_MapAddMapObject, cluster, req){
	const auto map_object_uuid    = MapObjectUuid(req.map_object_uuid);

	auto map_object = WorldMap::get_map_object(map_object_uuid);
	if(map_object){
		const auto old_cluster = WorldMap::get_cluster(map_object->get_coord());
		if(old_cluster != cluster){
			// 替换旧的。
			map_object.reset();
		}
	}
	if(!map_object){
		const auto map_object_type_id = MapObjectTypeId(req.map_object_type_id);
		const auto owner_uuid         = AccountUuid(req.owner_uuid);
		const auto coord              = Coord(req.x, req.y);

		boost::container::flat_map<AttributeId, boost::int64_t> attributes;
		attributes.reserve(req.attributes.size());
		for(auto it = req.attributes.begin(); it != req.attributes.end(); ++it){
			attributes.emplace(AttributeId(it->attribute_id), it->value);
		}

		LOG_EMPERY_CLUSTER_DEBUG("Creating map object: map_object_uuid = ", map_object_uuid,
			", map_object_type_id = ", map_object_type_id, ", owner_uuid = ", owner_uuid, ", coord = ", coord);
		map_object = boost::make_shared<MapObject>(map_object_uuid, map_object_type_id, owner_uuid, coord, std::move(attributes));
		WorldMap::replace_map_object_no_synchronize(cluster, map_object);
	}

	return Response();
}

CLUSTER_SERVLET(Msg::SK_MapRemoveMapObject, cluster, req){
	const auto map_object_uuid = MapObjectUuid(req.map_object_uuid);

	LOG_EMPERY_CLUSTER_DEBUG("Removing map object: map_object_uuid = ", map_object_uuid);
	WorldMap::remove_map_object_no_synchronize(cluster, map_object_uuid);

	return Response();
}

CLUSTER_SERVLET(Msg::SK_MapSetAction, cluster, req){
	const auto map_object_uuid    = MapObjectUuid(req.map_object_uuid);
	const auto attack_target_uuid = MapObjectUuid(req.attack_target_uuid);

	const auto map_object = WorldMap::get_map_object(map_object_uuid);
	if(!map_object){
		return Response(Msg::ERR_NO_SUCH_MAP_OBJECT) <<map_object_uuid;
	}

	const auto from_coord = Coord(req.x, req.y);
	std::deque<MapObject::Waypoint> waypoints;
	for(auto it = req.waypoints.begin(); it != req.waypoints.end(); ++it){
		waypoints.emplace_back(it->delay, it->dx, it->dy);
	}
	map_object->set_action(from_coord, std::move(waypoints), attack_target_uuid);

	return Response();
}

}
