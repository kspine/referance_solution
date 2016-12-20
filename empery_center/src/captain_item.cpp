#include "precompiled.hpp"
#include "account.hpp"
#include "mmain.hpp"
#include "mongodb/captain.hpp"
#include "captain_item.hpp"
#include "singletons/captain_item_map.hpp"
#include "data/captain_item_config.hpp"
#include "data/captain_equipaffix_config.hpp"
#include "captain_item_attribute_ids.hpp"
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>


namespace EmperyCenter {

	std::pair<boost::shared_ptr<const Poseidon::JobPromise>, boost::shared_ptr<CaptainItem>> CaptainItem::async_create(
		CaptainItemUuid captainitem_uuid, AccountUuid account_uuid, std::uint64_t created_time)
{
	PROFILE_ME;

	auto obj = boost::make_shared<MongoDb::Center_CaptainItem>(captainitem_uuid.get(), account_uuid.get(), created_time);
	obj->enable_auto_saving();
	auto promise = Poseidon::MongoDbDaemon::enqueue_for_saving(obj, false, true);
	auto captain = boost::make_shared<CaptainItem>(std::move(obj), std::vector<boost::shared_ptr<MongoDb::Center_CaptainItemAttribute>>());
	return std::make_pair(std::move(promise), std::move(captain));
}

	CaptainItem::CaptainItem(boost::shared_ptr<MongoDb::Center_CaptainItem> obj,
		const std::vector<boost::shared_ptr<MongoDb::Center_CaptainItemAttribute>> &attributes)
	: m_obj(std::move(obj))
{
	for(auto it = attributes.begin(); it != attributes.end(); ++it){
		m_attributes.emplace(CaptainItemAttributeId((*it)->get_attribute_id()), *it);
	}
}
	CaptainItem::~CaptainItem(){
}

CaptainItemUuid CaptainItem::get_captain_item_uuid() const {
	return CaptainItemUuid(m_obj->unlocked_get_captain_item_uuid());
}

AccountUuid CaptainItem::get_account_uuid() const{
	return AccountUuid(m_obj->unlocked_get_account_uuid());
}

void CaptainItem::InitAttributes(std::uint64_t baseid)
{
	PROFILE_ME;

	const auto& config = Data::CaptainItemConfig::get(baseid);
	if (config)
	{
		// 设置属性
		boost::container::flat_map<CaptainItemAttributeId, std::string> modifiers;

		modifiers.emplace(CaptainItemAttributeIds::ID_BASEID, std::move(boost::lexical_cast<std::string>(baseid)));
		modifiers.emplace(CaptainItemAttributeIds::ID_INUSE, "0");
		modifiers.emplace(CaptainItemAttributeIds::ID_LEVEL, std::move(boost::lexical_cast<std::string>(config->level)));
		modifiers.emplace(CaptainItemAttributeIds::ID_QUALITY, std::move(boost::lexical_cast<std::string>(config->quality)));
		modifiers.emplace(CaptainItemAttributeIds::ID_ATTRIBUTE, config->basic_properties_array.dump());


		// 根据品质，取得词缀属性，设置属性
		LOG_EMPERY_CENTER_WARNING("InitAttributes 基础属性 ", config->basic_properties_array.dump());

		if (config->affix_num > 0)
		{
			// 可以取词缀属性

			Poseidon::JsonObject  obj;
			obj = std::move(config->basic_properties_array);

			std::vector<boost::shared_ptr<Data::CaptainEquipaffixConfig> > rets;
			Data::CaptainEquipaffixConfig::get_affix_by_level_pos(rets, config->level, config->position, config->affix_num);

			LOG_EMPERY_CENTER_WARNING("get_affix_by_level_pos return ret size ", rets.size());
			modifiers.emplace(CaptainItemAttributeIds::ID_ATTRIBUTE, std::move(obj.dump()));

			Poseidon::JsonObject  Affixobj;
			for (auto it = rets.begin(); it != rets.end(); ++it)
			{
				auto affix = *it;
				auto str = boost::lexical_cast<std::string>(affix->properties_id);
				auto rand_value = static_cast<std::uint64_t>(Poseidon::rand32(boost::lexical_cast<boost::uint32_t>(affix->properties_range_min), boost::lexical_cast<boost::uint32_t>(affix->properties_range_max)));
				auto strAffix = boost::lexical_cast<std::string>(affix->affix_id);
				Affixobj[SharedNts(strAffix.c_str())] = rand_value;
			}
			LOG_EMPERY_CENTER_WARNING("InitAttributes 词缀属性加成 ", Affixobj.dump());
			modifiers.emplace(CaptainItemAttributeIds::ID_AFFIX_ATTRIBUTE, std::move(Affixobj.dump()));
		}
		else
		{
			modifiers.emplace(CaptainItemAttributeIds::ID_AFFIX_ATTRIBUTE, "{}");
		}
		set_attributes(std::move(modifiers));
	}
	else
	{
		LOG_EMPERY_CENTER_WARNING("CaptainItem can not find: baseid = ", baseid);
		DEBUG_THROW(Exception, sslit("CaptainItem can not  exists"));
	}

}


std::uint64_t CaptainItem::get_created_time() const {
	return m_obj->get_created_time();
}

const std::string &CaptainItem::get_attribute(CaptainItemAttributeId attribute_id) const {
	PROFILE_ME;

	const auto it = m_attributes.find(attribute_id);
	if(it == m_attributes.end()){
		return Poseidon::EMPTY_STRING;
	}
	return it->second->unlocked_get_value();
}
void CaptainItem::get_attributes(boost::container::flat_map<CaptainItemAttributeId, std::string> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_attributes.size());
	for(auto it = m_attributes.begin(); it != m_attributes.end(); ++it){
		ret[it->first] = it->second->unlocked_get_value();
	}
}
void CaptainItem::set_attributes(boost::container::flat_map<CaptainItemAttributeId, std::string> modifiers){
	PROFILE_ME;


	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		const auto obj_it = m_attributes.find(it->first);
		if(obj_it == m_attributes.end()){
			auto obj = boost::make_shared<MongoDb::Center_CaptainItemAttribute>(m_obj->get_captain_item_uuid(),
				it->first.get(), std::string());
			obj->async_save(true);
			m_attributes.emplace(it->first, std::move(obj));
		}
	}

	for(auto it = modifiers.begin(); it != modifiers.end(); ++it){
		const auto &obj = m_attributes.at(it->first);
		obj->set_value(std::move(it->second));
	}

	CaptainItemMap::update(virtual_shared_from_this<CaptainItem>(), false);
}

void CaptainItem::delete_data()
{
	PROFILE_ME;

	// 删除属性表
	Poseidon::MongoDb::BsonBuilder query;
	query.append_regex(sslit("_id"),get_captain_item_uuid().str());
		
    LOG_EMPERY_CENTER_ERROR("query _id:captain_item_uuid=",get_captain_item_uuid().get());

    Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_CaptainItemAttribute", query, true);
}

void CaptainItem::set_use_status(std::string str)
{
	PROFILE_ME;

	boost::container::flat_map<CaptainItemAttributeId, std::string> modifiers;

	modifiers.emplace(CaptainItemAttributeIds::ID_INUSE, str);

	set_attributes(std::move(modifiers));
}

}