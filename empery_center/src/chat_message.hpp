#ifndef EMPERY_CENTER_CHAT_MESSAGE_HPP_
#define EMPERY_CENTER_CHAT_MESSAGE_HPP_

#include <poseidon/virtual_shared_from_this.hpp>
#include <poseidon/cxx_util.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <string>
#include "id_types.hpp"

namespace EmperyCenter {

class PlayerSession;

class ChatMessage : public virtual Poseidon::VirtualSharedFromThis {
private:
	const ChatMessageUuid m_chat_message_uuid;

	const ChatChannelId m_channel;
	const ChatMessageTypeId m_type;
	const LanguageId m_language_id;
	const boost::uint64_t m_created_time;

	AccountUuid m_from_account_uuid;
	std::vector<std::pair<ChatMessageSlotId, std::string>> m_segments;

public:
	ChatMessage(ChatMessageUuid chat_message_uuid,
		ChatChannelId channel, ChatMessageTypeId type, LanguageId language_id, boost::uint64_t created_time,
		AccountUuid from_account_uuid, std::vector<std::pair<ChatMessageSlotId, std::string>> segments);
	~ChatMessage();

public:
	ChatMessageUuid get_chat_message_uuid() const {
		return m_chat_message_uuid;
	}

	ChatChannelId get_channel() const {
		return m_channel;
	}
	ChatMessageTypeId get_type() const {
		return m_type;
	}
	LanguageId get_language_id() const {
		return m_language_id;
	}
	boost::uint64_t get_created_time() const {
		return m_created_time;
	}

	AccountUuid get_from_account_uuid() const {
		return m_from_account_uuid;
	}
	const std::vector<std::pair<ChatMessageSlotId, std::string>> &get_segments() const {
		return m_segments;
	}

	void synchronize_with_player(const boost::shared_ptr<PlayerSession> &session) const;
};

inline void synchronize_chat_message_with_player(const boost::shared_ptr<const ChatMessage> &chat_message,
	const boost::shared_ptr<PlayerSession> &session)
{
	chat_message->synchronize_with_player(session);
}
inline void synchronize_chat_message_with_player(const boost::shared_ptr<ChatMessage> &chat_message,
	const boost::shared_ptr<PlayerSession> &session)
{
	chat_message->synchronize_with_player(session);
}

}

#endif