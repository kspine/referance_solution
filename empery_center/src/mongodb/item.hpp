#ifndef EMPERY_CENTER_MONGODB_ITEM_HPP_
#define EMPERY_CENTER_MONGODB_ITEM_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Item
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
	FIELD_UNSIGNED            (item_id)	\
	FIELD_UNSIGNED            (count)	\
	FIELD_DATETIME          (updated_time)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
