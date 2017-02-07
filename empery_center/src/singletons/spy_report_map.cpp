#include "../precompiled.hpp"
#include "spy_report_map.hpp"

#include "../mmain.hpp"
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/mongodb_daemon.hpp>
#include <poseidon/singletons/timer_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>

#include "player_session_map.hpp"

#include <tuple>

#include "../msg/sc_spy.hpp"

#include "../player_session.hpp"
#include "../account.hpp"
#include "../string_utilities.hpp"

#include "../mongodb/spy.hpp"
#include "account_map.hpp"
#include "../account_attribute_ids.hpp"

#include "world_map.hpp"

#include "../map_object.hpp"
#include "../map_object_type_ids.hpp"
#include "../map_utilities.hpp"

#include "../buff_ids.hpp"
#include "../resource_ids.hpp"
#include "../attribute_ids.hpp"
#include "../account_attribute_ids.hpp"


#include "../castle.hpp"

#include "../map_cell.hpp"


namespace EmperyCenter{
namespace{

struct SpyReportElement{
boost::shared_ptr<MongoDb::Center_SpyReport> spys;
                SpyUuid     spy_uuid;
                AccountUuid account_uuid;
				MapObjectUuid map_object_uuid;
				std::int64_t x;
				std::int64_t y;
				AccountUuid spy_account_uuid;
				MapObjectUuid spy_map_object_uuid;
				std::int64_t spy_x;
				std::int64_t spy_y;
				std::int64_t spy_start_time;
				std::int64_t spy_time;
				std::uint64_t spy_mark;
				std::int64_t hp;
				std::int64_t attack;
				std::int64_t defense;
                std::int64_t prosperity_degree;
				std::int64_t map_cell_resource_id;
				std::int64_t map_cell_resource_amount;
                std::uint64_t spy_status;
				std::uint64_t read_status;
                std::uint64_t create_time;
                std::uint64_t deleted;
                std::uint64_t action;
                std::uint64_t spy_action;
                std::string castle_resource;
                std::string arm_attributes;
                std::string build_attributes;
                std::string captain_attributes;

explicit SpyReportElement(boost::shared_ptr<MongoDb::Center_SpyReport> spys_)
:spys(std::move(spys_))
,spy_uuid(SpyUuid(spys->get_spy_uuid()))
,account_uuid((AccountUuid)spys->get_account_uuid())
,map_object_uuid((MapObjectUuid)spys->get_map_object_uuid())
,x(spys->get_x())
,y(spys->get_y())
,spy_account_uuid((AccountUuid)spys->get_spy_account_uuid())
,spy_map_object_uuid((MapObjectUuid)spys->get_spy_map_object_uuid())
,spy_x(spys->get_spy_x())
,spy_y(spys->get_spy_y())
,spy_start_time(spys->get_spy_start_time())
,spy_time(spys->get_spy_time())
,spy_mark(spys->get_spy_mark())
,hp(spys->get_hp())
,attack(spys->get_attack())
,defense(spys->get_defense())
,prosperity_degree(spys->get_prosperity_degree())
,map_cell_resource_id(spys->get_map_cell_resource_id())
,map_cell_resource_amount(spys->get_map_cell_resource_amount())
,spy_status(spys->get_spy_status())
,read_status(spys->get_read_status())
,create_time(spys->get_create_time())
,deleted(spys->get_deleted())
,action(spys->get_action())
,spy_action(spys->get_spy_action())
,castle_resource(spys->get_castle_resource())
,arm_attributes(spys->get_arm_attributes())
,build_attributes(spys->get_build_attributes())
,captain_attributes(spys->get_captain_attributes()){}
};

MULTI_INDEX_MAP(SpyReportContainer,SpyReportElement,
UNIQUE_MEMBER_INDEX(spy_uuid)
MULTI_MEMBER_INDEX(spy_account_uuid)
MULTI_MEMBER_INDEX(account_uuid)
)

boost::shared_ptr<SpyReportContainer> g_spy_report_map;

void gc_spy_proc(std::uint64_t now)
{
	PROFILE_ME; 
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }

   SpyReportMap::check_spy_process();
}

void gc_spy_report_recyle_proc(std::uint64_t now)
{
   PROFILE_ME; 
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }

   SpyReportMap::spy_report_recycle();
}

MODULE_RAII_PRIORITY(handles,5000)
{
   const auto conn = Poseidon::MongoDbDaemon::create_connection();
   struct TempSpyReportElement
   {
      boost::shared_ptr<MongoDb::Center_SpyReport> obj;
   };
   std::map<SpyUuid,TempSpyReportElement> temp_map;

	LOG_EMPERY_CENTER_INFO("Loading `Center_SpyReport`...");
  
	conn->execute_query("Center_SpyReport", { }, 0, INT32_MAX);

   while(conn->fetch_next())
   {
      auto obj = boost::make_shared<MongoDb::Center_SpyReport>();
      obj->fetch(conn);
      obj->enable_auto_saving();
      const auto spy_uuid = SpyUuid(obj->get_spy_uuid());
      temp_map[spy_uuid].obj = std::move(obj);
   }

   LOG_EMPERY_CENTER_INFO("Loaded ", temp_map.size(), " SpyReport(s).");
   const auto spy_report_map = boost::make_shared<SpyReportContainer>();
   for (const auto &p : temp_map)
   {
     spy_report_map->insert(SpyReportElement(std::move(p.second.obj)));
   }
   g_spy_report_map = spy_report_map;
   handles.push(spy_report_map);

auto timer1 = Poseidon::TimerDaemon::register_timer(0, 1*1000,
			std::bind(&gc_spy_proc, std::placeholders::_2));
		handles.push(timer1);

auto timer2 = Poseidon::TimerDaemon::register_timer(0, 300000,
			std::bind(&gc_spy_report_recyle_proc, std::placeholders::_2));
		handles.push(timer2);
}
}


void SpyReportMap::spy_report_recycle()
{
   PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }

 for (auto it = spy_report_map->begin(); it != spy_report_map->end();)
 {
       auto &spysinfo = (it->spys);
       auto spy_deleted = spysinfo->get_deleted();
       if(spy_deleted == 3)
       {
           spy_delete(SpyUuid(spysinfo->get_spy_uuid()));
           spy_report_map->erase(it++);
       }
       else
       {
           ++it;
       }
  }
}

 void SpyReportMap::insert(const boost::shared_ptr<MongoDb::Center_SpyReport> &spys)
 {
   PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }
   if(!spy_report_map->insert(SpyReportElement(spys)).second){
     return;
   }
 }



void SpyReportMap::check_spy_process()
{
    PROFILE_ME;
    const auto &spy_report_map = g_spy_report_map;
    if(!spy_report_map){
      return;
    }

   for (auto it = spy_report_map->begin(); it != spy_report_map->end(); ++it)
   {
        auto &spysinfo = (it->spys);
        auto spy_uuid = (SpyUuid)spysinfo->get_spy_uuid();
        auto spy_status = spysinfo->get_spy_status();
        if(spy_status == 1)
        {
            auto spy_mark = spysinfo->get_spy_mark();
            auto spy_time = spysinfo->get_spy_time();
            auto spy_start_time = spysinfo->get_spy_start_time();
            auto current_time = Poseidon::get_utc_time();
            if((std::int64_t)current_time > ((std::int64_t)spy_time*1000) + spy_start_time)
            {
                LOG_EMPERY_CENTER_FATAL("check_spy_process  spy_uuid = ",spy_uuid);

                if(spy_mark == 1)
                {
                    spy_castle(spy_uuid);

                }
                else if(spy_mark == 2)
                {
                   spy_mapcell(spy_uuid);

                }
            }
        }
   }
}

void SpyReportMap::spy_castle(SpyUuid  spy_uuid)
{
   PROFILE_ME;
   const auto spys = get_spy_report(spy_uuid);
   if(!spys)
     return;

   const auto spy_account_uuid    = AccountUuid(spys->get_account_uuid());

   const auto spy_map_object_uuid = MapObjectUuid(spys->get_map_object_uuid());

    const auto spy_map_object = WorldMap::get_map_object(spy_map_object_uuid);
    if(!spy_map_object)
    {
      const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	  if(spy_session)
      {
	      Msg::SC_SpyFinishSync msg;
          msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
          spy_session->send(msg);
	  }
       auto create_time = Poseidon::get_utc_time();
       make_update_status(spy_uuid,3,1,create_time);
       		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
       return;
    }


     const auto spy_castles = boost::dynamic_pointer_cast<Castle>(spy_map_object);
     if(!spy_castles)
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,1,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }

     if(spy_castles->get_owner_uuid() != spy_account_uuid)
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,1,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }

     auto coord        = Coord(spys->get_x(), spys->get_y());
     if(spy_castles->get_coord() != coord)
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,1,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }
     if(spy_castles->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION))
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,2,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }

     if(spy_castles->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION))
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,2,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }

     if(spy_castles->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION_PREPARATION))
     {
       const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,2,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
     }




double total_attack  = spy_map_object->get_attribute(EmperyCenter::AttributeIds::ID_ATTACK_BONUS) + spy_map_object->get_attribute(EmperyCenter::AttributeIds::ID_ATTACK_ADD);

double total_defense  = spy_map_object->get_attribute(EmperyCenter::AttributeIds::ID_DEFENSE_BONUS) + spy_map_object->get_attribute(EmperyCenter::AttributeIds::ID_DEFENSE_ADD);


 std::int64_t hp = (std::int64_t)spy_map_object->get_attribute(AttributeIds::ID_HP_TOTAL);
 std::int64_t attack = (std::int64_t)total_attack;
 std::int64_t defense = (std::int64_t)total_defense;
 std::int64_t prosperity_degree = (std::int64_t)spy_map_object->get_attribute(AttributeIds::ID_PROSPERITY_POINTS);


  std::string castle_resource("");
  std::vector<Castle::ResourceInfo> resource_info;
  spy_castles->get_resources_all(resource_info);

  for(auto it = resource_info.begin(); it != resource_info.end(); ++it)
  {
        castle_resource +=  boost::lexical_cast<std::string>(it->resource_id);
        castle_resource += ",";
        castle_resource +=  boost::lexical_cast<std::string>(it->amount);
        castle_resource += "|";
  }

   std::string arm_attributes("");
   std::vector<Castle::SoldierInfo> soldier_info;
   spy_castles->get_soldiers_all(soldier_info);

   for(auto it = soldier_info.begin(); it != soldier_info.end(); ++it)
   {
        arm_attributes +=  boost::lexical_cast<std::string>(it->map_object_type_id);
        arm_attributes += ",";
        arm_attributes +=  boost::lexical_cast<std::string>(it->count);
        arm_attributes += "|";
   }

   std::vector<boost::shared_ptr<MapObject>> spy_map_object_info;
   WorldMap::get_map_objects_by_owner(spy_map_object_info, spy_account_uuid);
   for (auto it = spy_map_object_info.begin(); it != spy_map_object_info.end(); ++it)
   {
     const auto &map_object = *it;
     if (map_object->get_map_object_type_id() == MapObjectTypeIds::ID_CASTLE) 
     {
       continue;
     }
     if (map_object->get_map_object_type_id() == MapObjectTypeIds::ID_DEFENSE_TOWER) 
     {
       continue;
     }
     if (map_object->get_map_object_type_id() == MapObjectTypeIds::ID_BATTLE_BUNKER) 
     {
       continue;
     }
     if (map_object->get_map_object_type_id() == MapObjectTypeIds::ID_LEGION_WAREHOUSE) 
     {
       continue;
     }
     if(!map_object->is_garrisoned())
     {
       continue;
     }
      arm_attributes +=  boost::lexical_cast<std::string>(map_object->get_map_object_type_id());
      arm_attributes += ",";
      arm_attributes +=  boost::lexical_cast<std::string>(map_object->get_attribute(AttributeIds::ID_SOLDIER_COUNT));
        arm_attributes += "|";
   }

   std::string  build_attributes("");
   std::vector<Castle::BuildingBaseInfo> building_base_info;
   spy_castles->get_building_bases_all(building_base_info);

   for(auto it = building_base_info.begin(); it != building_base_info.end(); ++it)
   {
        build_attributes +=  boost::lexical_cast<std::string>(it->building_base_id);
        build_attributes += ",";
        build_attributes +=  boost::lexical_cast<std::string>(it->building_id);
        build_attributes += ",";
        build_attributes +=  boost::lexical_cast<std::string>(it->building_level);
        build_attributes += "|";
   }

  std::string captain_attributes("");
   auto create_time = Poseidon::get_utc_time();
   make_update_detail((SpyUuid)spy_uuid,hp,attack,defense,prosperity_degree,0,0,2,create_time,castle_resource,arm_attributes,build_attributes,captain_attributes);



		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";

		   accounts->set_attributes(std::move(modifiers));

        const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	    const auto session = PlayerSessionMap::get(AccountUuid(spys->get_account_uuid()));
	    if(session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             session->send(msg);
	    }
}

void SpyReportMap::spy_mapcell(SpyUuid  spy_uuid)
{
   PROFILE_ME;
   const auto spys = get_spy_report(spy_uuid);
   if(!spys)
     return;


  const auto spy_map_object_uuid = MapObjectUuid(spys->get_map_object_uuid());

  const auto spy_map_object = WorldMap::get_map_object(spy_map_object_uuid);
  if(!spy_map_object)
  {
    const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
        auto create_time = Poseidon::get_utc_time();
        make_update_status(spy_uuid,3,1,create_time);
        		   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
        return;
  }


   const auto spy_castle = boost::dynamic_pointer_cast<Castle>(spy_map_object);
   if(!spy_castle)
   {
     const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	auto create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,1,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
   }

	const auto spy_coord        = Coord(spys->get_x(), spys->get_y());

	const auto spy_map_cell     = WorldMap::get_map_cell(spy_coord);

   if(!spy_map_cell)
   {
     const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	std::uint64_t  create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,1,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
   }
    if(spy_map_cell->is_virtually_removed())
   {
     const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	std::uint64_t  create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,1,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
   }

  if(spy_map_cell->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION))
  {
    const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	std::uint64_t  create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,2,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
  }
  if(spy_map_cell->is_buff_in_effect(BuffIds::ID_OCCUPATION_MAP_CELL))
  {
    const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	std::uint64_t  create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,2,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
  }

  if(spy_map_cell->is_buff_in_effect(BuffIds::ID_OCCUPATION_PROTECTION))
  {
    const auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	std::uint64_t  create_time = Poseidon::get_utc_time();
	make_update_status(spy_uuid,3,2,create_time);
			   const  auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";
		   		   accounts->set_attributes(std::move(modifiers));
	return;
  }

  auto map_cell_resource_id      = (std::int64_t)spy_map_cell->get_production_resource_id().get();
  auto map_cell_resource_amount  = (std::int64_t)spy_map_cell->get_resource_amount();

  auto create_time = Poseidon::get_utc_time();
  make_update_detail(spy_uuid,
		0,
		0,
		0,
        0,
		map_cell_resource_id,
		map_cell_resource_amount,
                2,
                create_time,
                "",
                "",
                "",
                "");

		   auto accounts    = AccountMap::require(AccountUuid(spys->get_spy_account_uuid()));

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "0";

		   accounts->set_attributes(std::move(modifiers));

        auto spy_session = PlayerSessionMap::get(AccountUuid(spys->get_spy_account_uuid()));
	    if(spy_session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             spy_session->send(msg);
	    }
	    const auto session = PlayerSessionMap::get(AccountUuid(spys->get_account_uuid()));
	    if(session)
        {
	         Msg::SC_SpyFinishSync msg;
             msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
             session->send(msg);
	    }
}

void SpyReportMap::make_insert_uncondition(SpyUuid     spy_uuid,
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
                         )
{
    auto obj = boost::make_shared<MongoDb::Center_SpyReport>(
    spy_uuid.get(),
    account_uuid.get(),
    map_object_uuid.get(),
    x,
    y,
    spy_account_uuid.get(),
    spy_map_object_uuid.get(),
    spy_x,
    spy_y,
    spy_start_time,
    spy_time,
    spy_mark,
    0,
    0,
    0,
    0,
    0,
    0,
    spy_status,
    0,
    create_time,
    0,
    action,
    spy_action,
    "0",
    "0",
    "0",
    "0");

    obj->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(obj, true, true);

    insert(obj);
}

void SpyReportMap::make_insert_condition(
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
                std::uint64_t create_time)
{
    auto obj = boost::make_shared<MongoDb::Center_SpyReport>(spy_uuid.get(),account_uuid.get(),map_object_uuid.get(),x,y,spy_account_uuid.get(), 
    spy_map_object_uuid.get(),spy_x,spy_y,spy_start_time,spy_time, spy_mark,0,0,0,0,0,0,spy_status,0,create_time,0,0,0,"0", "0","0","0");

    obj->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(obj, true, true);

    insert(obj);
}

void SpyReportMap::make_update_detail(
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
                         )
{
   PROFILE_ME;


   const auto spys = get_spy_report(spy_uuid);
  if(!spys)
     return;

         spys->set_hp(hp);
         spys->set_attack(attack);
         spys->set_defense(defense);
         spys->set_prosperity_degree(prosperity_degree);
         spys->set_map_cell_resource_id(map_cell_resource_id);
         spys->set_map_cell_resource_amount(map_cell_resource_amount);
         spys->set_spy_status(spy_status);
         spys->set_create_time(create_time);
         spys->set_castle_resource(castle_resource);
         spys->set_arm_attributes(arm_attributes);
         spys->set_build_attributes(build_attributes);
         spys->set_captain_attributes(captain_attributes);

    spys->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(spys, true, true);

    insert(spys);
}

void SpyReportMap::make_update_status(SpyUuid spy_uuid,
                                std::uint64_t spy_status,
                                std::uint64_t action,
                                std::uint64_t create_time)
{
   PROFILE_ME;

   const auto spys = get_spy_report(spy_uuid);
   if(!spys)
     return;

   spys->set_spy_status(spy_status);
   spys->set_action(action);
   spys->set_create_time(create_time);

    spys->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(spys, true, true);

    insert(spys);
}


void SpyReportMap::make_read(SpyUuid spy_uuid,std::uint64_t read_status)
{
   PROFILE_ME;

   const auto spys = get_spy_report(spy_uuid);
   if(!spys)
     return;

    spys->set_read_status(read_status);

    spys->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(spys, true, true);

    insert(spys);
}

void SpyReportMap::make_delete(SpyUuid spy_uuid,uint64_t deleted)
{
   PROFILE_ME;

   const auto spys = get_spy_report(spy_uuid);
   if(!spys)
     return;

    spys->set_deleted(deleted);

    spys->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(spys, true, true);

    insert(spys);
}

void SpyReportMap::make_delete_all(AccountUuid spy_account_uuid)
{
  std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> spys;
  get_spy_report_by_spy_account_uuid(spy_account_uuid,spys);

  for (auto it = spys.begin(); it != spys.end(); ++it)
  {
    auto info = (*it);
    info->set_deleted(1);

    info->enable_auto_saving();

    Poseidon::MongoDbDaemon::enqueue_for_saving(info, true, true);

    insert(info);
  }
}

boost::shared_ptr<MongoDb::Center_SpyReport> SpyReportMap::get_spy_report(SpyUuid spy_uuid)
{
  PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return {};
   }

 LOG_EMPERY_CENTER_FATAL("get_spy_report spy_uuid = ",spy_uuid);
 
  const auto range = spy_report_map->equal_range<0>(spy_uuid);
  for (auto it = range.first; it != range.second; ++it)
  {
     if(SpyUuid(it->spys->unlocked_get_spy_uuid()) == spy_uuid)
     {
      LOG_EMPERY_CENTER_FATAL("get_spy_report Inner Find spy_uuid = ",spy_uuid);
       return it->spys;
     }
  }
  return {};
}

bool SpyReportMap::check_spy_report(SpyUuid  spy_uuid)
{
   PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return false;
   }

  const auto range = spy_report_map->equal_range<0>(spy_uuid);
  for (auto it = range.first; it != range.second; ++it)
  {
     if(SpyUuid(it->spys->unlocked_get_spy_uuid()) == spy_uuid)
          return true;
  }
  return false;
}

void  SpyReportMap::get_spy_report_by_spy_account_uuid(AccountUuid spy_account_uuid,std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> &ret)
{
   PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }
   const auto range = spy_report_map->equal_range<1>(spy_account_uuid);
   ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
   for (auto it = range.first; it != range.second; ++it)
   {
       if(AccountUuid(it->spys->unlocked_get_spy_account_uuid()) == spy_account_uuid)
         ret.emplace_back(it->spys);
   }
   return;
  }
void  SpyReportMap::get_spy_report_by_account_uuid(AccountUuid account_uuid,std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> &ret)
{
   PROFILE_ME;
   const auto &spy_report_map = g_spy_report_map;
   if(!spy_report_map){
     return;
   }
   const auto range = spy_report_map->equal_range<2>(account_uuid);
   ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
   for (auto it = range.first; it != range.second; ++it)
   {
       if(AccountUuid(it->spys->unlocked_get_account_uuid()) == account_uuid)
         ret.emplace_back(it->spys);
   }
   return;
  }
void SpyReportMap::spy_delete(SpyUuid spy_uuid)
{
   PROFILE_ME;
    Poseidon::MongoDb::BsonBuilder query;
		query.append_string(sslit("_id"), PRIMERY_KEYGEN::GenIDS::GenId(spy_uuid.get()));
		Poseidon::MongoDbDaemon::enqueue_for_deleting("Center_SpyReport", query, true);
    return;
}
}
