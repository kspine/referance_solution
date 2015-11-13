#ifndef EMPERY_CENTER_DATA_ITEM_HPP_
#define EMPERY_CENTER_DATA_ITEM_HPP_

#include "common.hpp"
#include <boost/container/flat_map.hpp>
#include <vector>

namespace EmperyCenter {

class ItemTransactionElement;

namespace Data {
	class Item {
	public:
		enum AutoIncType {
			AIT_NONE     = 0,
			AIT_HOURLY   = 1,
			AIT_DAILY    = 2,
			AIT_WEEKLY   = 3,
			AIT_PERIODIC = 4,
		};

	public:
		static boost::shared_ptr<const Item> get(ItemId item_id);
		static boost::shared_ptr<const Item> require(ItemId item_id);

		static void get_init(std::vector<boost::shared_ptr<const Item>> &ret);
		static void get_auto_inc(std::vector<boost::shared_ptr<const Item>> &ret);

	public:
		ItemId item_id;
		unsigned quality;
		std::pair<unsigned, unsigned> type;
		boost::uint64_t value;

		boost::uint64_t init_count;

		AutoIncType auto_inc_type;
		boost::uint64_t auto_inc_offset;
		boost::int64_t auto_inc_step;
		boost::uint64_t auto_inc_bound;
	};

	class ItemTrade {
	public:
		static boost::shared_ptr<const ItemTrade> get(TradeId trade_id);
		static boost::shared_ptr<const ItemTrade> require(TradeId trade_id);

		static void unpack(std::vector<ItemTransactionElement> &transaction,
			const boost::shared_ptr<const ItemTrade> &trade_data, boost::uint64_t repeat_count,
			boost::uint64_t param1);

	public:
		TradeId trade_id;
		boost::container::flat_map<ItemId, boost::uint64_t> items_consumed;
		boost::container::flat_map<ItemId, boost::uint64_t> items_produced;
	};

	class ItemRecharge {
	public:
		static boost::shared_ptr<const ItemRecharge> get(RechargeId recharge_id);
		static boost::shared_ptr<const ItemRecharge> require(RechargeId recharge_id);

	public:
		RechargeId recharge_id;
		TradeId trade_id;
	};

	class ItemShop {
	public:
		static boost::shared_ptr<const ItemShop> get(ShopId shop_id);
		static boost::shared_ptr<const ItemShop> require(ShopId shop_id);

	public:
		ShopId shop_id;
		TradeId trade_id;
	};
}

}

#endif
