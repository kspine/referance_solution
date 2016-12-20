#ifndef EMPERY_CENTER_CAPTAIN_ITEM_HPP_
#define EMPERY_CENTER_CAPTAIN_ITEM_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/fwd.hpp>
#include <poseidon/virtual_shared_from_this.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>
#include "id_types.hpp"


namespace EmperyCenter {

namespace MongoDb {
	class Center_CaptainItem;
	class Center_CaptainItemAttribute;
}

class PlayerSession;
class Account;

class CaptainItem : NONCOPYABLE, public virtual Poseidon::VirtualSharedFromThis {

public:

	static std::pair<boost::shared_ptr<const Poseidon::JobPromise>, boost::shared_ptr<CaptainItem>> async_create(
		CaptainItemUuid captainitem_uuid, AccountUuid account_uuid, std::uint64_t created_time);

private:
	const boost::shared_ptr<MongoDb::Center_CaptainItem> m_obj;

	boost::container::flat_map<CaptainItemAttributeId,
		boost::shared_ptr<MongoDb::Center_CaptainItemAttribute >> m_attributes;

public:
	CaptainItem(boost::shared_ptr<MongoDb::Center_CaptainItem> obj,
		const std::vector<boost::shared_ptr<MongoDb::Center_CaptainItemAttribute>> &attributes);
	~CaptainItem();

public:
	CaptainItemUuid get_captain_item_uuid() const;
	AccountUuid get_account_uuid() const;

	std::uint64_t get_created_time() const;


	// 初始化属性
	void InitAttributes(std::uint64_t baseid);

	const std::string &get_attribute(CaptainItemAttributeId account_attribute_id) const;
	void get_attributes(boost::container::flat_map<CaptainItemAttributeId, std::string> &ret) const;
	void set_attributes(boost::container::flat_map<CaptainItemAttributeId, std::string> modifiers);

	template<typename T, typename DefaultT = T>
	T cast_attribute(CaptainItemAttributeId attribute_id, const DefaultT def = DefaultT()){
		const auto &str = get_attribute(attribute_id);
		if(str.empty()){
			return def;
		}
		return boost::lexical_cast<T>(str);
	}

	void delete_data();

	void set_use_status(std::string str);
};

}

#endif
