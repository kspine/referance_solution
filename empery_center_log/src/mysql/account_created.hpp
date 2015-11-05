#ifndef EMPERY_CENTER_LOG_MYSQL_ACCOUNT_CREATED_HPP_
#define EMPERY_CENTER_LOG_MYSQL_ACCOUNT_CREATED_HPP_

#include <poseidon/mysql/object_base.hpp>

namespace EmperyCenterLog {

namespace MySql {

#define MYSQL_OBJECT_NAME	CenterLog_AccountCreated
#define MYSQL_OBJECT_FIELDS	\
	FIELD_DATETIME			(timestamp)	\
	FIELD_UUID				(accountUuid)	\
	FIELD_STRING			(remoteIp)
#include <poseidon/mysql/object_generator.hpp>

}

}

#endif
