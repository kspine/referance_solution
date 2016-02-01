#ifndef EMPERY_CENTER_REASON_IDS_HPP_
#define EMPERY_CENTER_REASON_IDS_HPP_

#include "id_types.hpp"

namespace EmperyCenter {

namespace ReasonIds {

constexpr ReasonId
	                                             // param1              param2              param3
	ID_ADMIN_OPERATION               ( 671001 ), // 自定义              自定义              自定义
	ID_UPGRADE_BUILDING              ( 672001 ), // 建筑 ID             等级                0
	ID_CANCEL_UPGRADE_BUILDING       ( 672002 ), // 建筑 ID             等级                0
	ID_UPGRADE_TECH                  ( 672003 ), // 科技 ID             等级                0
	ID_CANCEL_UPGRADE_TECH           ( 672004 ), // 科技 ID             等级                0
	ID_HARVEST_MAP_CELL              ( 672005 ), // 世界坐标 X          世界坐标 Y          土地购买券 ID
	ID_HARVEST_OVERLAY               ( 672006 ), // 世界坐标 X          世界坐标 Y          覆盖图 id
	ID_SPEED_UP_BUILDING_UPGRADE     ( 672007 ), // 建筑 ID             等级                0
	ID_SPEED_UP_TECH_UPGRADE         ( 672008 ), // 科技 ID             等级                0
	ID_UNPACK_INTO_CASTLE            ( 672009 ), // 城堡 UUID 高位      道具 ID             重复次数
	ID_BUY_ACCELERATION_CARD         ( 672010 ), // 玩家等级            0                   0
	ID_POPULATION_PRODUCTION         ( 672011 ), // 时间区间（毫秒）    0                   0
	ID_ENABLE_BATTALION              ( 672012 ), // 部队 ID             0                   0
	ID_PRODUCE_BATTALION             ( 672013 ), // 部队 ID             数量                0
	ID_CANCEL_PRODUCE_BATTALION      ( 672014 ), // 部队 ID             数量                0
	ID_HARVEST_BATTALION             ( 672015 ), // 部队 ID             数量                0
	ID_SPEED_UP_BATTALION_PRODUCTION ( 672016 ), // 部队 ID             数量                0
	ID_DISMISS_BATTALION             ( 672017 ), // 部队 ID             数量                0

	ID_INIT_ITEMS                    ( 673001 ), // 初始数量            0                   0
	ID_AUTO_INCREMENT                ( 673002 ), // 自增长类型          自增长偏移          0
	ID_TRADE_REQUEST                 ( 673003 ), // 客户端请求 ID       交易 ID             重复次数
	ID_MAIL_ATTACHMENTS              ( 673004 ), // 邮件 UUID 高位      语言 ID             邮件类型
	ID_PAYMENT                       ( 673005 ), // 0                   0                   0
	ID_AUCTION_TRANSFER_LOCK         ( 673006 ), // 0                   0                   0
	ID_AUCTION_TRANSFER_COMMIT       ( 673007 ), // 0                   0                   0
	ID_AUCTION_TRANSFER_UNLOCK       ( 673008 ), // 0                   0                   0
	ID_AUCTION_CENTER_WITHDRAW       ( 673009 ), // 0                   0                   0
	ID_TASK_REWARD                   ( 673010 ), // 任务 ID             0                   0

	ID_MAP_CELL_PURCHASE             ( 674001 ), // 世界坐标 X          世界坐标 Y          0
	ID_MAP_CELL_UPGRADE              ( 674002 ), // 世界坐标 X          世界坐标 Y          0
	ID_MAP_CELL_RECYCLE              ( 674003 ), // 世界坐标 X          世界坐标 Y          0
	ID_APPLY_ACCELERATION_CARD       ( 674004 ), // 世界坐标 X          世界坐标 Y          土地购买券 ID

	ID_ENTER_CASTLE                  ( 675001 ), // 城堡 UUID 高位      部队 UUID 高位      0
	ID_LEAVE_CASTLE                  ( 675002 ); // 城堡 UUID 高位      部队 UUID 高位      0

}

}

#endif
