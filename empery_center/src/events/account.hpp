#ifndef EMPERY_CENTER_EVENTS_ACCOUNT_HPP_
#define EMPERY_CENTER_EVENTS_ACCOUNT_HPP_

#include <poseidon/event_base.hpp>
#include <poseidon/shared_nts.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

namespace Events {
	struct AccountLoggedIn : public Poseidon::EventBase<330101> {
		AccountUuid accountUuid;
		Poseidon::SharedNts remoteIp;

		AccountLoggedIn(const AccountUuid &accountUuid_, Poseidon::SharedNts remoteIp_)
			: accountUuid(accountUuid_), remoteIp(std::move(remoteIp_))
		{
		}
	};

	struct AccountLoggedOut : public Poseidon::EventBase<330102> {
		AccountUuid accountUuid;
		boost::uint64_t onlineDuration;

		AccountLoggedOut(const AccountUuid &accountUuid_, boost::uint64_t onlineDuration_)
			: accountUuid(accountUuid_), onlineDuration(onlineDuration_)
		{
		}
	};

	struct AccountSetToken : public Poseidon::EventBase<330103> {
		PlatformId platformId;
		std::string loginName;
		std::string loginToken;
		boost::uint64_t expiryDuration;

		AccountSetToken(PlatformId platformId_, std::string loginName_,
			std::string loginToken_, boost::uint64_t expiryDuration_)
			: platformId(platformId_), loginName(std::move(loginName_))
			, loginToken(std::move(loginToken_)), expiryDuration(expiryDuration_)
		{
		}
	};
}

}

#endif
