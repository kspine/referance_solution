#ifndef EMPERY_CENTER_LOG_MONGODB_CASTLE_PROTECTION_HPP_
#define EMPERY_CENTER_LOG_MONGODB_CASTLE_PROTECTION_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_CastleProtection
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UUID              (owner_uuid)	\
	FIELD_SIGNED            (delta_preparation_duration)	\
	FIELD_SIGNED            (delta_protection_duration)	\
	FIELD_DATETIME          (protection_end)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
