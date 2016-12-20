#include "precompiled.hpp"
#include "account.hpp"
#include "mmain.hpp"
#include "mongodb/captain.hpp"
#include "captain.hpp"
#include "singletons/captain_map.hpp"
#include "singletons/captain_item_map.hpp"
#include "data/captain_config.hpp"
#include "data/captain_level_config.hpp"
#include "captain_attribute_ids.hpp"
#include "captain_item.hpp"
#include "data/captain_item_config.hpp"
#include "captain_item_attribute_ids.hpp"
#include "data/captain_suit_config.hpp"
#include "data/captain_equippro_config.hpp"
#include "data/captain_equipaffix_config.hpp"
#include "singletons/world_map.hpp"
#include "map_object.hpp"
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>


namespace EmperyCenter {

	std::pair<boost::shared_ptr<const Poseidon::JobPromise>, boost::shared_ptr<Captain>> Captain::async_create(
		CaptainUuid captain_uuid,  AccountUuid account_uuid, std::uint64_t created_time)
{
	PROFILE_ME;

	auto obj = boost::make_shared<MongoDb::Center_Captain>(captain_uuid.get(),  account_uuid.get(), created_time);
	obj->enable_auto_saving();
	auto promise = Poseidon::MongoDbDaemon::enqueue_for_saving(obj, false, true);
	auto captain = boost::make_shared<Captain>(std::move(obj), std::vector<boost::shared_ptr<MongoDb::Center_CaptainAttribute>>(), std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>>());
	return std::make_pair(std::move(promise), std::move(captain));
}

Captain::Captain(boost::shared_ptr<MongoDb::Center_Captain> obj,
	const std::vector<boost::shared_ptr<MongoDb::Center_CaptainAttribute>> &attributes, const std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> &equips)
	: m_obj(std::move(obj))
{
	for(auto it = attributes.begin(); it != attributes.end(); ++it){
		m_attributes.emplace(CaptainAttributeId((*it)->get_attribute_id()), *it);
	}

	for (auto eit = equips.begin(); eit != equips.end(); ++eit){
		m_equips.push_back(*eit);
	}
}
Captain::~Captain(){
}

CaptainUuid Captain::get_captain_uuid() const {
	return CaptainUuid(m_obj->unlocked_get_captain_uuid());
}

AccountUuid Captain::get_owner_uuid() const{
	return AccountUuid(m_obj->unlocked_get_own_uuid());
}

void Captain::InitAttributes(std::uint64_t baseid)
{
	PROFILE_ME;

	const auto& config = Data::CaptainConfig::get(baseid);
	if (config)
	{
		// 设置属性
		boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

		modifiers.emplace(CaptainAttributeIds::ID_BASEID, std::move(boost::lexical_cast<std::string>(baseid)));
		modifiers.emplace(CaptainAttributeIds::ID_LEVEL, "1");
		modifiers.emplace(CaptainAttributeIds::ID_STARLEVEL, "1");
		modifiers.emplace(CaptainAttributeIds::ID_CUREXP, "0");
		modifiers.emplace(CaptainAttributeIds::ID_QUALITYLEVEL, std::move(boost::lexical_cast<std::string>(config->begin_quality)));

		modifiers.emplace(CaptainAttributeIds::ID_BASEATTRIBUTE, config->basic_properties_array.dump());
		modifiers.emplace(CaptainAttributeIds::ID_SPECIALATTRIBUTE, "");
		modifiers.emplace(CaptainAttributeIds::ID_CURSTATUS, "0");
		modifiers.emplace(CaptainAttributeIds::ID_RELATION_OBJECT_UUID, "");

		modifiers.emplace(CaptainAttributeIds::ID_EQUIP_ATTRIBUTE, "{}");

		set_attributes(std::move(modifiers));

		// 计算下属性
		recalculate_captain_attributes();
	}
	else
	{
		LOG_EMPERY_CENTER_WARNING("Captain can not find: baseid = ", baseid);
		DEBUG_THROW(Exception, sslit("Captain can not  exists"));
	}

}


std::uint64_t Captain::get_created_time() const {
	return m_obj->get_add_time();
}

const std::string &Captain::get_attribute(CaptainAttributeId attribute_id) const {
	PROFILE_ME;

	const auto it = m_attributes.find(attribute_id);
	if(it == m_attributes.end()){
		return Poseidon::EMPTY_STRING;
	}
	return it->second->unlocked_get_value();
}
void Captain::get_attributes(boost::container::flat_map<CaptainAttributeId, std::string> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_attributes.size());
	for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it){
		ret[it->first] = it->second->unlocked_get_value();
	}
}
void Captain::set_attributes(boost::container::flat_map<CaptainAttributeId, std::string> modifiers){
	PROFILE_ME;


	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		const auto obj_it = m_attributes.find(it->first);
		if(obj_it == m_attributes.end()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainAttribute>(m_obj->get_captain_uuid(),
				it->first.get(), std::string());
			obj->async_save(true);
			m_attributes.emplace(it->first, std::move(obj));
		}
	}

	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		const auto &obj = m_attributes.at(it->first);
		obj->set_value(std::move(it->second));
	}

	CaptainMap::update(virtual_shared_from_this<Captain>(), false);
}

void Captain::synchronize_with_player(AccountUuid account_uuid, const boost::shared_ptr<PlayerSession> &session) const {
	PROFILE_ME;

}

void Captain::delete_data()
{
	PROFILE_ME;

	// 删除属性表
	Poseidon::MongoDb::BsonBuilder query;
	query.append_regex(sslit("_id"),PRIMERY_KEYGEN::GenIDS::GenId(get_captain_uuid().get()));
		
    LOG_EMPERY_CENTER_ERROR("query _id:captain_uuid=",get_captain_uuid().get());

	Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_CaptainAttribute", query, true);
}

void Captain::Addexp(std::uint64_t value)
{
	PROFILE_ME;

	const auto curexp = boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_CUREXP));
	const auto curlevel = boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_LEVEL));

	bool bup = false;
	// 设置属性
	boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

	auto nextlevelinfo = Data::CaptainLevelConfig::get(curlevel + 1);
	const auto total_exp = curexp + value;
	
	auto expect_level = curlevel;
	auto temp_exp     = total_exp;
	while(nextlevelinfo && temp_exp > nextlevelinfo->level_resource){
		expect_level += 1;
		temp_exp -= nextlevelinfo->level_resource;
		nextlevelinfo = Data::CaptainLevelConfig::get(expect_level + 1);
		bup = true;
	}
	if(bup){
		modifiers[CaptainAttributeIds::ID_CUREXP] = boost::lexical_cast<std::string>(temp_exp);
		modifiers[CaptainAttributeIds::ID_LEVEL] = boost::lexical_cast<std::string>(expect_level);
		set_attributes(std::move(modifiers));
		recalculate_captain_attributes();
	}
}

void Captain::equip(const boost::shared_ptr<CaptainItem> &item, std::uint64_t pos)
{
	PROFILE_ME;

	// 设置属性
	item->set_use_status(boost::lexical_cast<std::string>(pos));

	auto obj = boost::make_shared<MongoDb::Center_CaptainEquip>(get_captain_uuid().get(), item->get_captain_item_uuid().get());
	obj->enable_auto_saving();
	Poseidon::MongoDbDaemon::enqueue_for_saving(obj, false, true);

	m_equips.push_back(obj);

	// 计算属性
	recalculate_equip_attributes(item,true);

}

void Captain::unload_equip(const boost::shared_ptr<CaptainItem> &item)
{
	PROFILE_ME;

	// 设置属性
	item->set_use_status("0");

	for (auto eit = m_equips.begin(); eit != m_equips.end(); ++eit){
		if (CaptainItemUuid((*eit)->unlocked_get_captain_item_uuid()) == item->get_captain_item_uuid())
		{
			eit = m_equips.erase(eit);
			break;
		}
	}


	// 从装备表中删除
	Poseidon::MongoDb::BsonBuilder query;
	query.append_string(sslit("_id"),PRIMERY_KEYGEN::GenIDS::GenId(get_captain_uuid().get(),item->get_captain_item_uuid().get()));
	Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_CaptainEquip", query, true);

	// 计算属性
	recalculate_equip_attributes(item, false);

}

void Captain::get_equips(std::vector<boost::shared_ptr<MongoDb::Center_CaptainEquip>> &ret)
{
	PROFILE_ME;

	ret.reserve(m_equips.size());
	for (auto eit = m_equips.begin(); eit != m_equips.end(); ++eit){
		ret.push_back(*eit);
	}

}

boost::shared_ptr<CaptainItem> Captain::get_equip_by_pos(std::uint64_t pos)
{
	PROFILE_ME;

	for (auto eit = m_equips.begin(); eit != m_equips.end(); ++eit){
		auto equip = *eit;
		const auto& equip_info = CaptainItemMap::get(CaptainItemUuid(equip->unlocked_get_captain_item_uuid()));
		if (equip_info)
		{
			const auto& item = Data::CaptainItemConfig::get(boost::lexical_cast<std::uint64_t>(equip_info->get_attribute(CaptainItemAttributeIds::ID_BASEID)));
			if (item && item->position == pos)
			{
				return equip_info;
			}
		}
	}

	return{};
}

void Captain::UpgradeStar(std::uint64_t strtvalue)
{
	PROFILE_ME;

	// 设置属性
	boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

	modifiers[CaptainAttributeIds::ID_STARLEVEL] = boost::lexical_cast<std::string>(strtvalue);

	set_attributes(std::move(modifiers));

	// TODO 查看对属性的影响
	recalculate_captain_attributes();
}

void Captain::UpgradeQuality(std::uint64_t quality_value)
{
	PROFILE_ME;

	// 设置属性
	boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

	modifiers[CaptainAttributeIds::ID_QUALITYLEVEL] = boost::lexical_cast<std::string>(quality_value);

	set_attributes(std::move(modifiers));

	// TODO 查看对属性的影响
	recalculate_captain_attributes();
}

void Captain::recalculate_captain_attributes()
{
	PROFILE_ME;

	boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

	// 查看将领对属性的影响
	const auto& config = Data::CaptainConfig::get(boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_BASEID)));
	if (config)
	{
		// 当前队列士兵数量

		Poseidon::JsonObject  obj;

		obj = std::move(config->basic_properties_array);

		// 取得当前等级
		const auto curlevel = boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_LEVEL));

		// 取得星级影响系数
		double star_add_max_count = 0;
		double star_add_attack = 0;
		double star_add_defense = 0;
		const auto cur_star_level = boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_STARLEVEL));
		LOG_EMPERY_CENTER_INFO("recalculate_captain_attributes  将领基础属性解析 ===========", obj.dump(), ",curlevel=", curlevel, ",cur_star_level=", cur_star_level);
		auto sit = config->star.find(cur_star_level);
		if (sit != config->star.end())
		{

			auto ait = sit->second.find("6200003");
			if (ait != sit->second.end())
			{
				star_add_max_count = curlevel * ait->second;
			}
			
			ait = sit->second.find("6200001");
			if (ait != sit->second.end())
				star_add_attack = curlevel * ait->second;

			ait = sit->second.find("6200002");
			if (ait != sit->second.end())
				star_add_defense = curlevel * ait->second;
		}

		// 取得品质影响系数
		double quality_add_max_count = 0;
		double quality_add_attack = 0;
		double quality_add_defense = 0;
		const auto cur_quality_level = boost::lexical_cast<std::uint64_t>(get_attribute(CaptainAttributeIds::ID_QUALITYLEVEL));
		auto qit = config->quality.find(cur_quality_level);
		if (qit != config->quality.end())
		{
			auto ait = qit->second.find("6200003");
			if (ait != qit->second.end())
				quality_add_max_count = curlevel * ait->second;

			ait = qit->second.find("6200001");
			if (ait != qit->second.end())
				quality_add_attack = curlevel * ait->second;

			ait = qit->second.find("6200002");
			if (ait != qit->second.end())
				quality_add_defense = curlevel * ait->second;
		}

		auto  add_max_count = 0;
		if (config->basic_properties_array.find(SharedNts::view("6200003")) != config->basic_properties_array.end())
		{
			add_max_count = static_cast<std::uint64_t>(config->basic_properties_array.at(SharedNts::view("6200003")).get<double>());

			obj[SharedNts("6200003")] = static_cast<std::uint64_t>(std::ceil(add_max_count + star_add_max_count + quality_add_max_count));

			LOG_EMPERY_CENTER_INFO("recalculate_captain_attributes  将领基础属性 计算统帅力 add_max_count===========", add_max_count, ",star_add_max_count=", star_add_max_count, ",quality_add_max_count=", quality_add_max_count);
		}
		auto  add_attack = 0;
		if (config->basic_properties_array.find(SharedNts::view("6200001")) != config->basic_properties_array.end())
		{
			add_attack = static_cast<std::uint64_t>(config->basic_properties_array.at(SharedNts::view("6200001")).get<double>());

			obj[SharedNts("6200001")] = static_cast<std::uint64_t>(std::ceil(add_attack + star_add_attack + quality_add_attack));

			LOG_EMPERY_CENTER_INFO("recalculate_captain_attributes  将领基础属性 计算攻击力 add_attack===========", add_attack, ",star_add_attack=", star_add_attack, ",quality_add_attack=", quality_add_attack);
		}
		auto  add_defense = 0;
		if (config->basic_properties_array.find(SharedNts::view("6200002")) != config->basic_properties_array.end())
		{
			add_defense = static_cast<std::uint64_t>(config->basic_properties_array.at(SharedNts::view("6200002")).get<double>());

			obj[SharedNts("6200002")] = static_cast<std::uint64_t>(std::ceil(add_defense + star_add_defense + quality_add_defense));

			LOG_EMPERY_CENTER_INFO("recalculate_captain_attributes  将领基础属性 计算防御力 add_defense===========", add_defense, ",star_add_defense=", star_add_defense, ",quality_add_defense=", quality_add_defense);
		}

		LOG_EMPERY_CENTER_INFO("recalculate_captain_attributes  将领基础属性 计算后 ===========", obj.dump());

		modifiers[CaptainAttributeIds::ID_BASEATTRIBUTE] = obj.dump();

	}

	set_attributes(std::move(modifiers));
}

std::uint64_t Captain::get_suit_part_nums(std::uint64_t suit_id)
{
	PROFILE_ME;

	std::uint64_t ncount = 0;
	for (auto it = m_equips.begin(); it != m_equips.end(); ++it)
	{

		const auto &captain_item_info = CaptainItemMap::get(CaptainItemUuid((*it)->unlocked_get_captain_item_uuid()));
		if (captain_item_info)
		{
			const auto& item = Data::CaptainItemConfig::get(boost::lexical_cast<std::uint64_t>(captain_item_info->get_attribute(CaptainItemAttributeIds::ID_BASEID)));
			if (item && item->set_id == suit_id)
			{
				ncount += 1;
			}
		}
	}

	return ncount;
}

void Captain::recalculate_equip_attributes(const boost::shared_ptr<CaptainItem> &item, bool bwear)
{
	PROFILE_ME;

	// TODO 查看对属性的影响  检查套装逻辑
	const auto equip_attr = get_attribute(CaptainAttributeIds::ID_EQUIP_ATTRIBUTE);
	LOG_EMPERY_CENTER_INFO("Captain::equip  穿装备前属性 字符串 ===========", equip_attr);
	Poseidon::JsonObject  equip_attr_obj;
	if (!equip_attr.empty() || equip_attr != Poseidon::EMPTY_STRING)
	{
		std::istringstream iss(equip_attr);
		auto val = Poseidon::JsonParser::parse_object(iss);
		equip_attr_obj = std::move(val);
	}

	LOG_EMPERY_CENTER_INFO("Captain::equip  穿装备前属性 ===========", equip_attr_obj.dump());

	const auto item_attr = item->get_attribute(CaptainItemAttributeIds::ID_ATTRIBUTE);

	if (!item_attr.empty() || item_attr != Poseidon::EMPTY_STRING)
	{
		boost::container::flat_map<CaptainAttributeId, std::string> modifiers;

		LOG_EMPERY_CENTER_INFO("Captain::equip  装备自身属性 ===========", item_attr);

		Poseidon::JsonObject  itemobj;
		std::istringstream iss_item(item_attr);
		auto val_item = Poseidon::JsonParser::parse_object(iss_item);
		itemobj = std::move(val_item);

		LOG_EMPERY_CENTER_INFO("Captain::equip  装备自身属性转换后 ===========", itemobj.dump());

		for (auto it = itemobj.begin(); it != itemobj.end(); ++it)
		{
			auto str_attr = std::string(it->first.get());
			const auto nvalue = static_cast<std::uint64_t>(it->second.get<double>());

			std::uint64_t old_value = 0;
			if (equip_attr_obj.find(SharedNts::view(str_attr.c_str())) != equip_attr_obj.end())
			{
				old_value = static_cast<std::uint64_t>(equip_attr_obj.at(SharedNts::view(str_attr.c_str())).get<double>());
			}

			if (bwear)
			{
				equip_attr_obj[SharedNts(str_attr.c_str())] = std::ceil(old_value + nvalue);
			}
			else
			{
				equip_attr_obj[SharedNts(str_attr.c_str())] = std::ceil(old_value - nvalue);
			}

			LOG_EMPERY_CENTER_INFO("Captain::equip  装备自身属性 ===========", str_attr, ",nvalue=", nvalue, ",old_value=", old_value);
		}

		LOG_EMPERY_CENTER_INFO("Captain::equip  穿装备后属性 ===========", equip_attr_obj.dump());
		modifiers[CaptainAttributeIds::ID_EQUIP_ATTRIBUTE] = equip_attr_obj.dump();
		set_attributes(std::move(modifiers));

		// 判断是否有关联的部队,查看对部队的影响
		const auto str_map_object_uuid = get_attribute(CaptainAttributeIds::ID_RELATION_OBJECT_UUID);
		if (!str_map_object_uuid.empty())
		{
			// 计算属性
			const auto map_object_uuid = MapObjectUuid(str_map_object_uuid);
			const auto& map_object = WorldMap::get_map_object(map_object_uuid);
			if (map_object)
			{
				map_object->recalculate_captain_attributes(virtual_shared_from_this<Captain>(), true);
			}
		}

	}
}
	
void Captain::cal_affix_attr(const std::string &affix_attr,const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change){
	PROFILE_ME;

	if(!map_object){
		return;
	}
	Poseidon::JsonObject affix_object;
	std::vector<std::pair<std::uint64_t,double>> attr_vec;
	if(!affix_attr.empty()){
		std::istringstream iss(affix_attr);
		auto val = Poseidon::JsonParser::parse_object(iss);
		affix_object = std::move(val);
	}
	attr_vec.reserve(affix_object.size());
	for(auto it = affix_object.begin(); it != affix_object.end(); ++it)
	{
		try{
			const auto affix_id = boost::lexical_cast<std::uint64_t>(it->first);
			const auto nvalue = it->second.get<double>();
			const auto equip_affix = Data::CaptainEquipaffixConfig::get(affix_id);
			if(!equip_affix){
				LOG_EMPERY_CENTER_WARNING("empty equip affix,affix_id = ",affix_id);
				continue;
			}
			const auto properties_id = equip_affix->properties_id;
			const auto equip_pro = Data::CaptainEquipproConfig::get(properties_id);
			if(!equip_pro){
				LOG_EMPERY_CENTER_WARNING("empty equip pro,properties_id = ",properties_id);
				continue;
			}
			switch(equip_pro->type){
				case AT_SOLIDER:
				{
					if(equip_pro->arm_id == map_object->get_map_object_type_id().get()){
						attr_vec.push_back(std::make_pair(equip_pro->attr_id,nvalue));
					}
				}
					break;
				case AT_BATTALION:
				{
					attr_vec.push_back(std::make_pair(equip_pro->attr_id,nvalue));
				}
					break;
			}
		}catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING(e.what());
		}
	}
	attr_change.reserve(attr_change.size() + attr_vec.size());
	for(auto it = attr_vec.begin(); it != attr_vec.end(); ++it){
		const auto attr_id = it->first;
		const auto attr_value = it->second;
		auto itc = attr_change.find(attr_id);
		if(itc == attr_change.end()){
			attr_change.emplace(attr_id,attr_value);
		}else{
			itc->second += attr_value;
		}
	}
}

void Captain::cal_suit_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change){
	//统计各套装装备数量
	boost::container::flat_map<std::uint64_t,std::uint64_t> suit_map;
	for (auto it = m_equips.begin(); it != m_equips.end(); ++it)
	{
		const auto &captain_item_info = CaptainItemMap::get(CaptainItemUuid((*it)->unlocked_get_captain_item_uuid()));
		if (captain_item_info)
		{
			const auto& item = Data::CaptainItemConfig::get(boost::lexical_cast<std::uint64_t>(captain_item_info->get_attribute(CaptainItemAttributeIds::ID_BASEID)));
			if (item)
			{
				auto its = suit_map.find(item->set_id);
				if(its == suit_map.end()){
					suit_map.emplace(item->set_id,1);
				}else{
					its->second += 1;
				}
			}
		}
	}
	if(suit_map.empty()){
		LOG_EMPERY_CENTER_WARNING("captain empty suit");
		return;
	}
	for(auto it = suit_map.begin(); it != suit_map.end(); ++it){
		const auto set_id = it->first;
		const auto suit_parts_count = it->second;
		// 查找对应的套装属性配置
		const auto &suit_info = Data::CaptainSuitConfig::get(set_id);
		if (suit_info)
		{
			LOG_EMPERY_CENTER_WARNING("suit affix attr,set_id = ",set_id, " count = ",suit_parts_count);
			auto its = suit_info->set_effect.find(suit_parts_count);
			if (its != suit_info->set_effect.end())
			{
				// 取得对应的属性信息
				auto equip_affix = Data::CaptainEquipaffixConfig::get(its->second);
				if(equip_affix)
				{
					Poseidon::JsonObject  affix_object;
					auto str = boost::lexical_cast<std::string>(equip_affix->affix_id);
					auto rand_value = static_cast<std::uint64_t>(Poseidon::rand32(boost::lexical_cast<boost::uint32_t>(equip_affix->properties_range_min), boost::lexical_cast<boost::uint32_t>(equip_affix->properties_range_max)));
					affix_object[SharedNts(str.c_str())] = rand_value;
					const auto affix_attr = affix_object.dump();
					cal_affix_attr(affix_attr,map_object,attr_change);
					LOG_EMPERY_CENTER_WARNING("recal suit affix attr, set_id = ",set_id," affix_attr = ",affix_attr);
				}
			}
		}
	}
}
void Captain::cal_equip_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change){
	PROFILE_ME;

	for (auto it = m_equips.begin(); it != m_equips.end(); ++it)
	{
		const auto &captain_item = CaptainItemMap::get(CaptainItemUuid((*it)->unlocked_get_captain_item_uuid()));
		if(!captain_item){
			continue;
		}
		const auto affix_attr = captain_item->get_attribute(CaptainItemAttributeIds::ID_AFFIX_ATTRIBUTE);
		cal_affix_attr(affix_attr,map_object,attr_change);
	}
}

void Captain::cal_captain_affix_attr(const boost::shared_ptr<MapObject> &map_object,boost::container::flat_map<std::uint64_t,double> &attr_change){
	cal_suit_affix_attr(map_object,attr_change);
	cal_equip_affix_attr(map_object,attr_change);
}

}
