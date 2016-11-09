#ifndef EMPERY_CENTER_MONGODB_MAP_EVENT_HPP_
#define EMPERY_CENTER_MONGODB_MAP_EVENT_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_MapEventBlock
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenCID(block_x,block_y);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (block_x)	\
	FIELD_SIGNED            (block_y)	\
	FIELD_DATETIME          (next_refresh_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_MapEvent
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenBID(x,y);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_SIGNED            (x)	\
	FIELD_SIGNED            (y)	\
	FIELD_DATETIME          (created_time)	\
	FIELD_DATETIME          (expiry_time)	\
	FIELD_SIGNED            (map_event_id)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
