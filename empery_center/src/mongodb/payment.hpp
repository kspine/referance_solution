#ifndef EMPERY_CENTER_MONGODB_PAYMENT_HPP_
#define EMPERY_CENTER_MONGODB_PAYMENT_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_PaymentTransaction
#define MONGODB_OBJECT_PRIMARY_KEY {return serial;}
#define MONGODB_OBJECT_FIELDS \
	FIELD_STRING            (serial)	\
	FIELD_UUID              (account_uuid)	\
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
