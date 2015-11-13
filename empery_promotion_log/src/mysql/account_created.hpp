#ifndef EMPERY_PROMOTION_LOG_MYSQL_ACCOUNT_CREATED_HPP_
#define EMPERY_PROMOTION_LOG_MYSQL_ACCOUNT_CREATED_HPP_

#include <poseidon/mysql/object_base.hpp>

namespace EmperyPromotionLog {

namespace MySql {

#define MYSQL_OBJECT_NAME   PromotionLog_AccountCreated
#define MYSQL_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_BIGINT_UNSIGNED   (account_id)	\
	FIELD_STRING            (remote_ip)
#include <poseidon/mysql/object_generator.hpp>

}

}

#endif
