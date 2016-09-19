#ifndef EMPERY_CENTER_LOG_MONGODB_MAP_EVENTS_OVERFLOWED_HPP_
#define EMPERY_CENTER_LOG_MONGODB_MAP_EVENTS_OVERFLOWED_HPP_

#include <poseidon/mongodb/object_base.hpp>

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_MapEventsOverflowed
#define MONGODB_OBJECT_FIELDS \
	FIELD_DATETIME          (timestamp)	\
	FIELD_SIGNED            (block_x)	\
	FIELD_SIGNED            (block_y)	\
	FIELD_SIGNED            (width)	\
	FIELD_SIGNED            (height)	\
	FIELD_SIGNED            (active_castle_count)	\
	FIELD_SIGNED            (map_event_id)	\
	FIELD_SIGNED            (events_to_refresh)	\
	FIELD_SIGNED            (events_retained)	\
	FIELD_SIGNED            (events_created)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
