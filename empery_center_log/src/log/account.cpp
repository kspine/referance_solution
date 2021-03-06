#include "../precompiled.hpp"
#include "common.hpp"
#include "../../../empery_center/src/msg/sl_account.hpp"
#include "../mongodb/account_created.hpp"
#include "../mongodb/account_logged_in.hpp"
#include "../mongodb/account_logged_out.hpp"
#include "../mongodb/account_number_online.hpp"

namespace EmperyCenterLog {

LOG_SERVLET(Msg::SL_AccountCreated, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_AccountCreated>(
		Poseidon::get_utc_time(),
		Poseidon::Uuid(req.account_uuid), std::move(req.remote_ip));
	obj->async_save(false, true);
}

LOG_SERVLET(Msg::SL_AccountLoggedIn, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_AccountLoggedIn>(
		Poseidon::get_utc_time(),
		Poseidon::Uuid(req.account_uuid), std::move(req.remote_ip));
	obj->async_save(false, true);
}

LOG_SERVLET(Msg::SL_AccountLoggedOut, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_AccountLoggedOut>(
		Poseidon::get_utc_time(),
		Poseidon::Uuid(req.account_uuid), req.online_duration);
	obj->async_save(false, true);
}

LOG_SERVLET(Msg::SL_AccountNumberOnline, log, req){
	auto obj = boost::make_shared<MongoDb::CenterLog_AccountNumberOnline>(
		Poseidon::get_utc_time(),
		req.interval, req.account_count);
	obj->async_save(false, true);
}

}
