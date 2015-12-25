#ifndef EMPERY_CENTER_UTILITIES_NAMESPACE_
#	error Do not use this file directly!
#endif

#include <boost/cstdint.hpp>
#include <algorithm>
#include <vector>
#include "coord.hpp"
#include "log.hpp"

namespace EMPERY_CENTER_UTILITIES_NAMESPACE_ {

extern std::size_t hash_string_nocase(const std::string &str) noexcept;
extern bool are_strings_equal_nocase(const std::string &lhs, const std::string &rhs) noexcept;

extern boost::uint64_t get_distance_of_coords(Coord lhs, Coord rhs);
extern void get_surrounding_coords(std::vector<Coord> &ret, Coord origin, boost::uint64_t radius);
extern void get_castle_foundation(std::vector<Coord> &ret, Coord origin, bool solid);

}

#undef EMPERY_CENTER_UTILITIES_NAMESPACE_
