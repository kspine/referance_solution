#ifndef EMPERY_CENTER_ADMIN_COMMON_HPP_
#define EMPERY_CENTER_ADMIN_COMMON_HPP_

#include <poseidon/cxx_ver.hpp>
#include <poseidon/cxx_util.hpp>
#include <poseidon/exception.hpp>
#include <poseidon/shared_nts.hpp>
#include <poseidon/module_raii.hpp>
#include <poseidon/http/exception.hpp>
#include <poseidon/http/status_codes.hpp>
#include <poseidon/stream_buffer.hpp>
#include <poseidon/optional_map.hpp>
#include <poseidon/json.hpp>
#include "../admin_session.hpp"
#include "../log.hpp"
#include "../cbpp_response.hpp"

/*
ADMIN_SERVLET(请求 URI, 返回 JSON 形参名, 会话形参名, GET 参数){
	// 处理逻辑
	return Response(code) <<msg;
}
*/

#define ADMIN_SERVLET(uri_, root_arg_, session_arg_, params_arg_)	\
	namespace {	\
		namespace Impl_ {	\
			::std::pair<long, ::std::string> TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (	\
				::Poseidon::JsonObject &, const ::boost::shared_ptr< ::EmperyCenter::AdminSession> &, ::Poseidon::OptionalMap);	\
			::std::pair<long, ::std::string> TOKEN_CAT3(AdminServlet, __LINE__, Entry_) (	\
				::Poseidon::JsonObject &root_, const ::boost::shared_ptr< ::EmperyCenter::AdminSession> &session_,	\
				::Poseidon::OptionalMap params_)	\
			{	\
				PROFILE_ME;	\
				LOG_EMPERY_CENTER_INFO("Admin servlet request: uri = ", uri_);	\
				return TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (root_, session_, ::std::move(params_));	\
			}	\
		}	\
	}	\
	MODULE_RAII(handles_){	\
		handles_.push(::EmperyCenter::AdminSession::create_servlet(uri_, & Impl_:: TOKEN_CAT3(AdminServlet, __LINE__, Entry_)));	\
	}	\
	::std::pair<long, ::std::string> Impl_:: TOKEN_CAT3(AdminServlet, __LINE__, Proc_) (	\
		::Poseidon::JsonObject & root_arg_ __attribute__((__unused__)),	\
		const ::boost::shared_ptr< ::EmperyCenter::AdminSession> & session_arg_ __attribute__((__unused__)),	\
		::Poseidon::OptionalMap params_arg_	\
		)

#define ADMIN_THROW(code_)              DEBUG_THROW(::Poseidon::Http::Exception, (code_))

namespace EmperyCenter {

using Response = CbppResponse;

}

#endif
