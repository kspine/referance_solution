#include "../precompiled.hpp"
#include "common.hpp"
#include "../singletons/account_map.hpp"
#include "../singletons/item_map.hpp"
#include "../item_transaction_element.hpp"
#include "../item_ids.hpp"
#include "../msg/err_account.hpp"
#include <poseidon/string.hpp>

namespace EmperyPromotion {

ACCOUNT_SERVLET("deactivate", /* session */, params){
	const auto &loginName = params.at("loginName");
	const auto &remarks = params.get("remarks");

	Poseidon::JsonObject ret;
	auto info = AccountMap::getByLoginName(loginName);
	if(Poseidon::hasNoneFlagsOf(info.flags, AccountMap::FL_VALID)){
		ret[sslit("errorCode")] = (int)Msg::ERR_NO_SUCH_ACCOUNT;
		ret[sslit("errorMessage")] = "Account is not found";
		return ret;
	}

	if(Poseidon::hasAnyFlagsOf(info.flags, AccountMap::FL_DEACTIVATED)){
		ret[sslit("errorCode")] = (int)Msg::ERR_ACCOUNT_DEACTIVATED;
		ret[sslit("errorMessage")] = "Account has already been deactivated";
		return ret;
	}

	const auto initGoldCoinArray = Poseidon::explode<boost::uint64_t>(',',
	                               getConfig<std::string>("init_gold_coins_array", "100,50,50"));

	std::vector<ItemTransactionElement> transaction;
	transaction.reserve(initGoldCoinArray.size());
	auto removeGoldCoinsFromWhom = info.accountId;
	for(auto it = initGoldCoinArray.begin(); it != initGoldCoinArray.end(); ++it){
		transaction.emplace_back(removeGoldCoinsFromWhom, ItemTransactionElement::OP_REMOVE_SATURATED, ItemIds::ID_GOLD_COINS, *it,
			Events::ItemChanged::R_DEACTIVATE_ACCOUNT, info.accountId.get(), 0, 0, remarks);

		const auto info = AccountMap::require(removeGoldCoinsFromWhom);
		removeGoldCoinsFromWhom = info.referrerId;
		if(!removeGoldCoinsFromWhom){
			break;
		}
	}
	ItemMap::commitTransaction(transaction.data(), transaction.size());

	AccountMap::setPhoneNumber(info.accountId, std::string());

	Poseidon::addFlags(info.flags, AccountMap::FL_DEACTIVATED);
	AccountMap::setFlags(info.accountId, info.flags);

	AccountMap::setBannedUntil(info.accountId, (boost::uint64_t)-1);

	ret[sslit("errorCode")] = (int)Msg::ST_OK;
	ret[sslit("errorMessage")] = "No error";
	return ret;
}

}
