#ifndef EMPERY_CENTER_SINGLETONS_CAPTAIN_ITEM_MAP_HPP_
#define EMPERY_CENTER_SINGLETONS_CAPTAIN_ITEM_MAP_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

namespace MongoDb {
	class Center_CaptainItem;
	class Center_CaptainItemAttribute;
}


class CaptainItem;
class PlayerSession;

struct CaptainItemMap {

	static boost::shared_ptr<CaptainItem> get(CaptainItemUuid captain_item_uuid);
	static boost::shared_ptr<CaptainItem> require(CaptainItemUuid captain_item_uuid);

	static void get_by_account_uuid(std::vector<boost::shared_ptr<CaptainItem> > &ret, AccountUuid account_uuid);

	static void insert(const boost::shared_ptr<CaptainItem> &captain);
	static void deletemember(CaptainItemUuid captain_item_uuid, bool bdeletemap = true);
	static void update(const boost::shared_ptr<CaptainItem> &captain, bool throws_if_not_exists = true);

	// 回收指定道具
	static int reclaim_item(AccountUuid account_uuid, CaptainItemUuid captain_item_uuid);

	// 回收指定品质的道具
	static void reclaim_buy_quality(AccountUuid account_uuid, std::uint64_t quality);

	static void synchronize_with_player(AccountUuid account_uuid, const boost::shared_ptr<PlayerSession> &session);

	// 玩家增加将领装备
	static void account_add_captain_item(AccountUuid account_uuid, std::uint64_t baseid);

private:
	CaptainItemMap() = delete;
};

}

#endif
