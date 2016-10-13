#ifndef EMPERY_CENTER_MONGODB_AUCTION_HPP_
#define EMPERY_CENTER_MONGODB_AUCTION_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_AuctionTransfer
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,map_object_uuid,item_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (item_id)	\
	FIELD_UNSIGNED            (item_count)	\
	FIELD_UNSIGNED            (item_count_locked)	\
	FIELD_UNSIGNED            (item_count_fee)	\
	FIELD_UNSIGNED            (resource_id)	\
	FIELD_UNSIGNED            (resource_amount_locked)	\
	FIELD_UNSIGNED            (resource_amount_fee)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (due_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_AuctionTransaction
#define MONGODB_OBJECT_PRIMARY_KEY {return serial;}
#define MONGODB_OBJECT_FIELDS \
	FIELD_STRING            (serial)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED            (operation)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UNSIGNED            (item_id)	\
	FIELD_UNSIGNED            (item_count)	\
	FIELD_STRING            (remarks)	\
	FIELD_DATETIME          (last_updated_time)	\
	FIELD_BOOLEAN           (committed)	\
	FIELD_BOOLEAN           (cancelled)	\
	FIELD_STRING            (operation_remarks)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
