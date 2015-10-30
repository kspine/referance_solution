#include "../precompiled.hpp"
#include "map_object_map.hpp"
#include <boost/container/flat_set.hpp>
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/mysql_daemon.hpp>
#include "../player_session.hpp"
#include "../map_object.hpp"
#include "../map_object_attr_ids.hpp"
#include "../mysql/map_object.hpp"
#include "../mysql/map_object_attribute.hpp"
#include "../msg/sc_map.hpp"

namespace EmperyCenter {

namespace {
	struct MapObjectElement {
		boost::shared_ptr<MapObject> mapObject;

		MapObjectUuid mapObjectUuid;
		Coord coord;

		MapObjectElement(boost::shared_ptr<MapObject> mapObject_, const Coord &coord_)
			: mapObject(std::move(mapObject_))
			, mapObjectUuid(mapObject->getMapObjectUuid()), coord(coord_)
		{
		}
	};

	MULTI_INDEX_MAP(MapObjectMapDelegator, MapObjectElement,
		UNIQUE_MEMBER_INDEX(mapObjectUuid)
		MULTI_MEMBER_INDEX(coord)
	)

	boost::weak_ptr<MapObjectMapDelegator> g_mapObjectMap;

	inline Coord sectorCoordFromMapCoord(const Coord &coord){
		return Coord(coord.x() >> 5, coord.y() >> 5);
	}

	struct MapSectorElement {
		Coord sectorCoord;

		mutable boost::container::flat_set<boost::weak_ptr<MapObject>> mapObjects;

		explicit MapSectorElement(const Coord &sectorCoord_)
			: sectorCoord(sectorCoord_)
		{
		}
	};

	MULTI_INDEX_MAP(MapSectorMapDelegator, MapSectorElement,
		UNIQUE_MEMBER_INDEX(sectorCoord)
	)

	boost::weak_ptr<MapSectorMapDelegator> g_mapSectorMap;

	struct PlayerViewElement {
		boost::weak_ptr<PlayerSession> session;
		Coord sectorCoord;

		PlayerViewElement(boost::weak_ptr<PlayerSession> session_, const Coord &sectorCoord_)
			: session(std::move(session_)), sectorCoord(sectorCoord_)
		{
		}
	};

	MULTI_INDEX_MAP(PlayerViewMapDelegator, PlayerViewElement,
		MULTI_MEMBER_INDEX(session)
		MULTI_MEMBER_INDEX(sectorCoord)
	)

	boost::weak_ptr<PlayerViewMapDelegator> g_playerViewMap;

	MODULE_RAII_PRIORITY(handles, 5300){
		const auto conn = Poseidon::MySqlDaemon::createConnection();

		std::map<Poseidon::Uuid, std::pair<
			boost::shared_ptr<MySql::Center_MapObject>,
			std::vector<boost::shared_ptr<MySql::Center_MapObjectAttribute>>
			>> tempMap;

		LOG_EMPERY_CENTER_INFO("Loading map objects...");
		conn->executeSql("SELECT * FROM `Center_MapObject`");
		while(conn->fetchRow()){
			auto obj = boost::make_shared<MySql::Center_MapObject>();
			obj->syncFetch(conn);
			obj->enableAutoSaving();
			const auto mapObjectUuid = obj->unlockedGet_mapObjectUuid();
			if(obj->get_deleted()){
				LOG_EMPERY_CENTER_WARNING(
					"The following map object has been marked as deleted, consider deleting it from database: mapObjectUuid = ", mapObjectUuid);
				continue;
			}
			auto &pair = tempMap[mapObjectUuid];
			pair.first = std::move(obj);
		}
		LOG_EMPERY_CENTER_INFO("Loaded ", tempMap.size(), " map object(s).");

		LOG_EMPERY_CENTER_INFO("Loading map object attributes...");
		conn->executeSql("SELECT * FROM `Center_MapObjectAttribute`");
		while(conn->fetchRow()){
			auto obj = boost::make_shared<MySql::Center_MapObjectAttribute>();
			obj->syncFetch(conn);
			obj->enableAutoSaving();
			const auto mapObjectUuid = obj->unlockedGet_mapObjectUuid();
			const auto it = tempMap.find(mapObjectUuid);
			if(it == tempMap.end()){
				LOG_EMPERY_CENTER_WARNING(
					"A dangling map object attribute has been found, consider deleting it from database: mapObjectUuid = ", mapObjectUuid);
				continue;
			}
			it->second.second.emplace_back(std::move(obj));
		}
		LOG_EMPERY_CENTER_INFO("Loaded ", tempMap.size(), " map object attribute(s).");

		const auto mapObjectMap = boost::make_shared<MapObjectMapDelegator>();
		for(auto it = tempMap.begin(); it != tempMap.end(); ++it){
			auto mapObject = boost::make_shared<MapObject>(std::move(it->second.first), it->second.second);
			auto coord = Coord(mapObject->getAttribute(MapObjectAttrIds::ID_COORD_X),
				mapObject->getAttribute(MapObjectAttrIds::ID_COORD_Y));
			mapObjectMap->insert(MapObjectElement(std::move(mapObject), coord));
		}
		g_mapObjectMap = mapObjectMap;
		handles.push(mapObjectMap);

		const auto mapSectorMap = boost::make_shared<MapSectorMapDelegator>();
		g_mapSectorMap = mapSectorMap;
		handles.push(mapSectorMap);

		const auto playerViewMap = boost::make_shared<PlayerViewMapDelegator>();
		g_playerViewMap = playerViewMap;
		handles.push(playerViewMap);
	}

	void synchronizeMapObject(const boost::shared_ptr<MapObject> &mapObject, const boost::shared_ptr<PlayerSession> &session) noexcept {
		PROFILE_ME;

		try {
			if(mapObject->hasBeenDeleted()){
				Msg::SC_MapObjectRemoved msg;
				msg.objectUuid = mapObject->getMapObjectUuid().str();
				session->send(msg);
			} else {
				boost::container::flat_map<MapObjectAttrId, boost::int64_t> attributes;
				mapObject->getAttributes(attributes);

				Msg::SC_MapObjectInfo msg;
				msg.objectUuid   = mapObject->getMapObjectUuid().str();
				msg.objectTypeId = mapObject->getMapObjectTypeId().get();
				msg.ownerUuid    = mapObject->getOwnerUuid().str();
				msg.attributes.reserve(attributes.size());
				for(auto it = attributes.begin(); it != attributes.end(); ++it){
					msg.attributes.emplace_back();
					auto &attribute = msg.attributes.back();
					attribute.slot  = it->first.get();
					attribute.value = it->second;
				}
				session->send(msg);
			}
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			session->forceShutdown();
		}
	}
	void synchronizeMapObject(const boost::shared_ptr<MapObject> &mapObject, const Coord &coord) noexcept {
		PROFILE_ME;

		const auto playerViewMap = g_playerViewMap.lock();
		if(!playerViewMap){
			LOG_EMPERY_CENTER_WARNING("Player view map is not initialized.");
			return;
		}

		const auto sectorCoord = sectorCoordFromMapCoord(coord);
		const auto range = playerViewMap->equalRange<1>(sectorCoord);
		if(range.first == range.second){
			LOG_EMPERY_CENTER_DEBUG("No player in sector: sectorCoord = ", sectorCoord);
			return;
		}
		auto it = range.first;
		do {
			const auto session = it->session.lock();
			if(!session){
				it = playerViewMap->erase<1>(it);
				continue;
			}
			synchronizeMapObject(mapObject, session);
			++it;
		} while(it != range.second);
	}
}

boost::shared_ptr<MapObject> MapObjectMap::get(const MapObjectUuid &mapObjectUuid){
	PROFILE_ME;

	const auto mapObjectMap = g_mapObjectMap.lock();
	if(!mapObjectMap){
		LOG_EMPERY_CENTER_WARNING("Map object map is not loaded.");
		return { };
	}

	const auto it = mapObjectMap->find<0>(mapObjectUuid);
	if(it == mapObjectMap->end<0>()){
		LOG_EMPERY_CENTER_DEBUG("Map object is not found: mapObjectUuid = ", mapObjectUuid);
		return { };
	}
	return it->mapObject;
}
void MapObjectMap::update(const boost::shared_ptr<MapObject> &mapObject, const Coord &coord){
	PROFILE_ME;

	const auto mapObjectMap = g_mapObjectMap.lock();
	if(!mapObjectMap){
		LOG_EMPERY_CENTER_WARNING("Map object map is not loaded.");
		DEBUG_THROW(Exception, sslit("Map object map is not loaded"));
	}
	const auto mapSectorMap = g_mapSectorMap.lock();
	if(!mapSectorMap){
		LOG_EMPERY_CENTER_WARNING("Map sector map is not loaded.");
		DEBUG_THROW(Exception, sslit("Map sector map is not loaded"));
	}

	const auto mapObjectUuid = mapObject->getMapObjectUuid();

	if(mapObject->hasBeenDeleted()){
		LOG_EMPERY_CENTER_DEBUG("Map object has been marked as deleted: mapObjectUuid = ", mapObjectUuid);
		remove(mapObjectUuid);
		return;
	}

	const auto sectorCoord = sectorCoordFromMapCoord(coord);
	auto sectorIt = mapSectorMap->find<0>(sectorCoord);
	if(sectorIt == mapSectorMap->end<0>()){
		sectorIt = mapSectorMap->insert<0>(sectorIt, MapSectorElement(sectorCoord));
	}
	sectorIt->mapObjects.reserve(sectorIt->mapObjects.size() + 1);

	auto oldSectorIt = mapSectorMap->end<0>();
	auto it = mapObjectMap->find<0>(mapObjectUuid);
	if(it == mapObjectMap->end<0>()){
		it = mapObjectMap->insert<0>(it, MapObjectElement(mapObject, coord));
	} else {
		oldSectorIt = mapSectorMap->find<0>(sectorCoordFromMapCoord(it->coord));
		mapObjectMap->setKey<0, 1>(it, coord);
	}
	it->mapObject->setAttribute(MapObjectAttrIds::ID_COORD_X, coord.x());
	it->mapObject->setAttribute(MapObjectAttrIds::ID_COORD_Y, coord.y());

	if(oldSectorIt != sectorIt){
		if(oldSectorIt != mapSectorMap->end<0>()){
			LOG_EMPERY_CENTER_DEBUG("> Removing map object from sector: mapObjectUuid = ", mapObjectUuid,
				", sectorCoord = ", oldSectorIt->sectorCoord);
			oldSectorIt->mapObjects.erase(mapObject);
		}
		LOG_EMPERY_CENTER_DEBUG("> Adding map object into sector: mapObjectUuid = ", mapObjectUuid,
			", sectorCoord = ", sectorIt->sectorCoord);
		sectorIt->mapObjects.insert(mapObject);
	}

	synchronizeMapObject(mapObject, coord);
}
void MapObjectMap::remove(const MapObjectUuid &mapObjectUuid) noexcept {
	PROFILE_ME;

	const auto mapObjectMap = g_mapObjectMap.lock();
	if(!mapObjectMap){
		LOG_EMPERY_CENTER_WARNING("Map object map is not loaded.");
		return;
	}
	const auto mapSectorMap = g_mapSectorMap.lock();
	if(!mapSectorMap){
		LOG_EMPERY_CENTER_FATAL("Map sector map is not loaded.");
		std::abort();
	}

	const auto it = mapObjectMap->find<0>(mapObjectUuid);
	if(it == mapObjectMap->end<0>()){
		LOG_EMPERY_CENTER_DEBUG("Map object is not found: mapObjectUuid = ", mapObjectUuid);
		return;
	}
	const auto mapObject = it->mapObject;
	const auto coord = it->coord;
	mapObjectMap->erase<0>(it);

	try {
		mapObject->deleteFromGame();
	} catch(std::exception &e){
		LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
	}

	const auto oldSectorIt = mapSectorMap->find<0>(sectorCoordFromMapCoord(coord));
	if(oldSectorIt != mapSectorMap->end<0>()){
		LOG_EMPERY_CENTER_DEBUG("> Removing map object from sector: mapObjectUuid = ", mapObjectUuid,
			", sectorCoord = ", oldSectorIt->sectorCoord);
		oldSectorIt->mapObjects.erase(mapObject);
	}

	synchronizeMapObject(mapObject, coord);
}

void MapObjectMap::getByRectangle(std::vector<std::pair<Coord, boost::shared_ptr<MapObject>>> &ret, const Rectangle &rectangle){
	PROFILE_ME;

	const auto mapObjectMap = g_mapObjectMap.lock();
	if(!mapObjectMap){
		LOG_EMPERY_CENTER_WARNING("Map object map is not loaded.");
		return;
	}

	auto x = rectangle.left();
	while(x < rectangle.right()){
		auto it = mapObjectMap->lowerBound<1>(Coord(x, rectangle.bottom()));
		for(;;){
			if(it == mapObjectMap->end<1>()){
				x = INT64_MAX;
				break;
			}
			if(it->coord.x() != x){
				x = it->coord.x();
				break;
			}
			if(it->coord.y() >= rectangle.top()){
				++x;
				break;
			}
			ret.emplace_back(it->coord, it->mapObject);
			++it;
		}
	}
}

void MapObjectMap::setPlayerView(const boost::shared_ptr<PlayerSession> &session, const Rectangle &view){
	PROFILE_ME;

	const auto mapSectorMap = g_mapSectorMap.lock();
	if(!mapSectorMap){
		LOG_EMPERY_CENTER_WARNING("Map sector map is not initialized.");
		DEBUG_THROW(Exception, sslit("Map sector map is not initialized"));
	}
	const auto playerViewMap = g_playerViewMap.lock();
	if(!playerViewMap){
		LOG_EMPERY_CENTER_WARNING("Player view map is not initialized.");
		DEBUG_THROW(Exception, sslit("Player view map is not initialized"));
	}

	playerViewMap->erase<0>(session);

	if((view.left() == view.right()) || (view.bottom() == view.top())){
		return;
	}

	const auto sectorBottomLeft = sectorCoordFromMapCoord(Coord(view.left(), view.bottom()));
	const auto sectorUpperRight = sectorCoordFromMapCoord(Coord(view.right() - 1, view.top() - 1));
	LOG_EMPERY_CENTER_DEBUG("Set player view: view = ", view,
		", sectorBottomLeft = ", sectorBottomLeft, ", sectorUpperRight = ", sectorUpperRight);
	try {
		for(boost::int64_t x = sectorBottomLeft.x(); x <= sectorBottomLeft.x(); ++x){
			for(boost::int64_t y = sectorUpperRight.y(); y <= sectorUpperRight.y(); ++y){
				playerViewMap->insert(PlayerViewElement(session, Coord(x, y)));
			}
		}
	} catch(std::exception &e){
		LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
		playerViewMap->erase<0>(session);
		throw;
	}

	synchronizePlayerView(session, view);
}

void MapObjectMap::synchronizePlayerView(const boost::shared_ptr<PlayerSession> &session, const Rectangle &view) noexcept {
	PROFILE_ME;

	const auto mapSectorMap = g_mapSectorMap.lock();
	if(!mapSectorMap){
		LOG_EMPERY_CENTER_DEBUG("Map sector map is not initialized.");
		return;
	}
	const auto playerViewMap = g_playerViewMap.lock();
	if(!playerViewMap){
		LOG_EMPERY_CENTER_DEBUG("Player view map is not initialized.");
		return;
	}

	const auto sectorBottomLeft = sectorCoordFromMapCoord(Coord(view.left(), view.bottom()));
	const auto sectorUpperRight = sectorCoordFromMapCoord(Coord(view.right() - 1, view.top() - 1));
	LOG_EMPERY_CENTER_DEBUG("Synchronize player view: view = ", view,
		", sectorBottomLeft = ", sectorBottomLeft, ", sectorUpperRight = ", sectorUpperRight);
	try {
		for(boost::int64_t x = sectorBottomLeft.x(); x <= sectorBottomLeft.x(); ++x){
			for(boost::int64_t y = sectorUpperRight.y(); y <= sectorUpperRight.y(); ++y){
				const auto sectorIt = mapSectorMap->find<0>(Coord(x, y));
				if(sectorIt == mapSectorMap->end<0>()){
					continue;
				}

				auto it = sectorIt->mapObjects.begin();
				while(it != sectorIt->mapObjects.end()){
					const auto mapObject = it->lock();
					if(!mapObject){
						it = sectorIt->mapObjects.erase(it);
						continue;
					}
					synchronizeMapObject(mapObject, session);
					++it;
				}
			}
		}
	} catch(std::exception &e){
		LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
		session->forceShutdown();
	}
}

}
