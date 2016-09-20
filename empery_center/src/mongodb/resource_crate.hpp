#ifndef EMPERY_CENTER_MONGODB_RESOURCE_CRATE_HPP_
#define EMPERY_CENTER_MONGODB_RESOURCE_CRATE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {
	namespace MongoDb {
#define MONGODB_OBJECT_NAME   Center_ResourceCrate
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (resource_crate_uuid)	\
	FIELD_UNSIGNED            (resource_id)	\
	FIELD_UNSIGNED            (amount_max)	\
	FIELD_SIGNED			(x)	\
	FIELD_SIGNED			(y)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UNSIGNED            (amount_remaining)
#include <poseidon/mongodb/object_generator.hpp>
	}
}

#endif