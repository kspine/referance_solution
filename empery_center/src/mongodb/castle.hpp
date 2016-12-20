#ifndef EMPERY_CENTER_MONGODB_CASTLE_HPP_
#define EMPERY_CENTER_MONGODB_CASTLE_HPP_

#include <poseidon/mongodb/object_base.hpp>

#include "../primerykeygen.hpp"

namespace EmperyCenter {

namespace MongoDb {

#define MONGODB_OBJECT_NAME   Center_CastleBuildingBase
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,building_base_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (building_base_id)	\
	FIELD_UNSIGNED            (building_id)	\
	FIELD_UNSIGNED            (building_level)	\
	FIELD_UNSIGNED            (mission)	\
	FIELD_UNSIGNED            (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleTech
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,tech_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (tech_id)	\
	FIELD_UNSIGNED            (tech_level)	\
	FIELD_UNSIGNED            (mission)	\
	FIELD_UNSIGNED            (mission_duration)	\
	FIELD_DATETIME          (mission_time_begin)	\
	FIELD_DATETIME          (mission_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleResource
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,resource_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (resource_id)	\
	FIELD_UNSIGNED            (amount)	\
	FIELD_DATETIME          (updated_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleBattalion
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,map_object_type_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (map_object_type_id)	\
	FIELD_UNSIGNED            (count)	\
	FIELD_BOOLEAN           (enabled)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleBattalionProduction
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,building_base_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (building_base_id)	\
	FIELD_UNSIGNED            (map_object_type_id)	\
	FIELD_UNSIGNED            (count)	\
	FIELD_UNSIGNED            (production_duration)	\
	FIELD_DATETIME          (production_time_begin)	\
	FIELD_DATETIME          (production_time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleWoundedSoldier
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,map_object_type_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (map_object_type_id)	\
	FIELD_UNSIGNED            (count)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleTreatment
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,map_object_type_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED            (map_object_type_id)	\
	FIELD_UNSIGNED            (count)	\
	FIELD_UNSIGNED            (duration)	\
	FIELD_DATETIME          (time_begin)	\
	FIELD_DATETIME          (time_end)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleTechEra
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,tech_era);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED          (tech_era)	\
	FIELD_BOOLEAN           (unlocked)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleResourceBattalionUnload
#define MONGODB_OBJECT_PRIMARY_KEY {return PRIMERY_KEYGEN::GenIDS::GenId(map_object_uuid,resource_id);}
#define MONGODB_OBJECT_FIELDS \
	FIELD_UUID              (map_object_uuid)	\
	FIELD_UNSIGNED  (resource_id)	\
	FIELD_UNSIGNED   (delta)	\
	FIELD_DATETIME          (updated_time)
#include <poseidon/mongodb/object_generator.hpp>

#define MONGODB_OBJECT_NAME   Center_CastleOfflineUpgradeBuildingBase
#define MONGODB_OBJECT_PRIMARY_KEY {return auto_uuid;}
#define MONGODB_OBJECT_FIELDS \
  FIELD_STRING            (auto_uuid) \
	FIELD_UUID              (account_uuid)	\
	FIELD_UUID              (map_object_uuid)	\
	FIELD_STRING            (building_base_ids)
#include <poseidon/mongodb/object_generator.hpp>

}

}

#endif
