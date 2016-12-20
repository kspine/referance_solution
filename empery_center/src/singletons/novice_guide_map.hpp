#ifndef EMPERY_CENTER_SINGLETONS_NOVICE_GUIDE_MAP_HPP_
#define EMPERY_CENTER_SINGLETONS_NOVICE_GUIDE_MAP_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "../id_types.hpp"

namespace EmperyCenter
{
	namespace MongoDb
	{
	  class Center_NoviceGuide;
	}

    class PlayerSession;

	struct NoviceGuideMap
	{
	   static void insert(const boost::shared_ptr<MongoDb::Center_NoviceGuide> &novice_guide_);
	   static boost::shared_ptr<MongoDb::Center_NoviceGuide> find(AccountUuid account_uuid,TaskId task_id);
	   static std::uint64_t get_step_id(AccountUuid account_uuid,TaskId task_id);
	   public:
	   		enum EIndex
	   		{
	   		  EIndex_account_uuid,
	          EIndex_task_id,
	        };
	   private:
	        NoviceGuideMap() = delete;
     };
}

#endif//