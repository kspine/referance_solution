#ifndef EMPERY_CENTER_DATA_SPY_IN_HPP_
#define EMPERY_CENTER_DATA_SPY_IN_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>

namespace EmperyCenter
{
	namespace Data
	{
	   class SpyData
	   {
		   public:
			   static boost::shared_ptr<const SpyData> get (SpyId spy_id);
	   
			   static boost::shared_ptr<const SpyData> require (SpyId spy_id);
		   public:
			   SpyId spy_id;

			   std::uint64_t spy_range_min;
			   std::uint64_t spy_range_max;
			   std::uint64_t spy_time;
			   std::uint64_t spy_front_time;
		   public:
			   static std::uint64_t get_spy_time(std::uint64_t spy_distance);
	   };
	}
}









































#endif//


