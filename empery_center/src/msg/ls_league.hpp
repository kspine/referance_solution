#ifndef EMPERY_CENTER_MSG_LS_LEAGUE_HPP_
#define EMPERY_CENTER_MSG_LS_LEAGUE_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

#define MESSAGE_NAME    LS_LeagueRegisterServer
#define MESSAGE_ID      51000
#define MESSAGE_FIELDS  \
	//
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    LS_LeagueInfo
#define MESSAGE_ID      51001
#define MESSAGE_FIELDS  \
	FIELD_STRING        (account_uuid)	\
	FIELD_STRING        (league_uuid)	\
	FIELD_STRING        (league_name)	\
	FIELD_STRING        (league_icon)	\
	FIELD_STRING        (league_notice)	\
	FIELD_VUINT         (league_level)	\
	FIELD_VUINT         (legion_titleid)	\
	FIELD_STRING        (leader_legion_uuid)	\
	FIELD_STRING        (create_account_uuid)	\
	FIELD_ARRAY         (members,	\
		FIELD_STRING        (legion_uuid)	\
		FIELD_VUINT         (titleid)	\
		FIELD_VUINT         (speakflag)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    LS_Leagues
#define MESSAGE_ID      51002
#define MESSAGE_FIELDS  \
	FIELD_STRING        (account_uuid)	\
	FIELD_ARRAY         (leagues,	\
		FIELD_STRING        (league_uuid)	\
		FIELD_STRING        (league_name)	\
		FIELD_STRING        (league_icon)	\
		FIELD_STRING        (league_notice)	\
		FIELD_STRING        (league_leader_uuid)	\
		FIELD_STRING        (autojoin)	\
		FIELD_STRING        (league_create_time) \
		FIELD_STRING        (isapplyjoin) \
		FIELD_ARRAY         (legions,	\
			FIELD_STRING        (legion_uuid)	\
		)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    LS_CreateLeagueRes
#define MESSAGE_ID      51003
#define MESSAGE_FIELDS  \
	FIELD_STRING        (account_uuid)	\
	FIELD_VUINT         (res)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME    LS_ApplyJoinList
#define MESSAGE_ID      51004
#define MESSAGE_FIELDS  \
	FIELD_STRING        (account_uuid)	\
	FIELD_ARRAY         (applies,	\
		FIELD_STRING        (legion_uuid)	\
	)
#include <poseidon/cbpp/message_generator.hpp>


#define MESSAGE_NAME    LS_InvieJoinList
#define MESSAGE_ID      51005
#define MESSAGE_FIELDS  \
	FIELD_STRING        (account_uuid)	\
	FIELD_STRING        (legion_uuid)	\
	FIELD_ARRAY         (invites,	\
		FIELD_STRING        (league_uuid)	\
		FIELD_STRING        (league_name)	\
		FIELD_STRING        (league_icon)	\
		FIELD_STRING        (league_leader_uuid)	\
	)
#include <poseidon/cbpp/message_generator.hpp>

}

}

#endif
