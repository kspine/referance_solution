#ifndef EMPERY_CENTER_COORD_HPP_
#define EMPERY_CENTER_COORD_HPP_

#include <boost/cstdint.hpp>
#include <iosfwd>
#include <climits>

namespace EmperyCenter {

class Coord {
private:
	boost::int64_t m_x;
	boost::int64_t m_y;

public:
	constexpr Coord(boost::int64_t x, boost::int64_t y) noexcept
		: m_x(x), m_y(y)
	{
	}

public:
	constexpr boost::int64_t x() const noexcept {
		return m_x;
	}
	constexpr boost::int64_t y() const noexcept {
		return m_y;
	}

	explicit operator bool() const noexcept;

	constexpr bool operator==(const Coord &rhs) const noexcept {
		return (m_x == rhs.m_x) && (m_y == rhs.m_y);
	}
	constexpr bool operator!=(const Coord &rhs) const noexcept {
		return !(*this == rhs);
	}

	constexpr bool operator<(const Coord &rhs) const noexcept {
		return (m_x != rhs.m_x) ? (m_x < rhs.m_x) : (m_y < rhs.m_y);
	}
	constexpr bool operator>(const Coord &rhs) const noexcept {
		return rhs < *this;
	}
	constexpr bool operator<=(const Coord &rhs) const noexcept {
		return !(rhs < *this);
	}
	constexpr bool operator>=(const Coord &rhs) const noexcept {
		return !(*this < rhs);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Coord &rhs){
	return os <<'(' <<rhs.x() <<',' <<rhs.y() <<')';
}

constexpr auto INVALID_COORD = Coord(INT64_MIN, INT64_MIN);

inline Coord::operator bool() const noexcept {
	return *this != INVALID_COORD;
}

}

#endif
