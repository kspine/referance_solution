#ifndef EMPERY_CENTER_EVENTS_ITEM_HPP_
#define EMPERY_CENTER_EVENTS_ITEM_HPP_

#include <poseidon/event_base.hpp>
#include "../id_types.hpp"

namespace EmperyCenter {

namespace Events {
	struct ItemChanged : public Poseidon::EventBase<340201> {
		AccountUuid account_uuid;
		ItemId item_id;
		std::uint64_t old_count;
		std::uint64_t new_count;

		ReasonId reason;
		std::int64_t param1;
		std::int64_t param2;
		std::int64_t param3;

		ItemChanged(AccountUuid account_uuid_,
			ItemId item_id_, std::uint64_t old_count_, std::uint64_t new_count_,
			ReasonId reason_, std::int64_t param1_, std::int64_t param2_, std::int64_t param3_)
			: account_uuid(account_uuid_)
			, item_id(item_id_), old_count(old_count_), new_count(new_count_)
			, reason(reason_), param1(param1_), param2(param2_), param3(param3_)
		{
		}
	};
}

}

#endif
