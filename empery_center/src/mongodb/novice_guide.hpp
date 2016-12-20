#ifndef EMPERY_CENTER_MONGODB_NOVICE_GUIDE_HPP_
#define EMPERY_CENTER_MONGODB_NOVICE_GUIDE_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_NoviceGuide
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(account_uuid,task_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (account_uuid)	\
    FIELD_UNSIGNED          (task_id)	\
	FIELD_UNSIGNED          (step_id)
#include <poseidon/mongodb/object_generator.hpp>
  }
}
#endif