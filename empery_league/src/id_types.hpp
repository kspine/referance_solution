#ifndef EMPERY_LEAGUE_ID_TYPES_HPP_
#define EMPERY_LEAGUE_ID_TYPES_HPP_

#include <iosfwd>
#include <iomanip>
#include <cstdint>
#include <poseidon/cxx_ver.hpp>
#include <poseidon/uuid.hpp>

namespace EmperyLeague {
	template<typename UnderlyingT, int>
	class GenericId {
	private:
		UnderlyingT m_id;

	public:
		constexpr GenericId()
			: m_id()
		{
		}
		explicit constexpr GenericId(UnderlyingT id)
			: m_id(id)
		{
		}

	public:
		constexpr UnderlyingT get() const {
			return m_id;
		}
		void set(UnderlyingT id) {
			m_id = id;
		}

	public:
		constexpr bool operator==(const GenericId &rhs) const {
			return get() == rhs.get();
		}
		constexpr bool operator!=(const GenericId &rhs) const {
			return get() != rhs.get();
		}
		constexpr bool operator<(const GenericId &rhs) const {
			return get() < rhs.get();
		}
		constexpr bool operator>(const GenericId &rhs) const {
			return get() > rhs.get();
		}
		constexpr bool operator<=(const GenericId &rhs) const {
			return get() <= rhs.get();
		}
		constexpr bool operator>=(const GenericId &rhs) const {
			return get() >= rhs.get();
		}

		constexpr explicit operator bool() const noexcept {
			return get() != 0;
		}

		GenericId &operator+=(UnderlyingT rhs) {
			m_id += rhs;
			return *this;
		}
		GenericId &operator-=(UnderlyingT rhs) {
			m_id -= rhs;
			return *this;
		}

		GenericId operator+(UnderlyingT rhs) const {
			return GenericId(m_id + rhs);
		}
		GenericId operator-(UnderlyingT rhs) const {
			return GenericId(m_id - rhs);
		}
		UnderlyingT operator-(GenericId rhs) const {
			return m_id - rhs.m_id;
		}

		GenericId &operator++() {
			return *this += 1;
		}
		GenericId &operator--() {
			return *this -= 1;
		}
		GenericId operator++(int) {
			auto ret = *this;
			++*this;
			return ret;
		}
		GenericId operator--(int) {
			auto ret = *this;
			--*this;
			return ret;
		}
	};

	template<typename UnderlyingT, int MAGIC_T>
	GenericId<UnderlyingT, MAGIC_T> operator+(UnderlyingT lhs, GenericId<UnderlyingT, MAGIC_T> rhs) {
		return rhs += lhs;
	}

	template<typename UnderlyingT, int MAGIC_T>
	std::ostream &operator<<(std::ostream &os, const GenericId<UnderlyingT, MAGIC_T> &id) {
		os << id.get();
		return os;
	}
	template<typename UnderlyingT, int MAGIC_T>
	std::istream &operator >> (std::istream &is, GenericId<UnderlyingT, MAGIC_T> &id) {
		UnderlyingT temp;
		if (is >> temp) {
			id.set(temp);
		}
		return is;
	}

	template<int>
	class GenericUuid {
	private:
		Poseidon::Uuid m_uuid;

	public:
		constexpr GenericUuid()
			: m_uuid()
		{
		}
		explicit constexpr GenericUuid(const Poseidon::Uuid &uuid)
			: m_uuid(uuid)
		{
		}
		explicit GenericUuid(const std::string &str)
			: m_uuid(str.empty() ? Poseidon::Uuid() : Poseidon::Uuid(str))
		{
		}

	public:
		constexpr const Poseidon::Uuid &get() const {
			return m_uuid;
		}
		void set(const Poseidon::Uuid &uuid) {
			m_uuid = uuid;
		}

		std::string str() const {
			if (m_uuid == Poseidon::Uuid()) {
				return{};
			}
			return m_uuid.to_string();
		}

	public:
		constexpr bool operator==(GenericUuid rhs) const {
			return get() == rhs.get();
		}
		constexpr bool operator!=(GenericUuid rhs) const {
			return get() != rhs.get();
		}
		constexpr bool operator<(GenericUuid rhs) const {
			return get() < rhs.get();
		}
		constexpr bool operator>(GenericUuid rhs) const {
			return get() > rhs.get();
		}
		constexpr bool operator<=(GenericUuid rhs) const {
			return get() <= rhs.get();
		}
		constexpr bool operator>=(GenericUuid rhs) const {
			return get() >= rhs.get();
		}

		constexpr explicit operator bool() const noexcept {
			return get() != Poseidon::Uuid();
		}
	};

	template<int MAGIC_T>
	std::ostream &operator<<(std::ostream &os, const GenericUuid<MAGIC_T> &uuid) {
		char str[37];
		uuid.get().to_string(reinterpret_cast<char(&)[36]>(str));
		str[36] = 0;
		os << str;
		return os;
	}
	template<int MAGIC_T>
	std::istream &operator >> (std::istream &is, GenericUuid<MAGIC_T> &uuid) {
		char str[37];
		if (is >> std::setw(sizeof(str)) >> str) {
			uuid.from_string(reinterpret_cast<const char(&)[36]>(str));
		}
		return is;
	}

	namespace IdTypes {

		using ChatMessageTypeId = GenericId<std::uint32_t, 110007>;
		using LeagueAttributeId = GenericId<std::uint32_t, 120039>;
		using LeagueMemberAttributeId = GenericId<std::uint32_t, 120040>;

		using AccountUuid = GenericUuid<210001>;
		using LegionUuid = GenericUuid<210011>;
		using LeagueUuid = GenericUuid<210014>;
	}

	using namespace IdTypes;
}

#endif