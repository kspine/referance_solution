#ifndef EMPERY_CENTER_MSG_ERR_CAPTAIN_HPP_
#define EMPERY_CENTER_MSG_ERR_CAPTAIN_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter {

namespace Msg {
	using namespace Poseidon::Cbpp::StatusCodes;

	enum {
		ERR_CAPTAIN_CANNOTFIND_BASEDATA                			= 75000,	  	// 找不到基础数据
		ERR_CAPTAIN_NOT_YOURS				       = 75001,	  	// 不属于自己的将领
		ERR_CAPTAIN_IN_USE										= 75002,	  	// 已经出征
		ERR_CAPTAIN_CUR_NOT_IN_USE								= 75003,	  	// 当前没有将领
		ERR_CAPTAIN_CAN_NOT_FIND_ITEM							= 75004,	  	// 当前找到没有道具
		ERR_CAPTAIN_ITEM_ISNOT_YOURS							= 75005,	  	// 不属于自己的道具
		ERR_CAPTAIN_ITEM_IN_USE									= 75006,	  	// 已经在使用
		ERR_CAPTAIN_ITEM_NOT_IN_USE								= 75007,	  	// 没有在使用
		ERR_CAPTAIN_ITEM_LEVEL_LIMIT							= 75008,	  	// 穿戴等级不足，请提升等级
		ERR_CAPTAIN_ITEM_LEVEL_NOT_ENOUGH						= 75009,	  	// 道具不足
		ERR_CAPTAIN_FULL_LEVEL									= 75010,	  	// 已经满级
		ERR_CAPTAIN_FULL_STAR_LEVEL								= 75011,	  	// 星级已经满级
		ERR_CAPTAIN_FULL_QUALITY_LEVEL							= 75012,	  	// 品质已经满级
	};
}

}

#endif
