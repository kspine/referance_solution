#include "precompiled.hpp"
#include "task_box.hpp"
#include <poseidon/json.hpp>
#include "msg/sc_task.hpp"
#include "mysql/task.hpp"
#include "singletons/player_session_map.hpp"
#include "player_session.hpp"
#include "data/task.hpp"
#include "data/item.hpp"
#include "item_ids.hpp"
#include "map_object_type_ids.hpp"
#include "task_type_ids.hpp"
#include "singletons/world_map.hpp"
#include "castle.hpp"

namespace EmperyCenter {

namespace {
	std::string encode_progress(const TaskBox::Progress &progress){
		PROFILE_ME;

		if(progress.empty()){
			return { };
		}
		Poseidon::JsonObject root;
		for(auto it = progress.begin(); it != progress.end(); ++it){
			const auto key = it->first;
			const auto count = it->second;
			char str[256];
			unsigned len = (unsigned)std::sprintf(str, "%lu", (unsigned long)key);
			root[SharedNts(str, len)] = count;
		}
		std::ostringstream oss;
		root.dump(oss);
		return oss.str();
	}
	TaskBox::Progress decode_progress(const std::string &str){
		PROFILE_ME;

		TaskBox::Progress progress;
		if(str.empty()){
			return progress;
		}
		std::istringstream iss(str);
		auto root = Poseidon::JsonParser::parse_object(iss);
		progress.reserve(root.size());
		for(auto it = root.begin(); it != root.end(); ++it){
			const auto key = boost::lexical_cast<std::uint64_t>(it->first);
			const auto count = static_cast<std::uint64_t>(it->second.get<double>());
			progress.emplace(key, count);
		}
		return progress;
	}

	bool has_task_been_accomplished(const Data::TaskAbstract *task_data, const TaskBox::Progress &progress){
		PROFILE_ME;

		for(auto it = task_data->objective.begin(); it != task_data->objective.end(); ++it){
			const auto key = it->first;
			const auto count_finish = it->second.at(0);

			const auto pit = progress.find(key);
			if(pit == progress.end()){
				LOG_EMPERY_CENTER_DEBUG("Progress element not found: key = ", key);
				return false;
			}
			if(pit->second < count_finish){
				LOG_EMPERY_CENTER_DEBUG("Progress element unmet: key = ", key, ", count = ", pit->second, ", count_finish = ", count_finish);
				return false;
			}
		}
		return true;
	}

	using TaskObjectPair = std::pair<boost::shared_ptr<MySql::Center_Task>, TaskBox::Progress>;

	void fill_task_info(TaskBox::TaskInfo &info, const boost::shared_ptr<TaskObjectPair> &pair){
		PROFILE_ME;

		const auto &obj = pair->first;
		const auto &progress = pair->second;

		info.task_id      = TaskId(obj->get_task_id());
		info.created_time = obj->get_created_time();
		info.expiry_time  = obj->get_expiry_time();
		info.progress     = boost::shared_ptr<const TaskBox::Progress>(pair, &progress);
		info.rewarded     = obj->get_rewarded();
	}

	void fill_task_message(Msg::SC_TaskChanged &msg, const boost::shared_ptr<TaskObjectPair> &pair){
		PROFILE_ME;

		const auto &obj = pair->first;
		const auto &progress = pair->second;

		msg.task_id       = obj->get_task_id();
		msg.created_time  = obj->get_created_time();
		msg.expiry_time   = obj->get_expiry_time();
		msg.progress.reserve(progress.size());
		for(auto it = progress.begin(); it != progress.end(); ++it){
			auto &elem = *msg.progress.emplace(msg.progress.end());
			elem.key = it->first;
			elem.count = it->second;
		}
		msg.rewarded      = obj->get_rewarded();
	}
}

TaskBox::TaskBox(AccountUuid account_uuid,
	const std::vector<boost::shared_ptr<MySql::Center_Task>> &tasks)
	: m_account_uuid(account_uuid)
{
	for(auto it = tasks.begin(); it != tasks.end(); ++it){
		const auto &obj = *it;
		const auto task_id = TaskId(obj->get_task_id());
		if(!task_id){
			m_stamps = obj;
		} else {
			m_tasks.emplace(task_id, boost::make_shared<TaskObjectPair>(obj, decode_progress(obj->unlocked_get_progress())));
		}
	}
	if(!m_stamps){
		auto obj = boost::make_shared<MySql::Center_Task>(get_account_uuid().get(), 0, 0, 0, std::string(), false);
		obj->async_save(true);
		m_stamps = std::move(obj);
	}
}
TaskBox::~TaskBox(){
}

void TaskBox::pump_status(){
	PROFILE_ME;
	LOG_EMPERY_CENTER_TRACE("Checking tasks: account_uuid = ", get_account_uuid());
/*
	const auto utc_now = Poseidon::get_utc_time();

*/
	// TODO
}

void TaskBox::check_init_tasks(){
	PROFILE_ME;
	LOG_EMPERY_CENTER_TRACE("Checking for initial tasks: account_uuid = ", get_account_uuid());

	const auto item_data = Data::Item::require(ItemIds::ID_TASK_DAILY_RESET);
	if(item_data->auto_inc_type != Data::Item::AIT_DAILY){
		LOG_EMPERY_CENTER_ERROR("Task daily reset item is not daily-reset?");
		DEBUG_THROW(Exception, sslit("Task daily reset item is not daily-reset"));
	}
	const auto auto_inc_offset = item_data->auto_inc_offset;

	const auto account_uuid = get_account_uuid();
	const auto utc_now = Poseidon::get_utc_time();

	// 删除旧任务。

	{
		auto it = m_tasks.begin();
		while(it != m_tasks.end()){
			const auto &obj = it->second->first;
			if(obj->get_expiry_time() < utc_now){
				++it;
				continue;
			}
			it = m_tasks.erase(it);
		}
	}

	// 检查新任务。

	{
		std::vector<boost::shared_ptr<const Data::TaskPrimary>> task_data_primary;
		Data::TaskPrimary::get_all(task_data_primary);
		for(auto it = task_data_primary.begin(); it != task_data_primary.end(); ++it){
			const auto &task_data = *it;
			const auto previous_id = task_data->previous_id;
			if(previous_id && !has_been_accomplished(previous_id)){
				continue;
			}
			const auto task_id = task_data->task_id;
			if(m_tasks.find(task_id) != m_tasks.end()){
				continue;
			}
			LOG_EMPERY_CENTER_DEBUG("New primary task: account_uuid = ", account_uuid, ", task_id = ", task_id);
			TaskInfo info = { };
			info.task_id      = task_id;
			info.created_time = utc_now;
			info.expiry_time  = UINT64_MAX;
			insert(std::move(info));
		}
	}

	// m_stamps 特殊：
	// created_time 是上次每日任务刷新时间。
	const auto last_refreshed_time = m_stamps->get_created_time();
	const auto last_refreshed_day = saturated_sub(last_refreshed_time, auto_inc_offset) / 86400000;
	const auto today = saturated_sub(utc_now, auto_inc_offset) / 86400000;
	LOG_EMPERY_CENTER_DEBUG("Checking for new daily task: last_refreshed_day = ", last_refreshed_day, ", today = ", today);
	if(last_refreshed_day < today){
		unsigned account_level = 0;
		std::vector<boost::shared_ptr<MapObject>> map_objects;
		WorldMap::get_map_objects_by_owner_and_type(map_objects, account_uuid, MapObjectTypeIds::ID_CASTLE);
		for(auto it = map_objects.begin(); it != map_objects.end(); ++it){
			const auto &map_object = *it;
			const auto castle = boost::dynamic_pointer_cast<Castle>(map_object);
			if(!castle){
				continue;
			}
			const auto current_level = castle->get_level();
			LOG_EMPERY_CENTER_DEBUG("> Found castle: account_uuid = ", account_uuid, ", current_level = ", current_level);
			if(account_level < current_level){
				account_level = current_level;
			}
		}
		LOG_EMPERY_CENTER_DEBUG("Account level: account_uuid = ", account_uuid, ", account_level = ", account_level);

		std::vector<boost::shared_ptr<const Data::TaskDaily>> task_data_daily;
		Data::TaskDaily::get_all(task_data_daily);
		for(auto it = task_data_daily.begin(); it != task_data_daily.end(); ++it){
			const auto &task_data = *it;
			if((account_level < task_data->level_limit_min) || (task_data->level_limit_max < account_level)){
				continue;
			}
			const auto task_id = task_data->task_id;
			if(m_tasks.find(task_id) != m_tasks.end()){
				continue;
			}
			LOG_EMPERY_CENTER_DEBUG("New daily task: account_uuid = ", account_uuid, ", task_id = ", task_id);
			TaskInfo info = { };
			info.task_id      = task_id;
			info.created_time = utc_now;
			info.expiry_time  = (today + 1) * 86400000;
			insert(std::move(info));
		}
		m_stamps->set_created_time(utc_now);
	}
}

TaskBox::TaskInfo TaskBox::get(TaskId task_id) const {
	PROFILE_ME;

	TaskInfo info = { };
	info.task_id = task_id;

	const auto it = m_tasks.find(task_id);
	if(it == m_tasks.end()){
		return info;
	}
	fill_task_info(info, it->second);
	return info;
}
void TaskBox::get_all(std::vector<TaskBox::TaskInfo> &ret) const {
	PROFILE_ME;

	ret.reserve(ret.size() + m_tasks.size());
	for(auto it = m_tasks.begin(); it != m_tasks.end(); ++it){
		TaskInfo info;
		fill_task_info(info, it->second);
		ret.emplace_back(std::move(info));
	}
}

void TaskBox::insert(TaskBox::TaskInfo info){
	PROFILE_ME;

	const auto task_id = info.task_id;
	const auto it = m_tasks.find(task_id);
	if(it != m_tasks.end()){
		LOG_EMPERY_CENTER_WARNING("Task exists: account_uuid = ", get_account_uuid(), ", task_id = ", task_id);
		DEBUG_THROW(Exception, sslit("Task exists"));
	}

	Progress progress;
	if(info.progress){
		progress = *info.progress;
	}
	const auto obj = boost::make_shared<MySql::Center_Task>(get_account_uuid().get(), task_id.get(),
		info.created_time, info.expiry_time, encode_progress(progress), info.rewarded);
	obj->async_save(true);
	const auto pair = boost::make_shared<TaskObjectPair>(obj, std::move(progress));
	m_tasks.emplace(task_id, pair);

	const auto session = PlayerSessionMap::get(get_account_uuid());
	if(session){
		try {
			Msg::SC_TaskChanged msg;
			fill_task_message(msg, pair);
			session->send(msg);
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			session->shutdown(e.what());
		}
	}
}
void TaskBox::update(TaskBox::TaskInfo info, bool throws_if_not_exists){
	PROFILE_ME;

	const auto task_id = info.task_id;
	const auto it = m_tasks.find(task_id);
	if(it == m_tasks.end()){
	    LOG_EMPERY_CENTER_WARNING("Task not found: account_uuid = ", get_account_uuid(), ", task_uuid = ", task_id);
		if(throws_if_not_exists){
			DEBUG_THROW(Exception, sslit("Task not found"));
		}
		return;
	}
	const auto &pair = it->second;

	std::string progress_str;
	bool reset_progress = false;
	if(pair.owner_before(info.progress) || info.progress.owner_before(pair)){
		progress_str = encode_progress(*info.progress);
		reset_progress = true;
	}

	pair->first->set_created_time(info.created_time);
	pair->first->set_expiry_time(info.expiry_time);
	if(reset_progress){
		pair->first->set_progress(std::move(progress_str));
	}
	pair->first->set_rewarded(info.rewarded);

	const auto session = PlayerSessionMap::get(get_account_uuid());
	if(session){
	    try {
        	Msg::SC_TaskChanged msg;
        	fill_task_message(msg, pair);
        	session->send(msg);
    	} catch(std::exception &e){
        	LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
        	session->shutdown(e.what());
    	}
	}
}
bool TaskBox::remove(TaskId task_id) noexcept {
	PROFILE_ME;

	const auto it = m_tasks.find(task_id);
	if(it == m_tasks.end()){
		return false;
	}
	const auto pair = std::move(it->second);
	m_tasks.erase(it);

	pair->first->set_expiry_time(0);

	const auto session = PlayerSessionMap::get(get_account_uuid());
	if(session){
		try {
			Msg::SC_TaskChanged msg;
			fill_task_message(msg, pair);
			session->send(msg);
		} catch(std::exception &e){
			LOG_EMPERY_CENTER_WARNING("std::exception thrown: what = ", e.what());
			session->shutdown(e.what());
		}
	}

	return true;
}

void TaskBox::check(TaskTypeId type, std::uint64_t key, std::uint64_t count,
	bool is_primary_castle, std::uint64_t param1, std::uint64_t param2, std::uint64_t param3)
{
	PROFILE_ME;

	// TODO
}
bool TaskBox::has_been_accomplished(TaskId task_id) const {
	PROFILE_ME;

	const auto it = m_tasks.find(task_id);
	if(it == m_tasks.end()){
		return false;
	}
	const auto task_data = Data::TaskAbstract::require(task_id);
	return has_task_been_accomplished(task_data.get(), it->second->second);
}

void TaskBox::synchronize_with_player(const boost::shared_ptr<PlayerSession> &session) const {
	PROFILE_ME;

	for(auto it = m_tasks.begin(); it != m_tasks.end(); ++it){
		Msg::SC_TaskChanged msg;
		fill_task_message(msg, it->second);
		session->send(msg);
	}
}

}
