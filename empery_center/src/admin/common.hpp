#ifndef EMPERY_CENTER_ADMIN_COMMON_HPP_
#define EMPERY_CENTER_ADMIN_COMMON_HPP_

#include <poseidon/cxx_ver.hpp>
#include <poseidon/cxx_util.hpp>
#include <poseidon/exception.hpp>
#include <poseidon/shared_nts.hpp>
#include <poseidon/module_raii.hpp>
#include <poseidon/http/exception.hpp>
#include <poseidon/http/status_codes.hpp>
#include <poseidon/job_base.hpp>
#include <poseidon/stream_buffer.hpp>
#include <poseidon/optional_map.hpp>
#include <poseidon/json.hpp>
#include "../singletons/account_map.hpp"
#include "../admin_session.hpp"
#include "../msg/err_account.hpp"
#include "../log.hpp"

/*
ADMIN_SERVLET(请求 URI, 会话形参名, GET 参数){
	// 处理逻辑
}
*/
#define ADMIN_SERVLET(uri_, sessionArg_, paramsArg_)	\
	namespace EmperyCenter {	\
		namespace {	\
			namespace Impl_ {	\
				::Poseidon::JsonObject TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (	\
					const ::boost::shared_ptr<AdminHttpSession> &, ::Poseidon::OptionalMap);	\
				::Poseidon::JsonObject  TOKEN_CAT3(AdminServlet, __LINE__, Entry_) (	\
					const ::boost::shared_ptr<AdminHttpSession> &session_, ::Poseidon::OptionalMap params_)	\
				{	\
					PROFILE_ME;	\
					LOG_EMPERY_CENTER_INFO("Admin servlet response: uri = ", uri_);	\
					return TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (session_, params_);	\
				}	\
			}	\
		}	\
		MODULE_RAII(handles_){	\
			handles_.push(AdminHttpSession::createServlet(uri_, & Impl_:: TOKEN_CAT3(AdminServlet, __LINE__, Entry_)));	\
		}	\
	}	\
	::Poseidon::JsonObject EmperyCenter::Impl_:: TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (	\
		const ::boost::shared_ptr<AdminHttpSession> & sessionArg_ __attribute__((__unused__)),	\
		::Poseidon::OptionalMap paramsArg_	\

#define ADMIN_THROW(code_)              DEBUG_THROW(::Poseidon::Http::Exception, (code_))

#endif
