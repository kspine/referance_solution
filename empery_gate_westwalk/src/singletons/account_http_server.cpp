#include "../precompiled.hpp"
#include <poseidon/tcp_server_base.hpp>
#include <poseidon/http/authorization.hpp>
#include <poseidon/singletons/epoll_daemon.hpp>
#include "../account_http_session.hpp"

namespace EmperyGateWestwalk {

namespace {
	class AccountHttpServer : public Poseidon::TcpServerBase {
	private:
		const boost::shared_ptr<const Poseidon::Http::AuthInfo> m_authInfo;
		const std::string m_path;

	public:
		AccountHttpServer(const Poseidon::IpPort &bindAddr, const std::string &cert, const std::string &privateKey,
			std::vector<std::string> authInfo, std::string path)
			: Poseidon::TcpServerBase(bindAddr, cert.c_str(), privateKey.c_str())
			, m_authInfo(authInfo.empty() ? boost::shared_ptr<Poseidon::Http::AuthInfo>()
				: Poseidon::Http::createAuthInfo(std::move(authInfo))), m_path(std::move(path))
		{
		}

	protected:
		boost::shared_ptr<Poseidon::TcpSessionBase> onClientConnect(Poseidon::UniqueFile client) const override {
			return boost::make_shared<AccountHttpSession>(std::move(client), m_authInfo, m_path + '/');
		}
	};
}

MODULE_RAII_PRIORITY(handles, 9000){
	auto bind = getConfig<std::string> ("account_http_server_bind", "0.0.0.0");
	auto port = getConfig<unsigned>    ("account_http_server_port", 13206);
	auto cert = getConfig<std::string> ("account_http_server_certificate");
	auto pkey = getConfig<std::string> ("account_http_server_private_key");
	auto auth = getConfigV<std::string>("account_http_server_auth_user_pass");
	auto path = getConfig<std::string> ("account_http_server_path", "/empery_gate_westwalk/account");

	const Poseidon::IpPort bindAddr(SharedNts(bind), port);
	LOG_EMPERY_GATE_WESTWALK_INFO("Creating account HTTP server on ", bindAddr, path);
	const auto server = boost::make_shared<AccountHttpServer>(bindAddr, cert, pkey, std::move(auth), std::move(path));
	Poseidon::EpollDaemon::registerServer(server);
	handles.push(server);
}

}