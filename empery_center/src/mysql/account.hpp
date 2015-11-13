#ifndef EMPERY_CENTER_MYSQL_ACCOUNT_HPP_
#define EMPERY_CENTER_MYSQL_ACCOUNT_HPP_

#include <poseidon/mysql/object_base.hpp>

namespace EmperyCenter {

namespace MySql {

#define MYSQL_OBJECT_NAME   Center_Account
#define MYSQL_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_INTEGER_UNSIGNED  (platform_id)	\
	FIELD_STRING            (login_name)	\
	FIELD_STRING            (nick)	\
	FIELD_BIGINT_UNSIGNED   (flags)	\
	FIELD_STRING            (login_token)	\
	FIELD_DATETIME          (login_token_expiry_time)	\
	FIELD_DATETIME          (banned_until)	\
	FIELD_DATETIME          (created_time)
#include <poseidon/mysql/object_generator.hpp>

}

}

#endif
