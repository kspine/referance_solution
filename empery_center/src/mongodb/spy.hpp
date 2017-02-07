#ifndef EMPERY_CENTER_MONGODB_SPY_REPORT_HPP_
#define EMPERY_CENTER_MONGODB_SPY_REPORT_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_SpyReport
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(spy_uuid);}
#define MONGODB_OBJECT_FIELDS \
        FIELD_UUID          (spy_uuid)	\
    	FIELD_UUID          (account_uuid)	\
        FIELD_UUID          (map_object_uuid)	\
        FIELD_SIGNED        (x)	\
    	FIELD_SIGNED        (y) \
        FIELD_UUID          (spy_account_uuid)	\
        FIELD_UUID          (spy_map_object_uuid)	\
        FIELD_SIGNED        (spy_x)	\
    	FIELD_SIGNED        (spy_y)     \
        FIELD_SIGNED        (spy_start_time)	\
        FIELD_SIGNED        (spy_time)	\
        FIELD_UNSIGNED      (spy_mark)	\
        FIELD_SIGNED        (hp)	\
        FIELD_SIGNED        (attack)	\
        FIELD_SIGNED        (defense)	\
        FIELD_SIGNED        (prosperity_degree) \
        FIELD_SIGNED        (map_cell_resource_id)	\
        FIELD_SIGNED        (map_cell_resource_amount)\
        FIELD_UNSIGNED      (spy_status)\
        FIELD_UNSIGNED      (read_status)\
        FIELD_DATETIME      (create_time)\
        FIELD_UNSIGNED      (deleted) \
        FIELD_UNSIGNED      (action) \
        FIELD_UNSIGNED      (spy_action) \
        FIELD_STRING        (castle_resource) \
        FIELD_STRING        (arm_attributes) \
        FIELD_STRING        (build_attributes) \
        FIELD_STRING        (captain_attributes)
#include <poseidon/mongodb/object_generator.hpp>
  }
}
#endif
