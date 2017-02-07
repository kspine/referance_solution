#ifndef EMPERY_CENTER_MSG_CS_SPY_HPP_
#define EMPERY_CENTER_MSG_CS_SPY_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter{
namespace Msg{

#define MESSAGE_NAME    CS_SpyReportHeadReq
#define MESSAGE_ID      2300
#define MESSAGE_FIELDS  
#include <poseidon/cbpp/message_generator.hpp>


#define MESSAGE_NAME    CS_SpyReportDetailReq
#define MESSAGE_ID      2301
#define MESSAGE_FIELDS  \
 FIELD_STRING    (spy_uuid)	
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SpyReportHeadRead
#define MESSAGE_ID      2302
#define MESSAGE_FIELDS  \
 FIELD_STRING    (spy_uuid)	
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SpyReportHeadDelete
#define MESSAGE_ID      2303
#define MESSAGE_FIELDS  \
 FIELD_STRING    (spy_uuid)	
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SpyReportHeadDeleteAll
#define MESSAGE_ID      2304
#define MESSAGE_FIELDS  \
 FIELD_STRING    (spy_uuid)	
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SpyCastle
#define MESSAGE_ID      2305
#define MESSAGE_FIELDS  \
    FIELD_STRING    (owner_map_object_uuid)	\
    FIELD_STRING    (spy_account_uuid)	\
    FIELD_STRING    (spy_map_object_uuid)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    CS_SpyMapCell
#define MESSAGE_ID      2306
#define MESSAGE_FIELDS  \
    FIELD_STRING    (owner_map_object_uuid)	\
    FIELD_STRING    (spy_account_uuid)	\
    FIELD_STRING    (spy_map_object_uuid) \
    FIELD_VINT     (x)	\
    FIELD_VINT     (y)	
#include <poseidon/cbpp/message_generator.hpp>
}
}
#endif//

