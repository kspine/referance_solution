#ifndef EMPERY_CENTER_SINGLETONS_WORLD_MAP_HPP_
#define EMPERY_CENTER_SINGLETONS_WORLD_MAP_HPP_

#include "../id_types.hpp"
#include "../coord.hpp"
#include "../rectangle.hpp"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace EmperyCenter {

class MapObject;
class PlayerSession;

struct WorldMap {
	static boost::shared_ptr<MapObject> get_map_object(MapObjectUuid map_object_uuid);
	static void insert_map_object(const boost::shared_ptr<MapObject> &map_object);
	static void update_map_object(const boost::shared_ptr<MapObject> &map_object, bool throws_if_not_exists = true);
	static void remove_map_object(MapObjectUuid map_object_uuid) noexcept;

	static void get_map_objects_by_owner(std::vector<boost::shared_ptr<MapObject>> &ret, AccountUuid owner_uuid);
	static void get_map_objects_by_parent_object(std::vector<boost::shared_ptr<MapObject>> &ret, MapObjectUuid parent_object_uuid);
	static void get_map_objects_by_rectangle(std::vector<boost::shared_ptr<MapObject>> &ret, Rectangle rectangle);

	static void set_player_view(const boost::shared_ptr<PlayerSession> &session, Rectangle view);
	static void synchronize_player_view(const boost::shared_ptr<PlayerSession> &session, Rectangle view) noexcept;

private:
	WorldMap() = delete;
};

}

#endif
