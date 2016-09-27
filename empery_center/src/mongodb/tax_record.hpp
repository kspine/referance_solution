#ifndef EMPERY_CENTER_MONGODB_TAX_RECORD_HPP_
#define EMPERY_CENTER_MONGODB_TAX_RECORD_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_TaxRecord
#define MONGODB_OBJECT_PRIMARY_KEY auto_uuid
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (auto_uuid)	\
	FIELD_UUID              (account_uuid)	\
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (from_account_uuid)	\
	FIELD_UNSIGNED            (reason)	\
	FIELD_UNSIGNED            (old_amount)	\
	FIELD_UNSIGNED            (new_amount)	\
	FIELD_UNSIGNED            (delta_amount)	\
	FIELD_BOOLEAN           (deleted)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
