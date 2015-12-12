#include "../precompiled.hpp"
#include "world_map.hpp"
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/timer_daemon.hpp>
#include <boost/container/flat_map.hpp>
#include "../../../empery_center/src/msg/ks_map.hpp"
#include "../coord.hpp"
#include "../map_cell.hpp"
#include "../map_object.hpp"
#include "../rectangle.hpp"
#include "../cluster_client.hpp"

namespace EmperyCluster {

namespace Msg {
	using namespace ::EmperyCenter::Msg;
}

namespace {
	struct MapCellElement {
		boost::shared_ptr<MapCell> map_cell;

		Coord coord;
		MapObjectUuid parent_object_uuid;
		boost::weak_ptr<ClusterClient> master;

		MapCellElement(boost::shared_ptr<MapCell> map_cell_, const boost::shared_ptr<ClusterClient> &master_)
			: map_cell(std::move(map_cell_))
			, coord(map_cell->get_coord()), parent_object_uuid(map_cell->get_parent_object_uuid())
			, master(master_)
		{
		}
	};

	MULTI_INDEX_MAP(MapCellMapContainer, MapCellElement,
		UNIQUE_MEMBER_INDEX(coord)
		MULTI_MEMBER_INDEX(parent_object_uuid)
		MULTI_MEMBER_INDEX(master)
	)

	boost::weak_ptr<MapCellMapContainer> g_map_cell_map;

	struct MapObjectElement {
		boost::shared_ptr<MapObject> map_object;

		MapObjectUuid map_object_uuid;
		Coord coord;
		AccountUuid owner_uuid;
		boost::weak_ptr<ClusterClient> master;

		MapObjectElement(boost::shared_ptr<MapObject> map_object_, const boost::shared_ptr<ClusterClient> &master_)
			: map_object(std::move(map_object_))
			, map_object_uuid(map_object->get_map_object_uuid()), coord(map_object->get_coord()), owner_uuid(map_object->get_owner_uuid())
			, master(master_)
		{
		}
	};

	MULTI_INDEX_MAP(MapObjectMapContainer, MapObjectElement,
		UNIQUE_MEMBER_INDEX(map_object_uuid)
		MULTI_MEMBER_INDEX(coord)
		MULTI_MEMBER_INDEX(owner_uuid)
		MULTI_MEMBER_INDEX(master)
	)

	boost::weak_ptr<MapObjectMapContainer> g_map_object_map;

	struct ClusterElement {
		Rectangle scope;

		Coord coord;
		boost::weak_ptr<ClusterClient> cluster;

		ClusterElement(Rectangle scope_, boost::weak_ptr<ClusterClient> cluster_)
			: scope(scope_)
			, coord(scope.bottom_left()), cluster(std::move(cluster_))
		{
		}
	};

	MULTI_INDEX_MAP(ClusterMapContainer, ClusterElement,
		UNIQUE_MEMBER_INDEX(coord)
		UNIQUE_MEMBER_INDEX(cluster)
	)

	boost::weak_ptr<ClusterMapContainer> g_cluster_map;

	MODULE_RAII_PRIORITY(handles, 5300){
		const auto map_cell_map = boost::make_shared<MapCellMapContainer>();
		g_map_cell_map = map_cell_map;
		handles.push(map_cell_map);

		const auto map_object_map = boost::make_shared<MapObjectMapContainer>();
		g_map_object_map = map_object_map;
		handles.push(map_object_map);

		const auto cluster_map = boost::make_shared<ClusterMapContainer>();
		g_cluster_map = cluster_map;
		handles.push(cluster_map);

		const auto gc_timer_interval = get_config<boost::uint64_t>("world_map_gc_timer_interval", 300000);
		auto timer = Poseidon::TimerDaemon::register_timer(0, gc_timer_interval,
			std::bind(
				[](const boost::weak_ptr<MapCellMapContainer> &map_cell_weak,
					const boost::weak_ptr<MapObjectMapContainer> &map_object_weak,
					const boost::weak_ptr<ClusterMapContainer> &cluster_weak)
				{
					PROFILE_ME;

					const auto cluster_map = cluster_weak.lock();
					if(!cluster_map){
						return;
					}

					const auto map_cell_map = map_cell_weak.lock();
					if(map_cell_map){
						auto it = map_cell_map->begin<2>();
						while(it != map_cell_map->end<2>()){
							const auto prev = it;
							it = map_cell_map->upper_bound<2>(prev->master);

							if(prev->master.expired()){
								LOG_EMPERY_CLUSTER_WARNING("Master is gone! Removing ", std::distance(prev, it), " map cell(s).");
								map_cell_map->erase<2>(prev, it);
							}
						}
					}

					const auto map_object_map = map_object_weak.lock();
					if(map_object_map){
						auto it = map_object_map->begin<3>();
						while(it != map_object_map->end<3>()){
							const auto prev = it;
							it = map_object_map->upper_bound<3>(prev->master);

							if(prev->master.expired()){
								LOG_EMPERY_CLUSTER_WARNING("Master is gone! Removing ", std::distance(prev, it), " map object(s).");
								map_object_map->erase<3>(prev, it);
							}
						}
					}
				},
				boost::weak_ptr<MapCellMapContainer>(map_cell_map),
				boost::weak_ptr<MapObjectMapContainer>(map_object_map),
				boost::weak_ptr<ClusterMapContainer>(cluster_map))
			);
		handles.push(std::move(timer));

		using InitServerMap = boost::container::flat_map<std::pair<boost::int64_t, boost::int64_t>, boost::weak_ptr<ClusterClient>>;
		InitServerMap init_servers;
		LOG_EMPERY_CLUSTER_INFO("Loading logical map servers...");
		const auto init_logical_map_servers = get_config_v<std::string>("init_logical_map_server");
		for(auto it = init_logical_map_servers.begin(); it != init_logical_map_servers.end(); ++it){
			const auto &str = *it;
			std::istringstream iss(str);
			boost::int64_t num_x, num_y;
			char comma;
			if(!(iss >>num_x >>comma >>num_y) || (comma != ',')){
				LOG_EMPERY_CLUSTER_ERROR("Invalid init_logical_map_server string: ", str);
				DEBUG_THROW(Exception, sslit("Invalid init_logical_map_server string"));
			}
			if(!init_servers.emplace(std::make_pair(num_x, num_y), boost::weak_ptr<ClusterClient>()).second){
				LOG_EMPERY_CLUSTER_ERROR("Duplicate init_logical_map_server entry: num_x = ", num_x, ", num_y = ", num_y);
				DEBUG_THROW(Exception, sslit("Duplicate init_logical_map_server entry"));
			}
			LOG_EMPERY_CLUSTER_DEBUG("> Logical server: num_x = ", num_x, ", num_y = ", num_y);
		}
		const auto client_timer_interval = get_config<boost::uint64_t>("cluster_client_reconnect_delay", 10000);
		timer = Poseidon::TimerDaemon::register_timer(0, client_timer_interval,
			std::bind(
				[](InitServerMap &init_servers){
					PROFILE_ME;

					for(auto it = init_servers.begin(); it != init_servers.end(); ++it){
						if(!it->second.expired()){
							continue;
						}
						const auto num_x = it->first.first;
						const auto num_y = it->first.second;
						LOG_EMPERY_CLUSTER_INFO("Creating logical map server: num_x = ", num_x, ", num_y = ", num_y);

						const auto cluster = ClusterClient::create(num_x, num_y);
						it->second = cluster;
					}
				},
				std::move(init_servers))
			);
		handles.push(std::move(timer));
	}

	void notify_cluster_map_object_updated(const boost::shared_ptr<MapObject> &map_object, const boost::shared_ptr<ClusterClient> &cluster){
		PROFILE_ME;

		boost::container::flat_map<AttributeId, boost::int64_t> attributes;
		map_object->get_attributes(attributes);

		Msg::KS_MapUpdateMapObject msg;
		msg.map_object_uuid    = map_object->get_map_object_uuid().str();
		msg.x                  = map_object->get_coord().x();
		msg.y                  = map_object->get_coord().y();
		msg.attributes.reserve(attributes.size());
		for(auto it = attributes.begin(); it != attributes.end(); ++it){
			msg.attributes.emplace_back();
			auto &attribute = msg.attributes.back();
			attribute.attribute_id = it->first.get();
			attribute.value        = it->second;
		}
		cluster->send(msg);
	}
	void notify_cluster_map_object_removed(const boost::shared_ptr<MapObject> &map_object, const boost::shared_ptr<ClusterClient> &cluster){
		PROFILE_ME;

		Msg::KS_MapRemoveMapObject msg;
		msg.map_object_uuid = map_object->get_map_object_uuid().str();
		cluster->send(msg);
	}
}

boost::shared_ptr<MapCell> WorldMap::get_map_cell(Coord coord){
	PROFILE_ME;

	const auto map_cell_map = g_map_cell_map.lock();
	if(!map_cell_map){
		LOG_EMPERY_CLUSTER_WARNING("Map cell map not loaded.");
		return { };
	}

	const auto it = map_cell_map->find<0>(coord);
	if(it == map_cell_map->end<0>()){
		LOG_EMPERY_CLUSTER_DEBUG("Map cell not found: coord = ", coord);
		return { };
	}
	if(it->master.expired()){
		LOG_EMPERY_CLUSTER_DEBUG("Master expired: coord = ", coord);
		const auto master = it->master;
		map_cell_map->erase<2>(master);
		return { };
	}
	return it->map_cell;
}
void WorldMap::replace_map_cell_no_synchronize(const boost::shared_ptr<ClusterClient> &master, const boost::shared_ptr<MapCell> &map_cell){
	PROFILE_ME;

	const auto map_cell_map = g_map_cell_map.lock();
	if(!map_cell_map){
		LOG_EMPERY_CLUSTER_WARNING("Map cell map not loaded.");
		DEBUG_THROW(Exception, sslit("Map cell map not loaded"));
	}

	const auto coord = map_cell->get_coord();

	auto it = map_cell_map->find<0>(coord);
	if(it == map_cell_map->end<0>()){
		LOG_EMPERY_CLUSTER_TRACE("Creating new map cell: coord = ", coord);
		it = map_cell_map->insert<0>(it, MapCellElement(map_cell, master));
	} else {
		LOG_EMPERY_CLUSTER_TRACE("Replacing existent map cell: coord = ", coord);
		map_cell_map->replace<0>(it, MapCellElement(map_cell, master));
	}
}

void WorldMap::get_map_cells_by_rectangle(std::vector<boost::shared_ptr<MapCell>> &ret, Rectangle rectangle){
	PROFILE_ME;

	const auto map_cell_map = g_map_cell_map.lock();
	if(!map_cell_map){
		LOG_EMPERY_CLUSTER_WARNING("Map cell map not loaded.");
		return;
	}

	auto x = rectangle.left();
	while(x < rectangle.right()){
		auto it = map_cell_map->lower_bound<0>(Coord(x, rectangle.bottom()));
		for(;;){
			if(it == map_cell_map->end<0>()){
				goto _exit_while;
			}
			if(it->coord.x() != x){
				x = it->coord.x();
				break;
			}
			if(it->coord.y() >= rectangle.top()){
				++x;
				break;
			}
			ret.emplace_back(it->map_cell);
			++it;
		}
	}
_exit_while:
	;
}

boost::shared_ptr<MapObject> WorldMap::get_map_object(MapObjectUuid map_object_uuid){
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		return { };
	}

	const auto it = map_object_map->find<0>(map_object_uuid);
	if(it == map_object_map->end<0>()){
		LOG_EMPERY_CLUSTER_DEBUG("Map object not found: map_object_uuid = ", map_object_uuid);
		return { };
	}
	if(it->master.expired()){
		LOG_EMPERY_CLUSTER_DEBUG("Master expired: map_object_uuid = ", map_object_uuid);
		const auto master = it->master;
		map_object_map->erase<3>(master);
		return { };
	}
	return it->map_object;
}
void WorldMap::replace_map_object_no_synchronize(const boost::shared_ptr<ClusterClient> &master, const boost::shared_ptr<MapObject> &map_object){
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		DEBUG_THROW(Exception, sslit("Map object map not loaded"));
	}

	const auto map_object_uuid = map_object->get_map_object_uuid();

	auto it = map_object_map->find<0>(map_object_uuid);
	if(it == map_object_map->end<0>()){
		LOG_EMPERY_CLUSTER_TRACE("Creating new map object: map_object_uuid = ", map_object_uuid);
		it = map_object_map->insert<0>(it, MapObjectElement(map_object, master));
	} else {
		LOG_EMPERY_CLUSTER_TRACE("Replacing existent map object: map_object_uuid = ", map_object_uuid);
		map_object_map->replace<0>(it, MapObjectElement(map_object, master));
	}
}
void WorldMap::remove_map_object_no_synchronize(const boost::weak_ptr<ClusterClient> & /* master */, MapObjectUuid map_object_uuid) noexcept {
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		return;
	}

	const auto it = map_object_map->find<0>(map_object_uuid);
	if(it == map_object_map->end<0>()){
		LOG_EMPERY_CLUSTER_DEBUG("Map object not found: map_object_uuid = ", map_object_uuid);
		return;
	}
	const auto map_object = it->map_object;
	const auto old_coord  = it->coord;

	LOG_EMPERY_CLUSTER_TRACE("Removing map object: map_object_uuid = ", map_object_uuid, ", old_coord = ", old_coord);
	map_object_map->erase<0>(it);
}
void WorldMap::update_map_object(const boost::shared_ptr<MapObject> &map_object, bool throws_if_not_exists){
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Map object map not loaded"));
		}
		return;
	}
	const auto cluster_map = g_cluster_map.lock();
	if(!cluster_map){
		LOG_EMPERY_CLUSTER_WARNING("Cluster map not loaded.");
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Cluster object map not loaded"));
		}
		return;
	}

	const auto map_object_uuid = map_object->get_map_object_uuid();

	const auto it = map_object_map->find<0>(map_object_uuid);
	if(it == map_object_map->end<0>()){
		LOG_EMPERY_CLUSTER_WARNING("Map object not found: map_object_uuid = ", map_object_uuid);
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Map object not found"));
		}
		return;
	}
	const auto old_coord = it->coord;
	const auto new_coord = map_object->get_coord();

	const auto cit = cluster_map->find<1>(it->master);
	if((cit == cluster_map->end<1>()) || !cit->scope.hit_test(new_coord)){
		LOG_EMPERY_CLUSTER_DEBUG("Map object is out of the scope of its master: map_object_uuid = ", map_object_uuid);
		map_object_map->erase<0>(it);
	} else {
		LOG_EMPERY_CLUSTER_TRACE("Updating map object: map_object_uuid = ", map_object_uuid,
			", old_coord = ", old_coord, ", new_coord = ", new_coord);
		if(it->coord != new_coord){
			map_object_map->set_key<0, 1>(it, new_coord);
		}
		const auto owner_uuid = map_object->get_owner_uuid();
		if(it->owner_uuid != owner_uuid){
			map_object_map->set_key<0, 2>(it, owner_uuid);
		}
	}

	const auto old_cluster = get_cluster(old_coord);
	if(old_cluster){
		try {
			notify_cluster_map_object_updated(map_object, old_cluster);
		} catch(std::exception &e){
			LOG_EMPERY_CLUSTER_WARNING("std::exception thrown: what = ", e.what());
			old_cluster->shutdown(e.what());
		}
	}
}
void WorldMap::remove_map_object(MapObjectUuid map_object_uuid) noexcept {
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		return;
	}

	const auto it = map_object_map->find<0>(map_object_uuid);
	if(it == map_object_map->end<0>()){
		LOG_EMPERY_CLUSTER_DEBUG("Map object not found: map_object_uuid = ", map_object_uuid);
		return;
	}
	const auto map_object = it->map_object;
	const auto old_coord  = it->coord;

	LOG_EMPERY_CLUSTER_TRACE("Removing map object: map_object_uuid = ", map_object_uuid, ", old_coord = ", old_coord);
	map_object_map->erase<0>(it);

	const auto old_cluster = get_cluster(old_coord);
	if(old_cluster){
		try {
			notify_cluster_map_object_removed(map_object, old_cluster);
		} catch(std::exception &e){
			LOG_EMPERY_CLUSTER_WARNING("std::exception thrown: what = ", e.what());
			old_cluster->shutdown(e.what());
		}
	}
}

void WorldMap::get_map_objects_by_rectangle(std::vector<boost::shared_ptr<MapObject>> &ret, Rectangle rectangle){
	PROFILE_ME;

	const auto map_object_map = g_map_object_map.lock();
	if(!map_object_map){
		LOG_EMPERY_CLUSTER_WARNING("Map object map not loaded.");
		return;
	}

	auto x = rectangle.left();
	while(x < rectangle.right()){
		auto it = map_object_map->lower_bound<1>(Coord(x, rectangle.bottom()));
		for(;;){
			if(it == map_object_map->end<1>()){
				goto _exit_while;
			}
			if(it->coord.x() != x){
				x = it->coord.x();
				break;
			}
			if(it->coord.y() >= rectangle.top()){
				++x;
				break;
			}
			ret.emplace_back(it->map_object);
			++it;
		}
	}
_exit_while:
	;
}

boost::shared_ptr<ClusterClient> WorldMap::get_cluster(Coord coord){
	PROFILE_ME;

	return get_cluster_and_scope(coord).first;
}
void WorldMap::get_all_clusters(std::vector<std::pair<Rectangle, boost::shared_ptr<ClusterClient>>> &ret){
	PROFILE_ME;

	const auto cluster_map = g_cluster_map.lock();
	if(!cluster_map){
		LOG_EMPERY_CLUSTER_WARNING("Cluster map not loaded.");
		return;
	}

	ret.reserve(ret.size() + cluster_map->size());
	for(auto it = cluster_map->begin(); it != cluster_map->end(); ++it){
		auto cluster = it->cluster.lock();
		if(!cluster){
			continue;
		}
		ret.emplace_back(it->scope, std::move(cluster));
	}
}
Rectangle WorldMap::get_cluster_scope(const boost::weak_ptr<ClusterClient> &cluster){
	PROFILE_ME;

	const auto cluster_map = g_cluster_map.lock();
	if(!cluster_map){
		LOG_EMPERY_CLUSTER_WARNING("Cluster map not loaded.");
		return Rectangle(0, 0, 0, 0);
	}

	const auto it = cluster_map->find<1>(cluster);
	if(it == cluster_map->end<1>()){
		LOG_EMPERY_CLUSTER_DEBUG("Cluster session not found.");
		return Rectangle(0, 0, 0, 0);
	}
	return it->scope;
}
std::pair<boost::shared_ptr<ClusterClient>, Rectangle> WorldMap::get_cluster_and_scope(Coord coord){
	PROFILE_ME;

	auto ret = std::make_pair(boost::shared_ptr<ClusterClient>(), Rectangle(0, 0, 0, 0));

	const auto cluster_map = g_cluster_map.lock();
	if(!cluster_map){
		LOG_EMPERY_CLUSTER_DEBUG("Cluster map not loaded.");
		return ret;
	}

	auto it = cluster_map->upper_bound<0>(coord);
	for(;;){
		if(it == cluster_map->begin<0>()){
			LOG_EMPERY_CLUSTER_DEBUG("Cluster not found: coord = ", coord);
			break;
		}
		--it;

		if(!it->scope.hit_test(coord)){
			continue;
		}

		auto test = it->cluster.lock();
		if(test){
			LOG_EMPERY_CLUSTER_DEBUG("Cluster found: coord = ", coord, ", scope = ", it->scope);
			ret.first  = std::move(test);
			ret.second = it->scope;
			break;
		}
		it = cluster_map->erase<0>(it);
	}

	return ret;
}
void WorldMap::set_cluster(const boost::shared_ptr<ClusterClient> &cluster, Rectangle scope){
	PROFILE_ME;

	const auto cluster_map = g_cluster_map.lock();
	if(!cluster_map){
		LOG_EMPERY_CLUSTER_WARNING("Cluster map not loaded.");
		DEBUG_THROW(Exception, sslit("Cluster map not loaded"));
	}

	const auto cit = cluster_map->find<1>(cluster);
	if(cit != cluster_map->end<1>()){
		LOG_EMPERY_CLUSTER_WARNING("Cluster already registered: old_scope = ", cit->scope);
		DEBUG_THROW(Exception, sslit("Cluster already registered"));
	}

	LOG_EMPERY_CLUSTER_INFO("Setting up cluster client:  scope = ", scope);
	auto it = cluster_map->find<0>(scope.bottom_left());
	if(it == cluster_map->end<0>()){
		it = cluster_map->insert<0>(it, ClusterElement(scope, cluster));
	} else {
		if(!it->cluster.expired()){
			LOG_EMPERY_CLUSTER_WARNING("Cluster server conflict:  scope = ", scope);
			DEBUG_THROW(Exception, sslit("Cluster server conflict"));
		}
		cluster_map->set_key<0, 1>(it, cluster);
	}
}

}
