#ifndef EMPERY_CENTER_MONGODB_OVERLAY_HPP_
#define EMPERY_CENTER_MONGODB_OVERLAY_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_Overlay
#define MONGODB_OBJECT_PRIMARY_KEY cluster_x cluster_y overlay_group_name
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (cluster_x)	\
	FIELD_SIGNED            (cluster_y)	\
	FIELD_STRING            (overlay_group_name)	\
	FIELD_SIGNED            (overlay_id)	\
	FIELD_SIGNED            (resource_id)	\
	FIELD_SIGNED            (resource_amount)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
