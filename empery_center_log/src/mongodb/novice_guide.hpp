#ifndef EMPERY_CENTER_LOG_MONGODB_NOVICE_GUIDE_HPP_
#define EMPERY_CENTER_LOG_MONGODB_NOVICE_GUIDE_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenterLog {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   CenterLog_NoviceGuideTrace
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId();}
#define MONGODB_OBJECT_FIELDS \
    FIELD_UUID              (account_uuid)  \
    FIELD_UNSIGNED          (task_id)   \
    FIELD_UNSIGNED          (step_id)   \
    FIELD_DATETIME          (created_time)
#include <poseidon/mongodb/object_generator.hpp>
}
}
#endif
