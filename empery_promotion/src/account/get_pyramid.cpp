#include "../precompiled.hpp"
#include "common.hpp"
#include "../singletons/account_map.hpp"
#include "../msg/err_account.hpp"

namespace EmperyPromotion {

ACCOUNT_SERVLET("getPyramid", /* session */, params){
	const auto &loginName = params.at("loginName");
	const auto &maxDepthStr = params.get("maxDepth");

	Poseidon::JsonObject ret;
	auto info = AccountMap::getByLoginName(loginName);
	if(Poseidon::hasNoneFlagsOf(info.flags, AccountMap::FL_VALID)){
		ret[sslit("errorCode")] = (int)Msg::ERR_NO_SUCH_ACCOUNT;
		ret[sslit("errorMessage")] = "Account is not found";
		return ret;
	}

	const auto level          = AccountMap::castAttribute<boost::uint64_t>(info.accountId, AccountMap::ATTR_ACCOUNT_LEVEL);
	const auto subordCount    = AccountMap::castAttribute<boost::uint64_t>(info.accountId, AccountMap::ATTR_SUBORD_COUNT);
	const auto maxSubordLevel = AccountMap::castAttribute<boost::uint64_t>(info.accountId, AccountMap::ATTR_MAX_SUBORD_LEVEL);

	const auto pyramidDepthLimit = getConfig<unsigned>("pyramid_depth_limit", 32);

	Poseidon::JsonArray members;
	std::deque<std::pair<AccountId, Poseidon::JsonArray *>> thisLevel, nextLevel;
	thisLevel.emplace_back(info.accountId, &members);
	unsigned maxDepth = 1;
	if(!maxDepthStr.empty()){
		maxDepth = boost::lexical_cast<unsigned>(maxDepthStr);
	}
	if(maxDepth > pyramidDepthLimit){
		maxDepth = pyramidDepthLimit;
	}
	for(unsigned depth = 0; depth < maxDepth; ++depth){
		if(thisLevel.empty()){
			break;
		}
		do {
			const auto currentAccountId = thisLevel.front().first;
			const auto currentArrayDest = thisLevel.front().second;
			thisLevel.pop_front();

			std::vector<AccountMap::AccountInfo> memberInfos;
			AccountMap::getByReferrerId(memberInfos, currentAccountId);
			for(auto it = memberInfos.begin(); it != memberInfos.end(); ++it){
				const auto level          = AccountMap::castAttribute<boost::uint64_t>(it->accountId, AccountMap::ATTR_ACCOUNT_LEVEL);
				const auto subordCount    = AccountMap::castAttribute<boost::uint64_t>(it->accountId, AccountMap::ATTR_SUBORD_COUNT);
				const auto maxSubordLevel = AccountMap::castAttribute<boost::uint64_t>(it->accountId, AccountMap::ATTR_MAX_SUBORD_LEVEL);
				LOG_EMPERY_PROMOTION_DEBUG("> Depth ", depth, ", accountId = ", it->accountId,
					", level = ", level, ", subordCount = ", subordCount, ", maxSubordLevel = ", maxSubordLevel);

				currentArrayDest->emplace_back(Poseidon::JsonObject());
				auto &member = currentArrayDest->back().get<Poseidon::JsonObject>();

				member[sslit("loginName")]      = std::move(it->loginName);
				member[sslit("nick")]           = std::move(it->nick);
				member[sslit("level")]          = boost::lexical_cast<std::string>(level);
				member[sslit("subordCount")]    = subordCount;
				member[sslit("maxSubordLevel")] = maxSubordLevel;
				member[sslit("members")]        = Poseidon::JsonArray();

				nextLevel.emplace_back(it->accountId, &member[sslit("members")].get<Poseidon::JsonArray>());
			}
		} while(!thisLevel.empty());

		thisLevel.swap(nextLevel);
		nextLevel.clear();
	}

	ret[sslit("loginName")]      = std::move(info.loginName);
	ret[sslit("nick")]           = std::move(info.nick);
	ret[sslit("level")]          = boost::lexical_cast<std::string>(level);
	ret[sslit("subordCount")]    = subordCount;
	ret[sslit("maxSubordLevel")] = maxSubordLevel;
	ret[sslit("members")]        = std::move(members);

	ret[sslit("errorCode")] = (int)Msg::ST_OK;
	ret[sslit("errorMessage")] = "No error";
	return ret;
}

}
