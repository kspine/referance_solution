#include "precompiled.hpp"
#include "admin_http_session.hpp"
#include <poseidon/http/verbs.hpp>
#include <poseidon/http/status_codes.hpp>
#include <poseidon/http/exception.hpp>
#include <poseidon/http/utilities.hpp>
#include <poseidon/job_base.hpp>

namespace EmperyPromotion {

namespace {
	typedef AdminHttpSession::ServletCallback ServletCallback;

	std::map<std::string, boost::weak_ptr<const ServletCallback> > g_servletMap;
}

AdminHttpSession::AdminHttpSession(Poseidon::UniqueFile socket,
	boost::shared_ptr<const Poseidon::Http::AuthInfo> authInfo, std::string prefix)
	: Poseidon::Http::Session(std::move(socket))
	, m_authInfo(std::move(authInfo)), m_prefix(std::move(prefix))
{
}
AdminHttpSession::~AdminHttpSession(){
}

boost::shared_ptr<const ServletCallback> AdminHttpSession::createServlet(const std::string &uri, ServletCallback callback){
	PROFILE_ME;

	auto servlet = boost::make_shared<ServletCallback>(std::move(callback));
	if(!g_servletMap.insert(std::make_pair(uri, servlet)).second){
		LOG_EMPERY_PROMOTION_ERROR("Duplicate admin HTTP servlet: uri = ", uri);
		DEBUG_THROW(Exception, sslit("Duplicate admin HTTP servlet"));
	}
	return std::move(servlet);
}
boost::shared_ptr<const ServletCallback> AdminHttpSession::getServlet(const std::string &uri){
	PROFILE_ME;

	const auto it = g_servletMap.find(uri);
	if(it == g_servletMap.end()){
		return { };
	}
	return it->second.lock();
}

boost::shared_ptr<Poseidon::Http::UpgradedSessionBase> AdminHttpSession::predispatchRequest(
	Poseidon::Http::RequestHeaders &requestHeaders, Poseidon::StreamBuffer &entity)
{
	checkAndThrowIfUnauthorized(m_authInfo, getRemoteInfo(), requestHeaders);

	return Poseidon::Http::Session::predispatchRequest(requestHeaders, entity);
}

void AdminHttpSession::onSyncRequest(const Poseidon::Http::RequestHeaders &requestHeaders, const Poseidon::StreamBuffer & /* entity */){
	PROFILE_ME;
	LOG_EMPERY_PROMOTION(Poseidon::Logger::SP_MAJOR | Poseidon::Logger::LV_INFO,
		"Accepted admin HTTP request from ", getRemoteInfo());

	auto uri = Poseidon::Http::urlDecode(requestHeaders.uri);
	if((uri.size() < m_prefix.size()) || (uri.compare(0, m_prefix.size(), m_prefix) != 0)){
		LOG_EMPERY_PROMOTION_WARNING("Inacceptable admin HTTP request: uri = ", uri, ", prefix = ", m_prefix);
		DEBUG_THROW(Poseidon::Http::Exception, Poseidon::Http::ST_NOT_FOUND);
	}
	uri.erase(0, m_prefix.size());

	if(requestHeaders.verb != Poseidon::Http::V_GET){
		DEBUG_THROW(Poseidon::Http::Exception, Poseidon::Http::ST_METHOD_NOT_ALLOWED);
	}

	const auto servlet = getServlet(uri);
	if(!servlet){
		LOG_EMPERY_PROMOTION_WARNING("No servlet available: uri = ", uri);
		DEBUG_THROW(Poseidon::Http::Exception, Poseidon::Http::ST_NOT_FOUND);
	}

	try {
		(*servlet)(virtualSharedFromThis<AdminHttpSession>(), requestHeaders.getParams);
	} catch(Poseidon::Http::Exception &){
		throw;
	} catch(std::logic_error &e){
		LOG_EMPERY_PROMOTION_WARNING("std::logic_error thrown: what = ", e.what());
		DEBUG_THROW(Poseidon::Http::Exception, Poseidon::Http::ST_BAD_REQUEST);
	} catch(std::exception &e){
		LOG_EMPERY_PROMOTION_WARNING("std::exception thrown: what = ", e.what());
		DEBUG_THROW(Poseidon::Http::Exception, Poseidon::Http::ST_INTERNAL_SERVER_ERROR);
	}
}

}
