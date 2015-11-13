#ifndef EMPERY_CENTER_SINGLETONS_ACCOUNT_MAP_HPP_
#define EMPERY_CENTER_SINGLETONS_ACCOUNT_MAP_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

class PlayerSession;

struct AccountMap {
	enum {
		FL_VALID                    = 0x0001,
		FL_ROBOT                    = 0x0002,

		ATTR_CUSTOM_PUBLIC_END      = 100,
		ATTR_CUSTOM_END             = 200,
		ATTR_PUBLIC_END             = 300,
		ATTR_END                    = 400,

		MAX_NICK_LEN                =  255,
		MAX_ATTRIBUTE_LEN           = 4096,

		ATTR_GENDER                 =   1,
		ATTR_AVATAR                 =   2,

		ATTR_TIME_LAST_LOGGED_IN    = 300,
		ATTR_TIME_LAST_LOGGED_OUT   = 301,
		ATTR_TIME_CREATED           = 302,
	};

	struct AccountInfo {
		AccountUuid account_uuid;
		std::string login_name;
		std::string nick;
		boost::uint64_t flags;
		boost::uint64_t created_time;
	};

	static bool has(const AccountUuid &account_uuid);
	static AccountInfo get(const AccountUuid &account_uuid);
	static AccountInfo require(const AccountUuid &account_uuid);
	static void get_by_nick(std::vector<AccountInfo> &ret, const std::string &nick);

	struct LoginInfo {
		PlatformId platform_id;
		std::string login_name;
		AccountUuid account_uuid;
		boost::uint64_t flags;
		std::string login_token;
		boost::uint64_t login_token_expiry_time;
		boost::uint64_t banned_until;
	};

	static LoginInfo get_login_info(const AccountUuid &account_uuid);
	static LoginInfo get_login_info(PlatformId platform_id, const std::string &login_name);

	static std::pair<AccountUuid, bool> create(PlatformId platform_id, std::string login_name,
		std::string nick, boost::uint64_t flags, std::string remote_ip);

	static void set_nick(const AccountUuid &account_uuid, std::string nick);
	static void set_login_token(const AccountUuid &account_uuid, std::string login_token, boost::uint64_t expiry_time);
	static void set_banned_until(const AccountUuid &account_uuid, boost::uint64_t until);
	static void set_flags(const AccountUuid &account_uuid, boost::uint64_t flags);

	static const std::string &get_attribute(const AccountUuid &account_uuid, unsigned slot);
	static void get_attributes(boost::container::flat_map<unsigned, std::string> &ret,
		const AccountUuid &account_uuid, unsigned begin_slot, unsigned end_slot);
	static void touch_attribute(const AccountUuid &account_uuid, unsigned slot);
	static void set_attribute(const AccountUuid &account_uuid, unsigned slot, std::string value);

	template<typename T, typename DefaultT = T>
	static T cast_attribute(const AccountUuid &account_uuid, unsigned slot, const DefaultT def = DefaultT()){
		const auto &str = get_attribute(account_uuid, slot);
		if(str.empty()){
			return T(def);
		}
		return boost::lexical_cast<T>(str);
	}

	static void send_attributes_to_client(const AccountUuid &account_uuid, const boost::shared_ptr<PlayerSession> &session,
		bool wants_nick, bool wants_attributes, bool wants_private_attributes, bool wants_items);
	static void combined_send_attributes_to_client(const AccountUuid &account_uuid, const boost::shared_ptr<PlayerSession> &session);

private:
	AccountMap() = delete;
};

}

#endif
