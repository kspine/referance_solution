#ifndef TEXAS_GATE_WESTWALK_SMS_HTTP_CLIENT_HPP_
#define TEXAS_GATE_WESTWALK_SMS_HTTP_CLIENT_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/sock_addr.hpp>
#include <poseidon/fwd.hpp>

namespace TexasGateWestwalk {

class SmsHttpClient : NONCOPYABLE, public boost::enable_shared_from_this<SmsHttpClient> {
private:
	class HttpConnector;

private:
	const std::string m_phone;
	const std::string m_password;

	boost::shared_ptr<Poseidon::TimerItem> m_timer;
	boost::weak_ptr<HttpConnector> m_connector;
	std::size_t m_retryCount;

public:
	SmsHttpClient(std::string phone, std::string password);
	~SmsHttpClient();

private:
	void timerProc() NOEXCEPT;

public:
	void commit();
};

}

#endif
