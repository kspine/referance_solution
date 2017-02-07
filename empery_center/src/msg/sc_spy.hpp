#ifndef EMPERY_CENTER_MSG_SC_SPY_HPP_
#define EMPERY_CENTER_MSG_SC_SPY_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter{
	namespace Msg{


#define MESSAGE_NAME   SC_SpyReportHeadRes
#define MESSAGE_ID      2350
#define MESSAGE_FIELDS  \
	FIELD_ARRAY (attributes,\
	     FIELD_STRING (spy_uuid) \
	     )
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    SC_SpyStartSync
#define MESSAGE_ID      2351
#define MESSAGE_FIELDS  \
    FIELD_STRING    (spy_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    SC_SpyReportDetailRes
#define MESSAGE_ID      2352
#define MESSAGE_FIELDS  \
	FIELD_STRING (spy_uuid)	\
	FIELD_STRING (account_uuid)	\
        FIELD_STRING (map_object_uuid)	\
        FIELD_VINT   (x) \
	    FIELD_VINT   (y) \
        FIELD_STRING (spy_account_uuid)	\
        FIELD_STRING (spy_map_object_uuid)	\
        FIELD_VINT   (spy_x)	\
    	FIELD_VINT   (spy_y)     \
        FIELD_VINT   (spy_start_time)	\
        FIELD_VINT   (spy_time)	\
        FIELD_VUINT  (spy_mark)	\
        FIELD_VINT   (hp)	\
        FIELD_VINT   (attack)	\
        FIELD_VINT   (defense)	\
        FIELD_VINT   (prosperity_degree) \
        FIELD_VINT   (map_cell_resource_id)	\
        FIELD_VINT   (map_cell_resource_amount)\
        FIELD_VUINT  (spy_status)\
        FIELD_VUINT  (read_status)\
        FIELD_VUINT  (create_time) \
        FIELD_VUINT  (action) \
        FIELD_VUINT  (spy_action) \
        FIELD_STRING (castle_resource) \
        FIELD_STRING (arm_attributes) \
        FIELD_STRING (build_attributes) \
        FIELD_STRING (captain_attributes) \
        FIELD_VUINT  (deleted)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    SC_SpyFinishSync
#define MESSAGE_ID      2353
#define MESSAGE_FIELDS  \
    FIELD_STRING    (spy_uuid)
#include <poseidon/cbpp/message_generator.hpp>
	}
}
#endif//
