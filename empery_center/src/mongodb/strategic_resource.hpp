#ifndef EMPERY_CENTER_MONGODB_STRATEGIC_RESOURCE_HPP_
#define EMPERY_CENTER_MONGODB_STRATEGIC_RESOURCE_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_StrategicResource
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_UNSIGNED            (resource_id)	\
	FIELD_UNSIGNED            (resource_amount)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_UNSIGNED            (map_event_id)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
