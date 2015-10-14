#include "../precompiled.hpp"
#include "account_map.hpp"
#include <string.h>
#include <poseidon/multi_index_map.hpp>
#include <poseidon/hash.hpp>
#include <poseidon/http/utilities.hpp>
#include <poseidon/singletons/mysql_daemon.hpp>
#include <poseidon/singletons/event_dispatcher.hpp>
#include "../mysql/account.hpp"

namespace EmperyGateWestwalk {

namespace {
	struct StrCaseComparator {
		bool operator()(const std::string &lhs, const std::string &rhs) const {
			return ::strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
		}
	};

	struct AccountElement {
		boost::shared_ptr<MySql::Westwalk_Account> obj;

		std::string accountName;

		explicit AccountElement(boost::shared_ptr<MySql::Westwalk_Account> obj_)
			: obj(std::move(obj_))
			, accountName(obj->unlockedGet_accountName())
		{
		}
	};

	MULTI_INDEX_MAP(AccountMapDelegator, AccountElement,
		UNIQUE_MEMBER_INDEX(accountName, StrCaseComparator)
	)

	boost::shared_ptr<AccountMapDelegator> g_accountMap;

	MODULE_RAII_PRIORITY(handles, 5000){
		const auto conn = Poseidon::MySqlDaemon::createConnection();

		const auto accountMap = boost::make_shared<AccountMapDelegator>();
		LOG_EMPERY_GATE_WESTWALK_INFO("Loading accounts...");
		conn->executeSql("SELECT * FROM `Westwalk_Account`");
		while(conn->fetchRow()){
			auto obj = boost::make_shared<MySql::Westwalk_Account>();
			obj->syncFetch(conn);
			obj->enableAutoSaving();
			accountMap->insert(AccountElement(std::move(obj)));
		}
		LOG_EMPERY_GATE_WESTWALK_INFO("Loaded ", accountMap->size(), " account(s).");
		g_accountMap = accountMap;
		handles.push(accountMap);
	}

	void fillAccountInfo(AccountMap::AccountInfo &info, const boost::shared_ptr<MySql::Westwalk_Account> &obj){
		PROFILE_ME;

		info.accountName                  = obj->unlockedGet_accountName();
		info.createdIp                    = obj->unlockedGet_createdIp();
		info.createdTime                  = obj->get_createdTime();
		info.remarks                      = obj->unlockedGet_remarks();
		info.passwordHash                 = obj->unlockedGet_passwordHash();
		info.disposablePasswordHash       = obj->unlockedGet_disposablePasswordHash();
		info.disposablePasswordExpiryTime = obj->get_disposablePasswordExpiryTime();
		info.passwordRegainCooldownTime   = obj->get_passwordRegainCooldownTime();
		info.token                        = obj->unlockedGet_token();
		info.tokenExpiryTime              = obj->get_tokenExpiryTime();
		info.lastLoginIp                  = obj->unlockedGet_lastLoginIp();
		info.lastLoginTime                = obj->get_lastLoginTime();
		info.bannedUntil                  = obj->get_bannedUntil();
		info.retryCount                   = obj->get_retryCount();
		info.flags                        = obj->get_flags();
	}
}

bool AccountMap::has(const std::string &accountName){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		return false;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		return false;
	}
	return true;
}
AccountMap::AccountInfo AccountMap::get(const std::string &accountName){
	PROFILE_ME;

	AccountInfo info = { };
	info.accountName = accountName;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		return info;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		return info;
	}
	fillAccountInfo(info, it->obj);
	return info;
}
AccountMap::AccountInfo AccountMap::require(const std::string &accountName){
	PROFILE_ME;

	auto info = get(accountName);
	if(Poseidon::hasNoneFlagsOf(info.flags, FL_VALID)){
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	return info;
}
void AccountMap::getAll(std::vector<AccountMap::AccountInfo> &ret){
	PROFILE_ME;

	ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(g_accountMap->begin(), g_accountMap->end())));
	for(auto it = g_accountMap->begin(); it != g_accountMap->end(); ++it){
		AccountInfo info;
		fillAccountInfo(info, it->obj);
		ret.emplace_back(std::move(info));
	}
}

std::string AccountMap::randomPassword(){
	PROFILE_ME;

	const auto chars = getConfig<std::string>("random_password_chars", "0123456789");
	const auto length = getConfig<std::size_t>("random_password_length", 6);

	std::string str;
	str.resize(length);
	for(auto it = str.begin(); it != str.end(); ++it){
		*it = chars.at(Poseidon::rand32() % chars.size());
	}
	return str;
}
std::string AccountMap::getPasswordHash(const std::string &password){
	PROFILE_ME;

	const auto salt = getConfig<std::string>("password_salt");
	const auto sha256 = Poseidon::sha256Hash(password + salt);
	return Poseidon::Http::base64Encode(sha256.data(), sha256.size());
}

void AccountMap::setPassword(const std::string &accountName, const std::string &password){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	it->obj->set_passwordHash(getPasswordHash(password));
}
void AccountMap::setDisposablePassword(const std::string &accountName, const std::string &password, boost::uint64_t expiryTime){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	it->obj->set_disposablePasswordHash(getPasswordHash(password));
	it->obj->set_disposablePasswordExpiryTime(expiryTime);
}
void AccountMap::commitDisposablePassword(const std::string &accountName){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_passwordHash(it->obj->unlockedGet_disposablePasswordHash());
	it->obj->set_disposablePasswordHash(VAL_INIT);
	it->obj->set_disposablePasswordExpiryTime(0);
}
void AccountMap::setPasswordRegainCooldownTime(const std::string &accountName, boost::uint64_t expiryTime){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_passwordRegainCooldownTime(expiryTime);
}
void AccountMap::setToken(const std::string &accountName, std::string token){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	it->obj->set_token(std::move(token));
}
void AccountMap::setLastLogin(const std::string &accountName, std::string lastLoginIp, boost::uint64_t lastLoginTime){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	it->obj->set_lastLoginIp(std::move(lastLoginIp));
	it->obj->set_lastLoginTime(lastLoginTime);
}
void AccountMap::setBannedUntil(const std::string &accountName, boost::uint64_t bannedUntil){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	it->obj->set_bannedUntil(bannedUntil);
}
boost::uint32_t AccountMap::decrementPasswordRetryCount(const std::string &accountName){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	auto count = it->obj->get_retryCount();
	if(count > 0){
		--count;
	}
	it->obj->set_retryCount(count);

	if(count == 0){
		auto flags = it->obj->get_flags();
		Poseidon::addFlags(flags, FL_FROZEN);
		it->obj->set_flags(flags);
	}

	return count;
}
boost::uint32_t AccountMap::resetPasswordRetryCount(const std::string &accountName){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountName);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account not found: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Account deleted: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}
	const auto passwordRetryCount = getConfig<boost::uint32_t>("password_retry_count", 10);
	it->obj->set_retryCount(passwordRetryCount);
	return passwordRetryCount;
}

void AccountMap::create(std::string accountName, std::string token, std::string remoteIp, std::string remarks, boost::uint64_t flags){
	PROFILE_ME;

	auto it = g_accountMap->find<0>(accountName);
	if(it != g_accountMap->end<0>()){
		LOG_EMPERY_GATE_WESTWALK_DEBUG("Duplicate account name: accountName = ", accountName);
		DEBUG_THROW(Exception, sslit("Duplicate account name"));
	}

	const auto tokenExpiryDuration = getConfig<boost::uint64_t>("token_expiry_duration", 604800000);

	Poseidon::addFlags(flags, AccountMap::FL_VALID);
	const auto localNow = Poseidon::getLocalTime();
	const auto passwordRetryCount = getConfig<boost::uint32_t>("password_retry_count", 10);
	auto obj = boost::make_shared<MySql::Westwalk_Account>(std::move(accountName), std::move(remoteIp), localNow,
		std::move(remarks), std::string() /* getPasswordHash(randomPassword()) */, std::string(), 0, 0,
		std::move(token), localNow + tokenExpiryDuration, std::string(), 0, 0, passwordRetryCount, flags);
	obj->asyncSave(true);
	it = g_accountMap->insert<0>(it, AccountElement(std::move(obj)));
}

}
