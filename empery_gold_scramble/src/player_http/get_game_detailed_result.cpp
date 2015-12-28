#include "../precompiled.hpp"
#include "common.hpp"
#include <poseidon/mysql/object_base.hpp>

namespace EmperyGoldScramble {

namespace MySql {
	namespace {

#define MYSQL_OBJECT_NAME   LoginNameAndResults
#define MYSQL_OBJECT_FIELDS \
	FIELD_STRING            (login_name)	\
	FIELD_BIGINT_UNSIGNED   (gold_coins)	\
	FIELD_BIGINT_UNSIGNED   (account_balance)
#include <poseidon/mysql/object_generator.hpp>

	}
}

PLAYER_HTTP_SERVLET("getGameDetailedResult", session, params){
	const auto game_auto_id = boost::lexical_cast<std::uint64_t>(params.at("gameAutoId"));

	Poseidon::JsonObject ret;

	std::vector<boost::shared_ptr<MySql::LoginNameAndResults>> objs;
	std::ostringstream oss;
	oss <<"SELECT `login_name`, SUM(`gold_coins`) AS `gold_coins`, SUM(`account_balance`) AS `account_balance` "
	    <<"  FROM `GoldScramble_BidHistory` "
	    <<"  WHERE `game_auto_id` = " <<game_auto_id
	    <<"  GROUP BY `login_name`";
	MySql::LoginNameAndResults::batch_load(objs, oss.str());

	Poseidon::JsonArray history;
	for(auto it = objs.begin(); it != objs.end(); ++it){
		const auto &obj = *it;
		Poseidon::JsonObject elem;
		elem[sslit("loginName")]      = obj->unlocked_get_login_name();
		elem[sslit("goldCoins")]      = obj->get_gold_coins();
		elem[sslit("accountBalance")] = obj->get_account_balance();
		history.emplace_back(std::move(elem));
	}
	ret[sslit("bidHistory")] = std::move(history);

	objs.clear();
	oss.str({ });
	oss <<"SELECT `login_name`, SUM(`gold_coins_won`) AS `gold_coins`, SUM(`account_balance_won`) AS `account_balance` "
	    <<"  FROM `GoldScramble_GameHistory` "
	    <<"  WHERE `game_auto_id` = " <<game_auto_id
	    <<"  GROUP BY `login_name`";
	MySql::LoginNameAndResults::batch_load(objs, oss.str());

	history.clear();
	for(auto it = objs.begin(); it != objs.end(); ++it){
		const auto &obj = *it;
		Poseidon::JsonObject elem;
		elem[sslit("loginName")]      = obj->unlocked_get_login_name();
		elem[sslit("goldCoins")]      = obj->get_gold_coins();
		elem[sslit("accountBalance")] = obj->get_account_balance();
		history.emplace_back(std::move(elem));
	}
	ret[sslit("gameHistory")] = std::move(history);

	ret[sslit("errorCode")] = 0;
	ret[sslit("errorMessage")] = "No error";
	return ret;
}

}
