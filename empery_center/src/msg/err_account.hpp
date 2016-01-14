#ifndef EMPERY_CENTER_MSG_ERR_ACCOUNT_HPP_
#define EMPERY_CENTER_MSG_ERR_ACCOUNT_HPP_

#include <poseidon/cbpp/status_codes.hpp>

namespace EmperyCenter {

namespace Msg {
	using namespace Poseidon::Cbpp::StatusCodes;

	enum {
		ERR_NO_SUCH_ACCOUNT                 = 71201,
		ERR_TOKEN_EXPIRED                   = 71202,
		ERR_ACCOUNT_BANNED                  = 71203,
		ERR_NOT_LOGGED_IN                   = 71204,
		ERR_INVALID_TOKEN                   = 71205,
		ERR_ATTR_NOT_SETTABLE               = 71206,
		ERR_ATTR_TOO_LONG                   = 71207,
		ERR_NO_SUCH_ACCOUNT_NICK            = 71208,
		ERR_NO_SUCH_LOGIN_NAME              = 71209,
		ERR_MULTIPLE_LOGIN                  = 71210,
		ERR_NICK_TOO_LONG                   = 71211,
		ERR_NO_SUCH_SIGN_IN_REWARD          = 71212,
		ERR_ALREADY_SIGNED_IN               = 71213,
		ERR_DUPLICATE_PLATFORM_LOGIN_NAME   = 71214,
		ERR_NO_SUCH_REFERRER                = 71215,
		ERR_ACTIVATION_CODE_DELETED         = 71216,
		ERR_ACTIVATE_YOUR_ACCOUNT           = 71217,
		ERR_ACCOUNT_ALREADY_ACTIVATED       = 71218,
		ERR_INVALID_PASSWORD                = 71219,
		ERR_NO_SUCH_REFERRER_UUID           = 71220,
		ERR_VERIFICATION_CODE_EXPIRED       = 71221,
		ERR_VERIFICATION_CODE_INCORRECT     = 71222,
		ERR_VERIFICATION_CODE_FLOOD         = 71223,
		ERR_ZERO_PAYMENT_AMOUNT             = 71224,
		ERR_PAYMENT_TRANSACTION_NOT_FOUND   = 71225,
		ERR_PAYMENT_TRANSACTION_CANCELLED   = 71226,
		ERR_PAYMENT_TRANSACTION_EXPIRED     = 71227,
		ERR_PAYMENT_CHECKSUM_ERROR          = 71228,
		ERR_PAYMENT_AMOUNT_MISMATCH         = 71229,
		ERR_PAYMENT_TRANSACTION_COMMITTED   = 71230,
		ERR_NO_SUCH_PROMOTION_LEVEL         = 71231,
		ERR_AUCTION_TRANSFER_IN_PROGRESS    = 71232,
		ERR_AUCTION_TRANSFER_EMPTY          = 71233,
		ERR_AUCTION_NO_ENOUGH_ITEMS         = 71234,
		ERR_ZERO_AUCTION_ITEM_COUNT         = 71235,
		ERR_AUCTION_TRANSACTION_NOT_FOUND   = 71236,
		ERR_AUCTION_TRANSACTION_CANCELLED   = 71237,
		ERR_AUCTION_TRANSACTION_EXPIRED     = 71238,
		ERR_AUCTION_TRANSACTION_COMMITTED   = 71239,
	};
}

}

#endif
