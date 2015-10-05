#include "../precompiled.hpp"
#include "common.hpp"
#include "../singletons/account_map.hpp"
#include "../msg/err_account.hpp"
#include <poseidon/mysql/object_base.hpp>

namespace EmperyPromotion {

namespace MySql {
	namespace {

#define MYSQL_OBJECT_NAME	BothBalanceHistoryResult
#define MYSQL_OBJECT_FIELDS	\
	FIELD_BIGINT_UNSIGNED	(accountId)	\
	FIELD_DATETIME			(timestamp)	\
	FIELD_BIGINT			(deltaBalance)	\
	FIELD_INTEGER_UNSIGNED	(reason)	\
	FIELD_BIGINT_UNSIGNED	(param1)	\
	FIELD_BIGINT_UNSIGNED	(param2)	\
	FIELD_BIGINT_UNSIGNED	(param3)	\
	FIELD_STRING			(remarks)
#include <poseidon/mysql/object_generator.hpp>

	}
}

ACCOUNT_SERVLET("getBothBalanceHistory", /* session */, params){
	const auto &loginName = params.get("loginName");
	const auto &begin = params.get("begin");
	const auto &count = params.get("count");
	const auto &reason = params.get("reason");
	const auto &timeBegin = params.get("timeBegin");
	const auto &timeEnd = params.get("timeEnd");

	Poseidon::JsonObject ret;

	AccountId accountId;
	if(!loginName.empty()){
		auto info = AccountMap::get(loginName);
		if(Poseidon::hasNoneFlagsOf(info.flags, AccountMap::FL_VALID)){
			ret[sslit("errorCode")] = (int)Msg::ERR_NO_SUCH_ACCOUNT;
			ret[sslit("errorMessage")] = "Account is not found";
			return ret;
		}
		accountId = info.accountId;
	}

	std::vector<boost::shared_ptr<MySql::BothBalanceHistoryResult> > objs;
	std::ostringstream ossIn, ossOut;
	ossIn  <<"(SELECT *, CAST(`incomeBalance` AS SIGNED) AS `deltaBalance` FROM `Promotion_IncomeBalanceHistory` WHERE 1=1 ";
	ossOut <<"(SELECT *, - CAST(`outcomeBalance` AS SIGNED) AS `deltaBalance` FROM `Promotion_OutcomeBalanceHistory` WHERE 1=1 ";
	if(!timeBegin.empty()){
		char str[256];
		Poseidon::formatTime(str, sizeof(str), boost::lexical_cast<boost::uint64_t>(timeBegin), false);
		ossIn  <<"AND '" <<str <<"' <= `timestamp` ";
		ossOut <<"AND '" <<str <<"' <= `timestamp` ";
		Poseidon::formatTime(str, sizeof(str), boost::lexical_cast<boost::uint64_t>(timeEnd), false);
		ossIn  <<"AND `timestamp` < '" <<str <<"' ";
		ossOut <<"AND `timestamp` < '" <<str <<"' ";
	}
	if(!reason.empty()){
		auto enumReason = boost::lexical_cast<boost::uint32_t>(reason);
		ossIn  <<"AND `reason` = " <<enumReason <<" ";
		ossOut <<"AND `reason` = " <<enumReason <<" ";
	}
	if(!loginName.empty()){
		ossIn  <<"AND `accountId` = " <<accountId <<" ";
		ossOut <<"AND `accountId` = " <<accountId <<" ";
	}
	ossIn  <<") ";
	ossOut <<") ";
	std::ostringstream oss;
	oss <<ossIn.str() <<" UNION ALL " <<ossOut.str() <<" ORDER BY `timestamp` DESC ";
	if(!begin.empty()){
		auto numBegin = boost::lexical_cast<boost::uint64_t>(begin);
		auto numCount = boost::lexical_cast<boost::uint64_t>(count);
		oss <<"LIMIT " <<numBegin <<", " <<numCount;
	}
	MySql::BothBalanceHistoryResult::batchLoad(objs, oss.str());

	Poseidon::JsonArray history;
	for(auto it = objs.begin(); it != objs.end(); ++it){
		const auto &obj = *it;
		Poseidon::JsonObject elem;
		elem[sslit("timestamp")] = obj->get_timestamp();
		elem[sslit("deltaBalance")] = obj->get_deltaBalance();
		elem[sslit("reason")] = obj->get_reason();
		elem[sslit("param1")] = obj->get_param1();
		elem[sslit("param2")] = obj->get_param2();
		elem[sslit("param3")] = obj->get_param3();
		elem[sslit("remarks")] = obj->unlockedGet_remarks();
		history.emplace_back(std::move(elem));
	}
	ret[sslit("history")] = std::move(history);

	ret[sslit("errorCode")] = (int)Msg::ST_OK;
	ret[sslit("errorMessage")] = "No error";
	return ret;
}

}
