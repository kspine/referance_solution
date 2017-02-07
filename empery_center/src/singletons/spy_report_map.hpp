#ifndef EMPERY_CENTER_SPY_REPORT_MAP_HPP_
#define EMPERY_CENTER_SPY_REPORT_MAP_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <boost/shared_ptr.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

	namespace MongoDb
	{
	    class Center_SpyReport;
	}

	class PlayerSession;

	struct SpyReportMap
	{

             static void spy_report_recycle();

             static void check_spy_process();
             static void spy_castle(SpyUuid  spy_uuid);
             static void spy_mapcell(SpyUuid  spy_uuid);

             
             static void insert(const boost::shared_ptr<MongoDb::Center_SpyReport> &spys);

           
             static boost::shared_ptr<MongoDb::Center_SpyReport> get_spy_report(SpyUuid  spy_uuid);

             static bool check_spy_report(SpyUuid  spy_uuid);
             static bool check_spy_account(AccountUuid spy_account_uuid);

             static void make_insert_uncondition(
                                SpyUuid     spy_uuid,
                                AccountUuid account_uuid,
				MapObjectUuid map_object_uuid,
				std::int64_t x,
				std::int64_t y,
				AccountUuid spy_account_uuid,
				MapObjectUuid spy_map_object_uuid,
				std::int64_t spy_x,
				std::int64_t spy_y,
				std::int64_t spy_start_time,
				std::int64_t spy_time,
				std::uint64_t spy_mark,
                                std::uint64_t spy_status,
                                std::uint64_t action,
                                std::uint64_t spy_action,
                                std::uint64_t create_time
                         );

            static void make_insert_condition(
                                SpyUuid     spy_uuid,
                                AccountUuid account_uuid,
				MapObjectUuid map_object_uuid,
				std::int64_t x,
				std::int64_t y,
				AccountUuid spy_account_uuid,
				MapObjectUuid spy_map_object_uuid,
				std::int64_t spy_x,
				std::int64_t spy_y,
				std::int64_t spy_start_time,
				std::int64_t spy_time,
				std::uint64_t spy_mark,
                std::uint64_t spy_status,
                std::uint64_t create_time);

               static void make_update_detail(
                                SpyUuid     spy_uuid,
				std::int64_t hp,
				std::int64_t attack,
				std::int64_t defense,
                                std::int64_t prosperity_degree,
				std::int64_t map_cell_resource_id,
				std::int64_t map_cell_resource_amount,
                                std::uint64_t spy_status,
                                std::uint64_t create_time,
                                std::string castle_resource,
                                std::string arm_attributes,
                                std::string build_attributes,
                                std::string captain_attributes
                         );

              static void make_update_status(SpyUuid spy_uuid,
                                std::uint64_t spy_status,
                                std::uint64_t action,
                                std::uint64_t create_time);

             static void make_read(SpyUuid spy_uuid,std::uint64_t read_status);

             static void make_delete(SpyUuid spy_uuid,std::uint64_t deleted);

             static void make_delete_all(AccountUuid spy_account_uuid);

             static void  get_spy_report_by_spy_account_uuid(AccountUuid spy_account_uuid,std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> &ret);

             static void  get_spy_report_by_account_uuid(AccountUuid account_uuid,std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> &ret);

            static void spy_delete(SpyUuid spy_uuid);
	private:
		SpyReportMap() = delete;
	};
}

#endif
