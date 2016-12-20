#ifndef EMPERY_CENTER_SINGLETONS_CAPTAIN_MAP_HPP_
#define EMPERY_CENTER_SINGLETONS_CAPTAIN_MAP_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

class Captain;
class PlayerSession;

struct CaptainMap {

	static boost::shared_ptr<Captain> get(CaptainUuid captain_uuid);
	static boost::shared_ptr<Captain> require(CaptainUuid captain_uuid);

	static void get_by_account_uuid(std::vector<boost::shared_ptr<Captain> > &ret, AccountUuid account_uuid);

	static void insert(const boost::shared_ptr<Captain> &captain);
	static void deletemember(CaptainUuid captain_uuid,AccountUuid account_uuid, bool bdeletemap = true);
	static void update(const boost::shared_ptr<Captain> &captain, bool throws_if_not_exists = true);

	static void synchronize_with_player(AccountUuid account_uuid, const boost::shared_ptr<PlayerSession> &session);

	static boost::shared_ptr<Captain> find_by_relation_map_object_uuid(AccountUuid account_uuid, std::string map_object_uuid);

private:
	CaptainMap() = delete;
};

}

#endif
