#ifndef EMPERY_CENTER_MYSQL_AUCTION_HPP_
#define EMPERY_CENTER_MYSQL_AUCTION_HPP_

#include <poseidon/mysql/object_base.hpp>

namespace EmperyCenter {

namespace MySql {

#define MYSQL_OBJECT_NAME   Center_AuctionTransfer
#define MYSQL_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (map_object_uuid)	\
	FIELD_INTEGER_UNSIGNED  (item_id)	\
	FIELD_BIGINT_UNSIGNED   (item_count_locked)	\
	FIELD_BIGINT_UNSIGNED   (item_count_fee)	\
	FIELD_INTEGER_UNSIGNED  (resource_id)	\
	FIELD_BIGINT_UNSIGNED   (resource_amount_locked)	\
	FIELD_BIGINT_UNSIGNED   (resource_amount_fee)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (due_time)
#include <poseidon/mysql/object_generator.hpp>

}

}

#endif
