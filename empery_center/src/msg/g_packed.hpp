#ifndef EMPERY_CENTER_MSG_G_PACKED_REQUEST_HPP_
#define EMPERY_CENTER_MSG_G_PACKED_REQUEST_HPP_

#include <poseidon/cbpp/message_base.hpp>

namespace EmperyCenter {

namespace Msg {

// 中心到集群双向。
#define MESSAGE_NAME	G_PackedRequest
#define MESSAGE_ID		99
#define MESSAGE_FIELDS	\
	FIELD_VUINT			(serial)	\
	FIELD_VUINT			(messageId)	\
	FIELD_STRING		(payload)
#include <poseidon/cbpp/message_generator.hpp>

// 中心到集群双向。和 G_PackedRequest 构成“请求-应答”对。
#define MESSAGE_NAME	G_PackedResponse
#define MESSAGE_ID		98
#define MESSAGE_FIELDS	\
	FIELD_VUINT			(serial)	\
	FIELD_VINT			(code)	\
	FIELD_STRING		(message)
#include <poseidon/cbpp/message_generator.hpp>

// 集群到中心单向。只转发，不需要响应。
#define MESSAGE_NAME	G_PackedAccountNotification
#define MESSAGE_ID		97
#define MESSAGE_FIELDS	\
	FIELD_STRING		(accountUuid)	\
	FIELD_VUINT			(messageId)	\
	FIELD_STRING		(payload)
#include <poseidon/cbpp/message_generator.hpp>

}

}

#endif
