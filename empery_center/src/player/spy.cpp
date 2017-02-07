#include "../precompiled.hpp"
#include "common.hpp"

#include "../msg/cs_spy.hpp"
#include "../msg/sc_spy.hpp"
#include "../msg/err_spy.hpp"
#include "../msg/err_map.hpp"
#include "../msg/err_castle.hpp"

#include "../map_object.hpp"
#include "../map_object_type_ids.hpp"
#include "../map_utilities.hpp"

#include "../buff_ids.hpp"
#include "../resource_ids.hpp"
#include "../attribute_ids.hpp"
#include "../account_attribute_ids.hpp"

#include "../singletons/world_map.hpp"

#include "../singletons/account_map.hpp"

#include "../singletons/spy_report_map.hpp"

#include "../player_session.hpp"

#include "../castle.hpp"

#include "../map_cell.hpp"

#include "../data/spy.hpp"

#include "../mongodb/spy.hpp"

namespace EmperyCenter{

PLAYER_SERVLET(Msg:: CS_SpyReportHeadReq,account, session, req)
{
PROFILE_ME;
  const auto account_uuid    = account->get_account_uuid();

  {
    std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> spy_ret;
    SpyReportMap::get_spy_report_by_spy_account_uuid(account_uuid,spy_ret);

       Msg::SC_SpyReportHeadRes msg;
       msg.attributes.reserve(spy_ret.size());
       for (auto it = spy_ret.begin(); it != spy_ret.end(); ++it)
       {
        auto &elem = *msg.attributes.emplace(msg.attributes.end());
        elem.spy_uuid = (*it)->unlocked_get_spy_uuid().to_string();
       }

       session->send(msg);
   }

   {
       std::vector<boost::shared_ptr<MongoDb::Center_SpyReport>> ret;
       SpyReportMap::get_spy_report_by_account_uuid(account_uuid,ret);

       Msg::SC_SpyReportHeadRes msg;
       msg.attributes.reserve(ret.size());
       for (auto it = ret.begin(); it != ret.end(); ++it)
       {
        auto &elem = *msg.attributes.emplace(msg.attributes.end());
        elem.spy_uuid = (*it)->unlocked_get_spy_uuid().to_string();
       }

       session->send(msg);
   }
   return Response(Msg::ST_OK);
}
PLAYER_SERVLET(Msg:: CS_SpyReportDetailReq,account, session, req)
{
  PROFILE_ME;
   auto spy_uuid    = SpyUuid(req.spy_uuid);
   LOG_EMPERY_CENTER_FATAL("Msg:: CS_SpyReportDetailReq spy_uuid = ",spy_uuid);
   auto spys = SpyReportMap::get_spy_report(spy_uuid);
   if(spys)
   {
        Msg::SC_SpyReportDetailRes msg;
        msg.spy_uuid = spys->unlocked_get_spy_uuid().to_string();
    	msg.account_uuid = spys->unlocked_get_account_uuid().to_string();
        msg.map_object_uuid = spys->unlocked_get_map_object_uuid().to_string();
        msg.x = spys->unlocked_get_x();
    	msg.y = spys->unlocked_get_y();
        msg.spy_account_uuid = spys->unlocked_get_spy_account_uuid().to_string();
        msg.spy_map_object_uuid = spys->unlocked_get_spy_map_object_uuid().to_string();
        msg.spy_x = spys->unlocked_get_spy_x();
    	msg.spy_y = spys->unlocked_get_spy_y();
        msg.spy_start_time = spys->unlocked_get_spy_start_time();
        msg.spy_time	= spys->unlocked_get_spy_time();
        msg.spy_mark	= spys->unlocked_get_spy_mark();
        msg.hp	= spys->unlocked_get_hp();
        msg.attack	= spys->unlocked_get_attack();
        msg.defense	= spys->unlocked_get_defense();
        msg.prosperity_degree= spys->unlocked_get_prosperity_degree();
        msg.map_cell_resource_id= spys->unlocked_get_map_cell_resource_id();
        msg.map_cell_resource_amount= spys->unlocked_get_map_cell_resource_amount();
        msg.spy_status= spys->unlocked_get_spy_status();
        msg.read_status= spys->unlocked_get_read_status();
        msg.create_time= spys->unlocked_get_create_time();
        msg.action = spys->unlocked_get_action();
        msg.spy_action= spys->unlocked_get_spy_action();
        msg.castle_resource = spys->unlocked_get_castle_resource();
        msg.arm_attributes = spys->unlocked_get_arm_attributes();
        msg.build_attributes = spys->unlocked_get_build_attributes();
        msg.captain_attributes = spys->unlocked_get_captain_attributes();
        msg.deleted = spys->unlocked_get_deleted();
        session->send(msg);
  }
    return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg:: CS_SpyReportHeadRead,account, session, req)
{
PROFILE_ME;
  const auto spy_uuid    = SpyUuid(req.spy_uuid);
  const auto spys = SpyReportMap::get_spy_report(spy_uuid);
  if(spys)
  {
   std::uint64_t read_status = spys->unlocked_get_read_status();

   if(account->get_account_uuid() == (AccountUuid)spys->unlocked_get_spy_account_uuid())
   {
    if(read_status == 0)
    {
      read_status = 1;
    }
    else if(read_status == 2)
    {
      read_status = 3;
    }
   }
   else if(account->get_account_uuid() == (AccountUuid)spys->unlocked_get_account_uuid())
   {
      if(read_status == 0)
      {
         read_status = 2;
      }
      else if(read_status == 1)
      {
        read_status = 3;
      }
   }
   SpyReportMap::make_read(spy_uuid,read_status);

   return Response(Msg::ST_OK);
  }
    return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg:: CS_SpyReportHeadDelete,account, session, req)
{
   PROFILE_ME;
  const auto spy_uuid    = SpyUuid(req.spy_uuid);
  const auto spys = SpyReportMap::get_spy_report(spy_uuid);
  if(spys)
  {
    std::uint64_t deleted = spys->unlocked_get_deleted();
    if(account->get_account_uuid() == (AccountUuid)spys->unlocked_get_spy_account_uuid())
    {
        if(deleted == 0)
        {
           deleted  = 1;
        }
        else if(deleted == 2)
        {
           deleted  = 3;
        }
    }
    else if(account->get_account_uuid() == (AccountUuid)spys->unlocked_get_account_uuid())
    {
        if(deleted == 0)
        {
           deleted = 2;
        }
        else if(deleted == 1)
        {
           deleted = 3;
        }
    }

   SpyReportMap::make_delete(spy_uuid,deleted);
   return Response(Msg::ST_OK);
  }
    return Response(Msg::ST_OK);
}

PLAYER_SERVLET(Msg:: CS_SpyReportHeadDeleteAll,account, session, req)
{
  PROFILE_ME;
  const auto account_uuid    = account->get_account_uuid();;

  SpyReportMap::make_delete_all(account_uuid);
  return Response(Msg::ST_OK);
}


	PLAYER_SERVLET(Msg::CS_SpyCastle,account, session, req)
	{
		PROFILE_ME;

         auto accounts    = AccountMap::require(account->get_account_uuid());
		 const auto &spy_status  = accounts->get_attribute(AccountAttributeIds::ID_SPY_STATUS);
		 if(!spy_status.empty())
		   if(spy_status == "1")
		     return Response(Msg::ERR_SPY_QUEUE_FULL);

        const auto owner_account_uuid = account->get_account_uuid();
		const auto owner_map_object_uuid = MapObjectUuid(req.owner_map_object_uuid);

           const auto owner_map_object = WorldMap::get_map_object(owner_map_object_uuid);
		if(!owner_map_object)
		  return Response(Msg::ERR_NO_SUCH_MAP_OBJECT);

		if(owner_map_object->get_owner_uuid() != account->get_account_uuid())
		  return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT);

		 auto owner_castle = boost::dynamic_pointer_cast<Castle>(owner_map_object);
		if(!owner_castle)
			return Response(Msg::ERR_NO_SUCH_CASTLE);

		if(owner_castle->get_owner_uuid() != account->get_account_uuid())
			return Response(Msg::ERR_NOT_CASTLE_OWNER);

         const auto spy_account_uuid    = AccountUuid(req.spy_account_uuid);

		const auto spy_map_object_uuid = MapObjectUuid(req.spy_map_object_uuid);

         auto spy_map_object = WorldMap::get_map_object(spy_map_object_uuid);
		if(!spy_map_object)
		    return Response(Msg::ERR_NO_SUCH_MAP_OBJECT);

		if(spy_map_object->get_owner_uuid() != spy_account_uuid)
		    return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT);

		const auto spy_castle = boost::dynamic_pointer_cast<Castle>(spy_map_object);	
		if(!spy_castle)
			return Response(Msg::ERR_NO_SUCH_CASTLE);

		if(spy_castle->get_owner_uuid() != spy_account_uuid)
		  return Response(Msg::ERR_NOT_CASTLE_OWNER);

        if(owner_castle->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION)) 
                {
                           const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                          return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }
		if(owner_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION)) 
                {
                           const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                          return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }
		if(owner_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION_PREPARATION)) 
		{
                           const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                          return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }

		if(spy_castle->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION)){
                  const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                  auto create_time = Poseidon::get_utc_time();
                  SpyReportMap::make_insert_uncondition(spy_uuid,spy_account_uuid,spy_map_object_uuid,spy_castle->get_coord().x(),spy_castle->get_coord().y(),owner_account_uuid,owner_map_object_uuid,owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),0,0,1,0,2,0,create_time);

             return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
        }
		if(spy_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION)){
                  const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                  auto create_time = Poseidon::get_utc_time();
                  SpyReportMap::make_insert_uncondition(spy_uuid,spy_account_uuid,spy_map_object_uuid,spy_castle->get_coord().x(),spy_castle->get_coord().y(),owner_account_uuid,owner_map_object_uuid,owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),0,0,1,0,2,0,create_time);

             return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
        }
		if(spy_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION_PREPARATION)){
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                          auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				spy_castle->get_coord().x(),
				spy_castle->get_coord().y(),
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                2,
                                0,
                                create_time);

                        return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
                }

		const auto distance = get_distance_of_coords(owner_castle->get_coord(),spy_castle->get_coord());
		const auto spy_time = (std::int64_t)Data::SpyData::get_spy_time(distance);
                const auto spy_start_time = (std::int64_t)Poseidon::get_utc_time();

                const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());

                auto create_time = Poseidon::get_utc_time();
                SpyReportMap::make_insert_condition(spy_uuid,
                spy_account_uuid,
				spy_map_object_uuid,
				spy_castle->get_coord().x(),
				spy_castle->get_coord().y(),
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				spy_start_time,
				spy_time,
				1,
                1,create_time);

        Msg::SC_SpyStartSync msg;
		msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
		session->send(msg);


		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "1";

		   accounts->set_attributes(std::move(modifiers));

                return Response(Msg::ST_OK);
	}

	PLAYER_SERVLET(Msg::CS_SpyMapCell,account, session, req)
	{
		PROFILE_ME;

         auto accounts    = AccountMap::require(account->get_account_uuid());
		 const auto &spy_status  = accounts->get_attribute(AccountAttributeIds::ID_SPY_STATUS);
		 if(!spy_status.empty())
		   if(spy_status == "1")
		    return Response(Msg::ERR_SPY_QUEUE_FULL);

        const auto owner_account_uuid = account->get_account_uuid();
  
		const auto owner_map_object_uuid = MapObjectUuid(req.owner_map_object_uuid);
	
                const auto owner_map_object = WorldMap::get_map_object(owner_map_object_uuid);
		if(!owner_map_object)
		  return Response(Msg::ERR_NO_SUCH_MAP_OBJECT);

		if(owner_map_object->get_owner_uuid() != account->get_account_uuid())
		  return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT);
		
		const auto owner_castle = boost::dynamic_pointer_cast<Castle>(owner_map_object);
		if(!owner_castle)
			return Response(Msg::ERR_NO_SUCH_CASTLE);

		if(owner_castle->get_owner_uuid() != account->get_account_uuid())
			return Response(Msg::ERR_NOT_CASTLE_OWNER);

                const auto spy_account_uuid    = AccountUuid(req.spy_account_uuid);
		
		const auto spy_map_object_uuid = MapObjectUuid(req.spy_map_object_uuid);
		
                const auto spy_map_object = WorldMap::get_map_object(spy_map_object_uuid);
		if(!spy_map_object)
		    return Response(Msg::ERR_NO_SUCH_MAP_OBJECT);

		if(spy_map_object->get_owner_uuid() != spy_account_uuid)
		    return Response(Msg::ERR_NOT_YOUR_MAP_OBJECT);
		  
		const auto spy_castle = boost::dynamic_pointer_cast<Castle>(spy_map_object);	
		if(!spy_castle)
			return Response(Msg::ERR_NO_SUCH_CASTLE);
		
		if(spy_castle->get_owner_uuid() != spy_account_uuid)
		  return Response(Msg::ERR_NOT_CASTLE_OWNER);

	if(owner_castle->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION)) 
                {
                           const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                          return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }
		if(owner_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION)) 
                {
                           const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                          return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }
		  
		
		if(owner_castle->is_buff_in_effect(BuffIds::ID_CASTLE_PROTECTION_PREPARATION)) 
                {
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                           auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				0,
				0,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                0,
                                2,
                                create_time);

                         return Response(Msg::ERR_SPY_PROTECTION_PERIOD_OWNER);
                }

		 auto spy_coord        = Coord(req.x, req.y);

		auto spy_map_cell     = WorldMap::get_map_cell(spy_coord);

		if(!spy_map_cell || spy_map_cell->is_virtually_removed())
        {
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                          auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				req.x,
				req.y,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                0,
                1,
                0,
                create_time);

             return Response(Msg::ERR_NO_TICKET_ON_MAP_CELL);
        }

		if(spy_map_cell->is_buff_in_effect(BuffIds::ID_OCCUPATION_MAP_CELL)) 
                {
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                          auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				req.x,
				req.y,
				spy_account_uuid,
				spy_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                                0,
                                2,
                                0,
                                create_time);

                         return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
                }


		if(spy_map_cell->is_buff_in_effect(BuffIds::ID_OCCUPATION_PROTECTION))
        {
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                          auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				req.x,
				req.y,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                0,
                2,
                0,
                create_time);

              return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
        }
        if(spy_map_cell->is_buff_in_effect(BuffIds::ID_NOVICIATE_PROTECTION))
        {
                          const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());
                          auto create_time = Poseidon::get_utc_time();
                           SpyReportMap::make_insert_uncondition(
                                spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				req.x,
				req.y,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				0,
				0,
				1,
                0,
                2,
                0,
                create_time);

              return Response(Msg::ERR_SPY_PROTECTIOM_PERIOD_OTHER);
        }

		const auto distance = get_distance_of_coords(owner_castle->get_coord(),spy_coord);
		const auto spy_time = (std::int64_t)Data::SpyData::get_spy_time(distance);

		const auto spy_start_time = (std::int64_t)Poseidon::get_utc_time();

                const auto spy_uuid = SpyUuid(Poseidon::Uuid::random());

                auto create_time = Poseidon::get_utc_time();
                SpyReportMap::make_insert_condition(spy_uuid,
                                spy_account_uuid,
				spy_map_object_uuid,
				req.x,
				req.y,
				owner_account_uuid,
				owner_map_object_uuid,
				owner_castle->get_coord().x(),
				owner_castle->get_coord().y(),
				spy_start_time,
				spy_time,
				2,
                1,
                create_time);

        Msg::SC_SpyStartSync msg;
		msg.spy_uuid = boost::lexical_cast<std::string>(spy_uuid);
		session->send(msg);

		   boost::container::flat_map<AccountAttributeId, std::string> modifiers;
		   modifiers.reserve(1);
		   modifiers[AccountAttributeIds::ID_SPY_STATUS]  = "1";

	       accounts->set_attributes(std::move(modifiers));


                return Response(Msg::ST_OK);

	}
}
