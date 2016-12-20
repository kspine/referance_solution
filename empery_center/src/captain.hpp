#ifndef EMPERY_CENTER_CAPTAIN_HPP_
#define EMPERY_CENTER_CAPTAIN_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/fwd.hpp>
#include <poseidon/virtual_shared_from_this.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>
#include "id_types.hpp"


namespace EmperyCenter {

namespace MongoDb {
	class Center_Captain;
	class Center_CaptainAttribute;
	class Center_CaptainEquip;
}

class PlayerSession;
class Account;
class CaptainItem;
class MapObject;

class Captain : NONCOPYABLE, public virtual Poseidon::VirtualSharedFromThis {
	enum AffixType{
		AT_SOLIDER,//部队
		AT_BATTALION,//队列
	};

public:
	static std::pair<boost::shared_ptr<const Poseidon::JobPromise>, boost::shared_ptr<Captain>> async_create(
		CaptainUuid captain_uuid, AccountUuid account_uuid, std::uint64_t created_time);

private:
	const boost::shared_ptr<MongoDb::Center_Captain> m_obj;

	boost::container::flat_map<CaptainAttributeId,
		boost::shared_ptr<MongoDb::Center_CaptainAttribute >> m_attributes;

	std::vector <boost::shared_ptr <MongoDb::Center_CaptainEquip >> m_equips;

public:
	Captain(boost::shared_ptr<MongoDb::Center_Captain> obj,
		const std::vector<boost::shared_ptr<MongoDb::Center_CaptainAttribute>> &attributes, const std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> &equips);
	~Captain();

public:
	CaptainUuid get_captain_uuid() const;
	AccountUuid get_owner_uuid() const;

	std::uint64_t get_created_time() const;
	// 删除数据
	void delete_data();

	// 初始化属性
	void InitAttributes(std::uint64_t baseid);

	const std::string &get_attribute(CaptainAttributeId account_attribute_id) const;
	void get_attributes(boost::container::flat_map<CaptainAttributeId, std::string> &ret) const;
	void set_attributes(boost::container::flat_map<CaptainAttributeId, std::string> modifiers);

	template<typename T, typename DefaultT = T>
	T cast_attribute(CaptainAttributeId attribute_id, const DefaultT def = DefaultT()){
		const auto &str = get_attribute(attribute_id);
		if(str.empty()){
			return def;
		}
		return boost::lexical_cast<T>(str);
	}

	void synchronize_with_player(AccountUuid account_uuid,const boost::shared_ptr<PlayerSession> &session) const;
	// 增加经验值
	void Addexp(std::uint64_t value);
	// 穿装备
	void equip(const boost::shared_ptr<CaptainItem> &item, std::uint64_t pos);
	// 脱装备
	void unload_equip(const boost::shared_ptr<CaptainItem> &item);
	// 获得装备
	void get_equips(std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> &ret);
	// 查看某个部位的装备
	boost::shared_ptr<CaptainItem> get_equip_by_pos(std::uint64_t pos);
	// 升星
	void UpgradeStar(std::uint64_t strtvalue);
	// 升品
	void UpgradeQuality(std::uint64_t quality_value);

	// 计算星级、品质对属性的影响
	void recalculate_captain_attributes();
	// 获得套装件数
	std::uint64_t get_suit_part_nums(std::uint64_t suit_id);

	// 计算装备对属性的影响
	void recalculate_equip_attributes(const boost::shared_ptr<CaptainItem> &item, bool bwear);
	void cal_affix_attr(const std::string &affix_attr,const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change);
	void cal_suit_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change);
	void cal_equip_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change);
	void cal_captain_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change);
};

}

#endif
