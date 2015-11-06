#include "../precompiled.hpp"
#include "common.hpp"
#include "../singletons/account_map.hpp"
#include "../msg/err_account.hpp"

namespace EmperyPromotion {

ACCOUNT_SERVLET("setPassword", session, params){
	const auto &loginName = params.at("loginName");
	const auto &password = params.at("password");
	auto newPassword = params.at("newPassword");

	Poseidon::JsonObject ret;
	auto info = AccountMap::getByLoginName(loginName);
	if(Poseidon::hasNoneFlagsOf(info.flags, AccountMap::FL_VALID)){
		ret[sslit("errorCode")] = (int)Msg::ERR_NO_SUCH_ACCOUNT;
		ret[sslit("errorMessage")] = "Account is not found";
		return ret;
	}
	if(AccountMap::getPasswordHash(password) != info.passwordHash){
		ret[sslit("errorCode")] = (int)Msg::ERR_INVALID_PASSWORD;
		ret[sslit("errorMessage")] = "Password is incorrect";
		return ret;
	}
	if((info.bannedUntil != 0) && (Poseidon::getUtcTime() < info.bannedUntil)){
		ret[sslit("errorCode")] = (int)Msg::ERR_ACCOUNT_BANNED;
		ret[sslit("errorMessage")] = "Referrer is banned";
		return ret;
	}

	AccountMap::setPassword(info.accountId, std::move(newPassword));

	ret[sslit("errorCode")] = (int)Msg::ST_OK;
	ret[sslit("errorMessage")] = "No error";
	return ret;
}

}
