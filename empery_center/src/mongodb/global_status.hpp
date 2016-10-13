#ifndef EMPERY_CENTER_MONGODB_GLOBAL_STATUS_HPP_
#define EMPERY_CENTER_MONGODB_GLOBAL_STATUS_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_GlobalStatus
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(slot);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (slot)	\
	FIELD_STRING            (value)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
