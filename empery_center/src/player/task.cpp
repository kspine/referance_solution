#include "../precompiled.hpp"
#include "common.hpp"
#include "../mmain.hpp"
#include "../msg/cs_task.hpp"
#include "../msg/sc_task.hpp"
#include "../msg/err_task.hpp"
#include "../singletons/task_box_map.hpp"
#include "../task_box.hpp"
#include "../data/task.hpp"
#include "../singletons/item_box_map.hpp"
#include "../item_box.hpp"
#include "../transaction_element.hpp"
#include "../reason_ids.hpp"

namespace EmperyCenter {

PLAYER_SERVLET(Msg::CS_ItemGetAllTasks, account, session, /* req */){
	const auto task_box = TaskBoxMap::require(account->get_account_uuid());
	task_box->pump_status();
	task_box->synchronize_with_player(session);

	return Response();
}

PLAYER_SERVLET(Msg::CS_ItemFetchTaskReward, account, session, req){
	const auto task_box = TaskBoxMap::require(account->get_account_uuid());
	task_box->pump_status();

	const auto item_box = ItemBoxMap::require(account->get_account_uuid());

	const auto utc_now = Poseidon::get_utc_time();

	const auto task_id = TaskId(req.task_id);
	auto info = task_box->get(task_id);
	if(info.expiry_time < utc_now){
		return Response(Msg::ERR_NO_SUCH_TASK) <<task_id;
	}
	if(info.rewarded){
		return Response(Msg::ERR_TASK_REWARD_FETCHED) <<task_id;
	}

	const auto task_data = Data::TaskAbstract::require(task_id);
	for(auto it = task_data->objective.begin(); it != task_data->objective.end(); ++it){
		const auto key = it->first;
		const auto count_finish = it->second.at(0);

		const auto oit = info.progress->find(key);
		if(oit == info.progress->end()){
			LOG_EMPERY_CENTER_DEBUG("Progress element not found: key = ", key);
			return Response(Msg::ERR_TASK_NOT_ACCOMPLISHED) <<key <<",0,0";
		}
		const auto count = oit->second;
		if(count < count_finish){
			LOG_EMPERY_CENTER_DEBUG("Objective not met: key = ", key, ", count = ", count, ", count_finish = ", count_finish);
			return Response(Msg::ERR_TASK_NOT_ACCOMPLISHED) <<key <<',' <<count <<',' <<count_finish;
		}
	}

	std::vector<ItemTransactionElement> transaction;
	transaction.reserve(task_data->reward.size());
	for(auto it = task_data->reward.begin(); it != task_data->reward.end(); ++it){
		transaction.emplace_back(ItemTransactionElement::OP_ADD, it->first, it->second,
			ReasonIds::ID_TASK_REWARD, task_id.get(), 0, 0);
	}
	info.rewarded = true;
	item_box->commit_transaction(transaction, false,
		[&]{ task_box->update(std::move(info)); });

	return Response();
}

}