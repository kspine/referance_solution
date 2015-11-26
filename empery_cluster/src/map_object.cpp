#include "precompiled.hpp"
#include "map_object.hpp"
#include "cluster_client.hpp"
#include "singletons/world_map.hpp"
#include "checked_arithmetic.hpp"

namespace EmperyCluster {

MapObject::MapObject(MapObjectUuid map_object_uuid, MapObjectTypeId map_object_type_id, AccountUuid owner_uuid,
	Coord coord, boost::container::flat_map<AttributeId, boost::int64_t> attributes)
	: m_map_object_uuid(map_object_uuid), m_map_object_type_id(map_object_type_id), m_owner_uuid(owner_uuid)
	, m_coord(coord), m_attributes(std::move(attributes))
	, m_last_step_time(0)
{
}
MapObject::~MapObject(){
}

void MapObject::pump_status(){
	PROFILE_ME;
	LOG_EMPERY_CLUSTER_TRACE("Map object timer: map_object_uuid = ", get_map_object_uuid());

	const auto now = Poseidon::get_fast_mono_clock();

	// 检查移动。
	for(;;){
		if(m_waypoints.empty()){
			break;
		}
		const auto &waypoint = m_waypoints.front();

		const auto due_time = saturated_add(m_last_step_time, waypoint.delay);
		if(now < due_time){
			break;
		}
		const auto old_coord = get_coord();
		const auto new_coord = Coord(old_coord.x() + waypoint.dx, old_coord.y() + waypoint.dy);

		LOG_EMPERY_CLUSTER_DEBUG("Setting new coord: map_object_uuid = ", get_map_object_uuid(), ", new_coord = ", new_coord);
		set_coord(new_coord);

		m_waypoints.pop_front();
		m_last_step_time = due_time;
	}

	// TODO AI
}

Coord MapObject::get_coord() const {
	return m_coord;
}
void MapObject::set_coord(Coord coord){
	PROFILE_ME;

	if(get_coord() == coord){
		return;
	}
	m_coord = coord;

	WorldMap::update_map_object(virtual_shared_from_this<MapObject>(), false);
}

boost::int64_t MapObject::get_attribute(AttributeId map_object_attr_id) const {
	PROFILE_ME;

	const auto it = m_attributes.find(map_object_attr_id);
	if(it == m_attributes.end()){
		return 0;
	}
	return it->second;
}
void MapObject::get_attributes(boost::container::flat_map<AttributeId, boost::int64_t> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_attributes.size());
	for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it){
		ret[it->first] = it->second;
	}
}
void MapObject::set_attributes(const boost::container::flat_map<AttributeId, boost::int64_t> &modifiers){
	PROFILE_ME;

	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		m_attributes.emplace(it->first, 0);
	}

	bool dirty = false;
	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		auto &value = m_attributes.at(it->first);
		if(value == it->second){
			continue;
		}
		value = it->second;
		++dirty;
	}
	if(!dirty){
	    return;
	}

	WorldMap::update_map_object(virtual_shared_from_this<MapObject>(), false);
}

void MapObject::set_waypoints(Coord from_coord, std::deque<Waypoint> waypoints){
	PROFILE_ME;

	set_coord(from_coord);

	m_waypoints = std::move(waypoints);
	m_last_step_time = Poseidon::get_fast_mono_clock();
}

void MapObject::set_attack_target_uuid(MapObjectUuid attack_target_uuid){
	PROFILE_ME;

	m_attack_target_uuid = attack_target_uuid;
}

}
