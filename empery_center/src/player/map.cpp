#include "../precompiled.hpp"
#include "common.hpp"
#include "../msg/cs_map.hpp"
#include "../msg/sc_map.hpp"
#include "../msg/cerr_map.hpp"
#include "../singletons/cluster_session_map.hpp"
#include "../singletons/map_object_map.hpp"
#include "../map_object.hpp"

namespace EmperyCenter {

PLAYER_SERVLET(Msg::CS_MapQueryWorldMap, account_uuid, session, req){
	(void)req;

	ClusterSessionMap::SessionContainer clusters;
	ClusterSessionMap::get_all(clusters);
	const auto map_size = ClusterSessionMap::require_map_size();

	Msg::SC_MapWorldMapList msg;
	msg.maps.reserve(clusters.size());
	for(auto it = clusters.begin(); it != clusters.end(); ++it){
		msg.maps.emplace_back();
		auto &map = msg.maps.back();
		map.map_x = it->first.first;
		map.map_x = it->first.second;
	}
	msg.map_width = map_size.first;
	msg.map_height = map_size.second;
	session->send(msg);

	return Response();
}

PLAYER_SERVLET(Msg::CS_MapSetView, account_uuid, session, req){
	MapObjectMap::set_player_view(session, Rectangle(req.x, req.y, req.width, req.height));

	return Response();
}

PLAYER_SERVLET(Msg::CS_MapRefreshView, account_uuid, session, req){
	MapObjectMap::synchronize_player_view(session, Rectangle(req.x, req.y, req.width, req.height));

	return Response();
}

PLAYER_SERVLET(Msg::CS_MapSetObjectPath, account_uuid, session, req){
	const auto map_object_uuid = MapObjectUuid(req.map_object_uuid);
	const auto map_object = MapObjectMap::get(map_object_uuid);
	if(!map_object){
		return { Msg::CERR_NO_SUCH_OBJECT, map_object_uuid.str() };
	}
	if(map_object->get_owner_uuid() != account_uuid){
		return { Msg::CERR_NOT_YOUR_OBJECT, map_object->get_owner_uuid().str() };
	}
	const auto from_coord = map_object->get_coord();
	if((from_coord.x() != req.x) || (from_coord.y() != req.y)){
		return { Msg::CERR_OBJECT_COORD_MISMATCH, boost::lexical_cast<std::string>(from_coord) };
	}
	// TODO 判断能不能走。

	return Response();
}

}
