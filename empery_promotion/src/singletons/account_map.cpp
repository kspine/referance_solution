#include "../precompiled.hpp"
#include "account_map.hpp"
#include <string.h>
#include <poseidon/multi_index_map.hpp>
#include <poseidon/singletons/mysql_daemon.hpp>
#include <poseidon/singletons/event_dispatcher.hpp>
#include <poseidon/hash.hpp>
#include <poseidon/http/utilities.hpp>
#include "../mysql/account.hpp"
#include "../mysql/account_attribute.hpp"
#include "../events/account.hpp"

namespace EmperyPromotion {

namespace {
	struct StringCaseComparator {
		bool operator()(const std::string &lhs, const std::string &rhs) const {
			return ::strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
		}
	};

	struct AccountElement {
		boost::shared_ptr<MySql::Promotion_Account> obj;

		AccountId accountId;
		std::string loginName;
		std::string nick;
		AccountId referrereId;

		explicit AccountElement(boost::shared_ptr<MySql::Promotion_Account> obj_)
			: obj(std::move(obj_))
			, accountId(obj->get_accountId()), loginName(obj->unlockedGet_loginName()), nick(obj->unlockedGet_nick())
			, referrereId(obj->get_referrerId())
		{
		}
	};

	MULTI_INDEX_MAP(AccountMapDelegator, AccountElement,
		UNIQUE_MEMBER_INDEX(accountId)
		UNIQUE_MEMBER_INDEX(loginName, StringCaseComparator)
		MULTI_MEMBER_INDEX(nick, StringCaseComparator)
		MULTI_MEMBER_INDEX(referrereId)
	)

	boost::shared_ptr<AccountMapDelegator> g_accountMap;

	struct AccountAttributeElement {
		boost::shared_ptr<MySql::Promotion_AccountAttribute> obj;

		AccountId accountId;
		std::pair<AccountId, unsigned> accountSlot;

		explicit AccountAttributeElement(boost::shared_ptr<MySql::Promotion_AccountAttribute> obj_)
			: obj(std::move(obj_))
			, accountId(obj->get_accountId())
			, accountSlot(std::make_pair(accountId, obj->get_slot()))
		{
		}
	};

	MULTI_INDEX_MAP(AccountAttributeMapDelegator, AccountAttributeElement,
		MULTI_MEMBER_INDEX(accountId)
		UNIQUE_MEMBER_INDEX(accountSlot)
	);

	boost::shared_ptr<AccountAttributeMapDelegator> g_attributeMap;

	MODULE_RAII_PRIORITY(handles, 5000){
		const auto conn = Poseidon::MySqlDaemon::createConnection();

		const auto accountMap = boost::make_shared<AccountMapDelegator>();
		LOG_EMPERY_PROMOTION_INFO("Loading accounts...");
		conn->executeSql("SELECT * FROM `Promotion_Account`");
		while(conn->fetchRow()){
			auto obj = boost::make_shared<MySql::Promotion_Account>();
			obj->syncFetch(conn);
			obj->enableAutoSaving();
			accountMap->insert(AccountElement(std::move(obj)));
		}
		LOG_EMPERY_PROMOTION_INFO("Loaded ", accountMap->size(), " account(s).");
		g_accountMap = accountMap;
		handles.push(accountMap);

		const auto attributeMap = boost::make_shared<AccountAttributeMapDelegator>();
		LOG_EMPERY_PROMOTION_INFO("Loading account attributes...");
		conn->executeSql("SELECT * FROM `Promotion_AccountAttribute`");
		while(conn->fetchRow()){
			auto obj = boost::make_shared<MySql::Promotion_AccountAttribute>();
			obj->syncFetch(conn);
			const auto it = g_accountMap->find<0>(AccountId(obj->get_accountId()));
			if(it == g_accountMap->end<0>()){
				LOG_EMPERY_PROMOTION_ERROR("No such account: accountId = ", AccountId(obj->get_accountId()));
				continue;
			}
			obj->enableAutoSaving();
			attributeMap->insert(AccountAttributeElement(std::move(obj)));
		}
		LOG_EMPERY_PROMOTION_INFO("Loaded ", attributeMap->size(), " account attribute(s).");
		g_attributeMap = attributeMap;
		handles.push(attributeMap);
	}
}

bool AccountMap::has(AccountId accountId){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		return false;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
		return false;
	}
	return true;
}
bool AccountMap::has(const std::string &loginName){
	PROFILE_ME;

	const auto it = g_accountMap->find<1>(loginName);
	if(it == g_accountMap->end<1>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: loginName = ", loginName);
		return false;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: loginName = ", loginName);
		return false;
	}
	return true;
}
AccountMap::AccountInfo AccountMap::get(AccountId accountId){
	PROFILE_ME;

	AccountInfo info = { };
	info.accountId = accountId;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		return info;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		return info;
	}
	info.loginName = it->obj->unlockedGet_loginName();
	info.nick = it->obj->unlockedGet_nick();
	info.passwordHash = it->obj->unlockedGet_passwordHash();
	info.dealPasswordHash = it->obj->unlockedGet_dealPasswordHash();
	info.referrerId = AccountId(it->obj->get_referrerId());
	info.flags = it->obj->get_flags();
	info.bannedUntil = it->obj->get_bannedUntil();
	info.createdTime = it->obj->get_createdTime();
	return info;
}
AccountMap::AccountInfo AccountMap::get(const std::string &loginName){
	PROFILE_ME;

	AccountInfo info = { };
	info.loginName = loginName;

	const auto it = g_accountMap->find<1>(loginName);
	if(it == g_accountMap->end<1>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: loginName = ", loginName);
		return info;
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: loginName = ", loginName);
		return info;
	}
	info.accountId = AccountId(it->obj->get_accountId());
	info.nick = it->obj->unlockedGet_nick();
	info.passwordHash = it->obj->unlockedGet_passwordHash();
	info.dealPasswordHash = it->obj->unlockedGet_dealPasswordHash();
	info.referrerId = AccountId(it->obj->get_referrerId());
	info.flags = it->obj->get_flags();
	info.bannedUntil = it->obj->get_bannedUntil();
	info.createdTime = it->obj->get_createdTime();
	return info;
}
AccountMap::AccountInfo AccountMap::require(AccountId accountId){
	PROFILE_ME;

	auto info = get(accountId);
	if(Poseidon::hasNoneFlagsOf(info.flags, FL_VALID)){
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	return info;
}
AccountMap::AccountInfo AccountMap::require(const std::string &loginName){
	PROFILE_ME;

	auto info = get(loginName);
	if(Poseidon::hasNoneFlagsOf(info.flags, FL_VALID)){
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	return info;
}
void AccountMap::getAll(std::vector<AccountMap::AccountInfo> &ret){
	PROFILE_ME;

	ret.reserve(ret.size() + g_accountMap->size());
	for(auto it = g_accountMap->begin(); it != g_accountMap->end(); ++it){
		AccountInfo info;
		info.accountId = AccountId(it->obj->get_accountId());
		info.loginName = it->obj->unlockedGet_loginName();
		info.nick = it->obj->unlockedGet_nick();
		info.passwordHash = it->obj->unlockedGet_passwordHash();
		info.dealPasswordHash = it->obj->unlockedGet_dealPasswordHash();
		info.referrerId = AccountId(it->obj->get_referrerId());
		info.flags = it->obj->get_flags();
		info.bannedUntil = it->obj->get_bannedUntil();
		info.createdTime = it->obj->get_createdTime();
		ret.push_back(std::move(info));
	}
}

void AccountMap::getByReferrerId(std::vector<AccountMap::AccountInfo> &ret, AccountId referrerId){
	PROFILE_ME;

	const auto range = g_accountMap->equalRange<3>(referrerId);
	ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
	for(auto it = range.first; it != range.second; ++it){
		if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
			LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", it->obj->get_accountId());
			continue;
		}
		AccountInfo info;
		info.accountId = AccountId(it->obj->get_accountId());
		info.loginName = it->obj->unlockedGet_loginName();
		info.nick = it->obj->unlockedGet_nick();
		info.passwordHash = it->obj->unlockedGet_passwordHash();
		info.dealPasswordHash = it->obj->unlockedGet_dealPasswordHash();
		info.referrerId = AccountId(it->obj->get_referrerId());
		info.flags = it->obj->get_flags();
		info.bannedUntil = it->obj->get_bannedUntil();
		info.createdTime = it->obj->get_createdTime();
		ret.push_back(std::move(info));
	}
}

std::string AccountMap::getPasswordHash(const std::string &password){
	PROFILE_ME;

	auto salt = getConfig<std::string>("password_salt");
	const auto sha256 = Poseidon::sha256Hash(password + std::move(salt));
	return Poseidon::Http::base64Encode(sha256.data(), sha256.size());
}

void AccountMap::setNick(AccountId accountId, std::string nick){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_nick(std::move(nick));
}
void AccountMap::setPassword(AccountId accountId, const std::string &password){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_passwordHash(getPasswordHash(password));
}
void AccountMap::setDealPassword(AccountId accountId, const std::string &dealPassword){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_dealPasswordHash(getPasswordHash(dealPassword));
}
void AccountMap::setFlags(AccountId accountId, boost::uint64_t flags){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
//	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
//		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
//		DEBUG_THROW(Exception, sslit("Account deleted"));
//	}

	it->obj->set_flags(flags);
}
void AccountMap::setBannedUntil(AccountId accountId, boost::uint64_t bannedUntil){
	PROFILE_ME;

	const auto it = g_accountMap->find<0>(accountId);
	if(it == g_accountMap->end<0>()){
		LOG_EMPERY_PROMOTION_DEBUG("Account not found: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account not found"));
	}
	if(Poseidon::hasNoneFlagsOf(it->obj->get_flags(), FL_VALID)){
		LOG_EMPERY_PROMOTION_DEBUG("Account deleted: accountId = ", accountId);
		DEBUG_THROW(Exception, sslit("Account deleted"));
	}

	it->obj->set_bannedUntil(bannedUntil);
}

AccountId AccountMap::create(std::string loginName, std::string nick,
	const std::string &password, const std::string &dealPassword, AccountId referrerId, boost::uint64_t flags)
{
	PROFILE_ME;

	auto it = g_accountMap->find<1>(loginName);
	if(it != g_accountMap->end<1>()){
		LOG_EMPERY_PROMOTION_DEBUG("Duplicate loginName: loginName = ", loginName);
		DEBUG_THROW(Exception, sslit("Duplicate loginName"));
	}

	auto accountId = g_accountMap->empty() ? AccountId() : g_accountMap->rbegin<0>()->accountId;
	do {
		++accountId;
	} while(g_accountMap->find<0>(accountId) != g_accountMap->end<0>());

	Poseidon::addFlags(flags, AccountMap::FL_VALID);
	const auto localNow = Poseidon::getLocalTime();
	auto obj = boost::make_shared<MySql::Promotion_Account>(accountId.get(), std::move(loginName), std::move(nick),
		getPasswordHash(password), getPasswordHash(dealPassword), referrerId.get(), flags, 0, localNow);
	obj->asyncSave(true);
	it = g_accountMap->insert<1>(it, AccountElement(std::move(obj)));

	return accountId;
}

const std::string &AccountMap::getAttribute(AccountId accountId, unsigned slot){
	PROFILE_ME;

	const auto it = g_attributeMap->find<1>(std::make_pair(accountId, slot));
	if(it == g_attributeMap->end<1>()){
		LOG_EMPERY_PROMOTION_TRACE("Account attribute not found: accountId = ", accountId, ", slot = ", slot);
		return Poseidon::EMPTY_STRING;
	}
	return it->obj->unlockedGet_value();
}
void AccountMap::getAttributes(std::vector<std::pair<unsigned, std::string> > &ret, AccountId accountId){
	PROFILE_ME;

	const auto range = g_attributeMap->equalRange<0>(accountId);
	ret.reserve(ret.size() + static_cast<std::size_t>(std::distance(range.first, range.second)));
	for(auto it = range.first; it != range.second; ++it){
		ret.push_back(std::make_pair(it->obj->get_slot(), it->obj->unlockedGet_value()));
	}
}
void AccountMap::touchAttribute(AccountId accountId, unsigned slot){
	PROFILE_ME;

	auto it = g_attributeMap->find<1>(std::make_pair(accountId, slot));
	if(it == g_attributeMap->end<1>()){
		it = g_attributeMap->insert<1>(it, AccountAttributeElement(
			boost::make_shared<MySql::Promotion_AccountAttribute>(accountId.get(), slot, std::string())));
		it->obj->asyncSave(true);
	}
}
void AccountMap::setAttribute(AccountId accountId, unsigned slot, std::string value){
	PROFILE_ME;

	auto it = g_attributeMap->find<1>(std::make_pair(accountId, slot));
	if(it == g_attributeMap->end<1>()){
		it = g_attributeMap->insert<1>(it, AccountAttributeElement(
			boost::make_shared<MySql::Promotion_AccountAttribute>(accountId.get(), slot, std::move(value))));
		it->obj->asyncSave(true);
	} else {
		it->obj->set_value(std::move(value));
	}
}

}
