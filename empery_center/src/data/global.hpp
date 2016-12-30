#ifndef EMPERY_CENTER_DATA_GLOBAL_HPP_
#define EMPERY_CENTER_DATA_GLOBAL_HPP_

#include "common.hpp"
#include <poseidon/fwd.hpp>

namespace EmperyCenter {
	namespace Data {
		class Global {
		public:
			enum Slot {
				SLOT_BUILDING_UPGRADE_CANCELLATION_REFUND_RATIO = 100001,
				SLOT_DEFAULT_MAIL_EXPIRY_DURATION = 100002,
				SLOT_NON_BEST_RESOURCE_PRODUCTION_RATE_MODIFIER = 100004,
				SLOT_NON_BEST_RESOURCE_CAPACITY_MODIFIER = 100005,
				SLOT_ACCELERATION_CARD_PRODUCTION_RATE_MODIFIER = 100006,
				SLOT_ACCELERATION_CARD_CAPACITY_MODIFIER = 100007,
				SLOT_MINIMUM_DISTANCE_BETWEEN_CASTLES = 100008,
				SLOT_CASTLE_IMMEDIATE_BUILDING_UPGRADE_TRADE_ID = 100011,
				SLOT_CASTLE_IMMEDIATE_TECH_UPGRADE_TRADE_ID = 100012,
				SLOT_IMMIGRANT_CREATION_TRADE_ID = 100013,
				SLOT_MAX_MESSAGES_IN_ADJACENT_CHANNEL = 100015,
				SLOT_MAX_MESSAGES_IN_ALLIANCE_CHANNEL = 100016,
				SLOT_MAX_MESSAGES_IN_SYSTEM_CHANNEL = 100017,
				SLOT_MAX_MESSAGES_IN_TRADE_CHANNEL = 100018,
				SLOT_MIN_MESSAGE_INTERVAL_IN_ADJACENT_CHANNEL = 100020,
				SLOT_MIN_MESSAGE_INTERVAL_IN_TRADE_CHANNEL = 100021,
				SLOT_MIN_MESSAGE_INTERVAL_IN_ALLIANCE_CHANNEL = 100022,
				SLOT_MAX_CONCURRENT_UPGRADING_BUILDING_COUNT = 100023,
				SLOT_MAX_CONCURRENT_UPGRADING_TECH_COUNT = 100024,
				SLOT_MAP_BORDER_THICKNESS = 100025,
				SLOT_MAP_SWITCH_MAX_BYPASSABLE_BLOCKS = 100026,
				SLOT_ACCELERATION_CARD_UNIT_PRICE = 100027,
				SLOT_AUCTION_TRANSFER_FEE_RATIO = 100028,
				SLOT_AUCTION_TAX_RATIO = 100029,
				SLOT_AUCTION_EARNEST_PAYMENT = 100030,
				SLOT_AUCTION_TRANSFER_DURATION = 100031,
				SLOT_ADJACENT_CHAT_RANGE = 100035,
				SLOT_TAX_RECORD_EXPIRY_DAYS = 100036,
				SLOT_BATTALION_PRODUCTION_CANCELLATION_REFUND_RATIO = 100037,
				SLOT_MAX_DAILY_TASK_COUNT = 100038,
				SLOT_CASTLE_IMMEDIATE_BATTALION_PRODUCTION_TRADE_ID = 100039,
				SLOT_WAR_STATE_PERSISTENCE_DURATION = 100041,
				SLOT_ACTIVE_CASTLE_THRESHOLD_DAYS = 100042,
				SLOT_MIN_ACTIVE_CASTLES_BY_MAP_EVENT_CIRCLE = 100043,
				SLOT_MAX_ACTIVE_CASTLES_BY_MAP_EVENT_CIRCLE = 100044,
				SLOT_INIT_ACTIVE_CASTLES_BY_MAP_EVENT_CIRCLE = 100045,
				SLOT_MAX_BATTLE_RECORD_COUNT = 100047,
				SLOT_BATTLE_RECORD_EXPIRY_DAYS = 100048,
				SLOT_AVATAR_MODIFICATION_TRADE_ID = 100052,
				SLOT_NICK_MODIFICATION_TRADE_ID = 100053,
				SLOT_CASTLE_NAME_MODIFICATION_TRADE_ID = 100054,
				SLOT_SIGNATURE_MODIFICATION_TRADE_ID = 100055,
				SLOT_MAX_TAX_RECORD_COUNT = 100058,
				SLOT_TECH_UPGRADE_CANCELLATION_REFUND_RATIO = 100060,
				SLOT_WOUNDED_SOLDIER_RATIO_BASIC_VALUE = 100061,
				SLOT_CASTLE_IMMEDIATE_TREATMENT_TRADE_ID = 100062,
				SLOT_RESOURCE_CRATE_AMOUNT_INNER_RATIO = 100063,
				SLOT_RESOURCE_CRATE_SEPARATION_AMOUNT_THRESHOLD = 100064,
				SLOT_RESOURCE_CRATE_NUMBER_LIMITS = 100065,
				SLOT_RESOURCE_CRATE_RADIUS_LIMITS = 100066,
				SLOT_RESOURCE_CRATE_RADIUS_EXPIRY_DURATION = 100067,
				SLOT_CASTLE_HANG_UP_INACTIVE_MINUTES = 100069,
				SLOT_CASTLE_PROTECTION_PREPARATION_DURATION = 100072,
				SLOT_CASTLE_PROTECTION_REFUND_RATIO = 100073,
				SLOT_MAP_CELL_OCCUPATION_DURATION = 100074,
				SLOT_MAP_CELL_RESCUE_DURATION = 100075,
				SLOT_MAP_CELL_PROTECTION_DURATION = 100076,
				SLOT_CASTLE_SIEGE_PROTECTION_DURATION = 100077,
				SLOT_BATTLE_BUNKER_GARRISON_PREPARATION_DURATION = 100078,
				SLOT_NOVICIATE_PROTECTION_DURATION = 100080,
				SLOT_NOVICIATE_PROTECTION_CASTLE_LEVEL_THRESHOLD = 100081,
				SLOT_GOBLIN_DROP_AWARD_HP_PERCENT = 100090,
				SLOT_GOBLIN_DROP_AWARD = 100091,
				SLOT_MAX_NUMBER_OF_FRIENDS = 100095,
				SLOT_MAX_NUMBER_OF_FRIENDS_REQUESTING = 100096,
				SLOT_MAX_NUMBER_OF_FRIENDS_REQUESTED = 100097,
				SLOT_WORLD_ACTIVITY_RANK_THRESHOLD = 100100,

				SLOT_LEGION_LEAVE_WAIT_MINUTE = 100105,
				SLOT_LEGION_ENABLE_APPLY_NUMBER = 100106,
				SLOT_LEGION_APPLY_COLD_MINUTE = 100107,
				SLOT_LEGION_INVITE_COLD_MINUTE = 100108,
				SLOT_LEGION_WEEK_DONATE_DIAMOND_LIMIT = 100109,
				SLOT_LEGION_MAINCITY_LOARD_LEVEL_LIMIT = 100110,
				SLOT_LEGION_LANGUAGE_KIND = 100111,
				SLOT_LEGION_DONATE_SCALE = 100112,
				SLOT_LEGION_PACKAGE_TASK_UNIT_LIMIT = 100113,
				SLOT_LEGION_PACKAGE_TASK_REFRESH_TIME = 100114,
				SLOT_LEGION_PACKAGE_SHARE_EXPIRE_MINUTE = 100115,
				SLOT_LEGION_STORE_UPDATE_MINUTE = 100116,
				SLOT_LEGION_MEMBER_DEFAULT_POWERID = 100117,
				SLOT_LEGION_RESTORE_DEPOT_BLOOD = 100118,
				SLOT_LEGION_ATTORN_LEADER_WAITTIME = 100119,
				SLOT_LEGION_KICK_OUTTIME = 100120,
				SLOT_CHAT_KINGCHANNLE_MONEY = 100121,
				SLOT_MIN_MESSAGE_INTERVAL_IN_KING_CHANNEL = 100122,
				SLOT_CHAT_RATE = 100123,
				SLOT_LEGION_WEEK_DONATE_UPDATETIME = 100124,
				SLOT_LEGION_WEEK_DONATE_DAY = 100125,
				SLOT_MAX_MESSAGES_IN_KING_CHANNEL = 100126,
				SLOT_LEGION_TASK_DAY_REFRESH = 100127,
				SLOT_LEGION_TASK_WEEK_REFRESH = 100128,
				SLOT_LEGION_WAREHOUSE_DEBUFF1 = 100129,
				SLOT_LEGION_WAREHOUSE_DEBUFF2 = 100130,
				SLOT_LEGION_WAREHOUSE_DEBUFF3 = 100131,
				SLOT_LEGAUE_APPLYJOIN_MAX                              = 100132,
				SLOT_LEGAUE_APPLYJOIN_EFFECT_MINUTE                    = 100133,
				SLOT_LEGAUE_INVITEJOIN_EFFECT_MINUTE                   = 100134,
				SLOT_LEGAUE_CREATE_NEED                                = 100135,
				SLOT_LEGAUE_CREATE_DEFAULT_MEMBERCOUNT                 = 100136,
				SLOT_LEGAUE_EXPAND_CONSUME                 			   = 100137,
				SLOT_LEGION_TASK_PERSONAL_CONTRIBUTE_THRESHOLD = 100140,
				SLOT_LEGION_MODIFY_NAME = 100141,
				SLOT_LEGION_MODIFY_ICON = 100142,
				SLOT_LEGION_BUILDING_HARVEST_COEFFICIENT = 100143,
				SLOT_LEGION_BUILDING_IMPAIRMENT_RATIO    = 100144,
				SLOT_DAILY_TASK_MAX_LEVEL                              = 100145,
				SLOT_MAX_FRIEND_RECORD_COUNT                           = 100146,
				SLOT_FRIEND_RECORD_EXPIRY_DAYS                         = 100147,
				SLOT_FRIEND_RECENT_EXPIRY_DAYS                         = 100148,
	                        SLOT_ITEM_DUNGEON_TRAD_PARAM                           = 100149,
				SLOT_BUILDING_FREE_UPGRADE_LIMIT                       = 100151,
				SLOT_ATTACK_RESOURCE_CREATE_COEFFICIENT                = 100152,
			};

		public:
			static const std::string &as_string(Slot slot);
			static std::int64_t as_signed(Slot slot);
			static std::uint64_t as_unsigned(Slot slot);
			static double as_double(Slot slot);
			static const Poseidon::JsonArray &as_array(Slot slot);
			static const Poseidon::JsonObject &as_object(Slot slot);
		};
	}
}

#endif
