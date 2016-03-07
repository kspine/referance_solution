#ifndef EMPERY_CENTER_MAP_UTILITIES_HPP_
#define EMPERY_CENTER_MAP_UTILITIES_HPP_

#include <cstdint>
#include <boost/shared_ptr.hpp>
#include <utility>
#include <string>
#include <vector>
#include "coord.hpp"
#include "id_types.hpp"

namespace EmperyCenter {

extern std::uint64_t get_distance_of_coords(Coord lhs, Coord rhs);
extern void get_surrounding_coords(std::vector<Coord> &ret, Coord origin, std::uint64_t radius);
extern void get_castle_foundation(std::vector<Coord> &ret, Coord origin, bool solid);

extern std::pair<long, std::string> can_deploy_castle_at(Coord coord, MapObjectUuid excluding_map_object_uuid);

}

#endif
