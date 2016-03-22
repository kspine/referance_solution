#include "precompiled.hpp"
#include "controller_session.hpp"
#include "mmain.hpp"
#include <boost/container/flat_map.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/job_promise.hpp>
#include <poseidon/atomic.hpp>
#include <poseidon/cbpp/control_message.hpp>
#include "../../empery_center/src/msg/g_packed.hpp"

namespace EmperyController {

namespace Msg {
	using namespace ::EmperyCenter::Msg;
}

using Result          = ControllerSession::Result;
using ServletCallback = ControllerSession::ServletCallback;

namespace {
	boost::container::flat_map<unsigned, boost::weak_ptr<const ServletCallback>> g_servlet_map;
}

boost::shared_ptr<const ServletCallback> ControllerSession::create_servlet(std::uint16_t message_id, ServletCallback callback){
	PROFILE_ME;

	auto &weak = g_servlet_map[message_id];
	if(!weak.expired()){
		LOG_EMPERY_CONTROLLER_ERROR("Duplicate controller servlet: message_id = ", message_id);
		DEBUG_THROW(Exception, sslit("Duplicate controller servlet"));
	}
	auto servlet = boost::make_shared<ServletCallback>(std::move(callback));
	weak = servlet;
	return std::move(servlet);
}
boost::shared_ptr<const ServletCallback> ControllerSession::get_servlet(std::uint16_t message_id){
	PROFILE_ME;

	const auto it = g_servlet_map.find(message_id);
	if(it == g_servlet_map.end()){
		return { };
	}
	auto servlet = it->second.lock();
	if(!servlet){
		g_servlet_map.erase(it);
		return { };
	}
	return servlet;
}

ControllerSession::ControllerSession(Poseidon::UniqueFile socket)
	: Poseidon::Cbpp::Session(std::move(socket), 0x1000000) // 16MiB
	, m_serial(0)
{
	LOG_EMPERY_CONTROLLER_INFO("Controller session constructor: this = ", (void *)this);
}
ControllerSession::~ControllerSession(){
	LOG_EMPERY_CONTROLLER_INFO("Controller session destructor: this = ", (void *)this);
}

void ControllerSession::on_connect(){
	PROFILE_ME;
	LOG_EMPERY_CONTROLLER_INFO("Controller session connected: remote = ", get_remote_info());

	const auto initial_timeout = get_config<std::uint64_t>("controller_session_initial_timeout", 30000);
	set_timeout(initial_timeout);

	Poseidon::Cbpp::Session::on_connect();
}
void ControllerSession::on_close(int err_code) noexcept {
	PROFILE_ME;
	LOG_EMPERY_CONTROLLER_INFO("Controller session closed: err_code = ", err_code);

	{
		const Poseidon::Mutex::UniqueLock lock(m_request_mutex);

		const auto requests = std::move(m_requests);
		m_requests.clear();
		for(auto it = requests.begin(); it != requests.end(); ++it){
			const auto &promise = it->second.promise;
			if(!promise || promise->is_satisfied()){
				continue;
			}
			try {
				try {
					DEBUG_THROW(Exception, sslit("Lost connection to controller server"));
				} catch(Poseidon::Exception &e){
					promise->set_exception(boost::copy_exception(e));
				} catch(std::exception &e){
					promise->set_exception(boost::copy_exception(e));
				}
			} catch(std::exception &e){
				LOG_EMPERY_CONTROLLER_ERROR("std::exception thrown: what = ", e.what());
			}
		}
	}

	Poseidon::Cbpp::Session::on_close(err_code);
}

void ControllerSession::on_sync_data_message(std::uint16_t message_id, Poseidon::StreamBuffer payload){
	PROFILE_ME;
	LOG_EMPERY_CONTROLLER_TRACE("Received data message from controller server: remote = ", get_remote_info(),
		", message_id = ", message_id, ", size = ", payload.size());

	if(message_id == Msg::G_PackedRequest::ID){
		Msg::G_PackedRequest packed(std::move(payload));
		Result result;
		try {
			const auto servlet = get_servlet(packed.message_id);
			if(!servlet){
				LOG_EMPERY_CONTROLLER_WARNING("No servlet found: message_id = ", packed.message_id);
				DEBUG_THROW(Poseidon::Cbpp::Exception, Poseidon::Cbpp::ST_NOT_FOUND, sslit("Unknown packed request"));
			}
			result = (*servlet)(virtual_shared_from_this<ControllerSession>(), Poseidon::StreamBuffer(packed.payload));
		} catch(Poseidon::Cbpp::Exception &e){
			LOG_EMPERY_CONTROLLER(Poseidon::Logger::SP_MAJOR | Poseidon::Logger::LV_INFO,
				"Poseidon::Cbpp::Exception thrown: message_id = ", message_id, ", what = ", e.what());
			result.first = e.status_code();
			result.second = e.what();
		} catch(std::exception &e){
			LOG_EMPERY_CONTROLLER(Poseidon::Logger::SP_MAJOR | Poseidon::Logger::LV_INFO,
				"std::exception thrown: message_id = ", message_id, ", what = ", e.what());
			result.first = Poseidon::Cbpp::ST_INTERNAL_ERROR;
			result.second = e.what();
		}
		if(result.first != 0){
			LOG_EMPERY_CONTROLLER_DEBUG("Sending response to controller server: message_id = ", message_id,
				", code = ", result.first, ", message = ", result.second);
		}
		Poseidon::Cbpp::Session::send(Msg::G_PackedResponse(packed.serial, result.first, std::move(result.second)));
		if(result.first < 0){
			shutdown_read();
			shutdown_write();
		}
	} else if(message_id == Msg::G_PackedResponse::ID){
		Msg::G_PackedResponse packed(std::move(payload));
		LOG_EMPERY_CONTROLLER_TRACE("Received response from controller server: code = ", packed.code, ", message = ", packed.message);
		{
			const Poseidon::Mutex::UniqueLock lock(m_request_mutex);
			const auto it = m_requests.find(packed.serial);
			if(it != m_requests.end()){
				const auto elem = std::move(it->second);
				m_requests.erase(it);

				if(elem.result){
					*elem.result = std::make_pair(packed.code, std::move(packed.message));
				}
				if(elem.promise){
					elem.promise->set_success();
				}
			}
		}
	} else {
		LOG_EMPERY_CONTROLLER_WARNING("Unknown message from controller server: remote = ", get_remote_info(), ", message_id = ", message_id);
		DEBUG_THROW(Poseidon::Cbpp::Exception, Poseidon::Cbpp::ST_NOT_FOUND, sslit("Unknown message"));
	}
}

bool ControllerSession::send(std::uint16_t message_id, Poseidon::StreamBuffer body){
	PROFILE_ME;

	const auto serial = Poseidon::atomic_add(m_serial, 1, Poseidon::ATOMIC_RELAXED);
	return Poseidon::Cbpp::Session::send(Msg::G_PackedRequest(serial, message_id, body.dump()));
}
void ControllerSession::shutdown(const char *message) noexcept {
	PROFILE_ME;

	shutdown(Poseidon::Cbpp::ST_INTERNAL_ERROR, message);
}
void ControllerSession::shutdown(int code, const char *message) noexcept {
	PROFILE_ME;

	if(!message){
		message = "";
	}
	try {
		Poseidon::Cbpp::Session::send_error(Poseidon::Cbpp::ControlMessage::ID, code, message);
		shutdown_read();
		shutdown_write();
	} catch(std::exception &e){
		LOG_EMPERY_CONTROLLER_ERROR("std::exception thrown: what = ", e.what());
		force_shutdown();
	}
}

Result ControllerSession::send_and_wait(std::uint16_t message_id, Poseidon::StreamBuffer body){
	PROFILE_ME;

	Result ret;

	const auto serial = Poseidon::atomic_add(m_serial, 1, Poseidon::ATOMIC_RELAXED);
	const auto promise = boost::make_shared<Poseidon::JobPromise>();
	{
		const Poseidon::Mutex::UniqueLock lock(m_request_mutex);
		m_requests.emplace(serial, RequestElement(&ret, promise));
	}
	try {
		if(!Poseidon::Cbpp::Session::send(Msg::G_PackedRequest(serial, message_id, body.dump()))){
			DEBUG_THROW(Exception, sslit("Could not send data to controller server"));
		}
		Poseidon::JobDispatcher::yield(promise, true);
	} catch(...){
		const Poseidon::Mutex::UniqueLock lock(m_request_mutex);
		m_requests.erase(serial);
		throw;
	}
	{
		const Poseidon::Mutex::UniqueLock lock(m_request_mutex);
		m_requests.erase(serial);
	}

	return ret;
}

}