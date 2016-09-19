#ifndef EMPERY_CENTER_PAYMENT_TRANSACTION_HPP_
#define EMPERY_CENTER_PAYMENT_TRANSACTION_HPP_

#include <poseidon/cxx_util.hpp>
#include <poseidon/virtual_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <cstddef>
#include "id_types.hpp"

namespace EmperyCenter {

namespace MongoDb {
	class Center_PaymentTransaction;
}

class ItemBox;
class MailBox;

class PaymentTransaction : NONCOPYABLE, public virtual Poseidon::VirtualSharedFromThis {
public:
	static std::string random_serial();

private:
	const boost::shared_ptr<MongoDb::Center_PaymentTransaction> m_obj;

public:
	PaymentTransaction(std::string serial, AccountUuid account_uuid, std::uint64_t created_time, std::uint64_t expiry_time,
		ItemId item_id, std::uint64_t item_count, std::string remarks);
	explicit PaymentTransaction(boost::shared_ptr<MongoDb::Center_PaymentTransaction> obj);
	~PaymentTransaction();

public:
	const std::string &get_serial() const;
	AccountUuid get_account_uuid() const;
	std::uint64_t get_created_time() const;
	std::uint64_t get_expiry_time() const;

	ItemId get_item_id() const;
	std::uint64_t get_item_count() const;

	const std::string &get_remarks() const;
	void set_remarks(std::string remarks);

	std::uint64_t get_last_updated_time() const;
	bool has_been_committed() const;
	bool has_been_cancelled() const;
	const std::string &get_operation_remarks() const;
	void commit(const boost::shared_ptr<ItemBox> &item_box, const boost::shared_ptr<MailBox> &mail_box,
		std::string operation_remarks);
	void cancel(std::string operation_remarks);

	bool is_virtually_removed() const;
};

}

#endif
