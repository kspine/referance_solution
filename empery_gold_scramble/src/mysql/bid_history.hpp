#ifndef EMPERY_GOLD_SCRAMBLE_MYSQL_BID_HISTORY_HPP_
#define EMPERY_GOLD_SCRAMBLE_MYSQL_BID_HISTORY_HPP_

#include <poseidon/mysql/object_base.hpp>

namespace EmperyGoldScramble {

namespace MySql {

#define MYSQL_OBJECT_NAME   GoldScramble_BidHistory
#define MYSQL_OBJECT_FIELDS \
	FIELD_BIGINT_UNSIGNED   (record_auto_id)	\
	FIELD_BIGINT_UNSIGNED   (game_auto_id)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_STRING            (login_name)	\
	FIELD_STRING            (nick)	\
	FIELD_BIGINT_UNSIGNED   (gold_coins)	\
	FIELD_BIGINT_UNSIGNED   (account_balance)
#include <poseidon/mysql/object_generator.hpp>

}

}

#endif
