#include "precompiled.hpp"
#include "activation_code.hpp"
#include "mysql/activation_code.hpp"
#include "singletons/activation_code_map.hpp"

namespace EmperyCenter {

ActivationCode::ActivationCode(std::string code, boost::uint64_t created_time, boost::uint64_t expiry_time)
	: m_obj(
		[&]{
			auto obj = boost::make_shared<MySql::Center_ActivationCode>(
				std::move(code), created_time, expiry_time, Poseidon::Uuid());
			obj->async_save(true);
			return obj;
		}())
{
}
ActivationCode::ActivationCode(boost::shared_ptr<MySql::Center_ActivationCode> obj)
	: m_obj(std::move(obj))
{
}
ActivationCode::~ActivationCode(){
}

const std::string &ActivationCode::get_code() const {
	return m_obj->unlocked_get_code();
}
boost::uint64_t ActivationCode::get_created_time() const {
	return m_obj->get_created_time();
}

boost::uint64_t ActivationCode::get_expiry_time() const {
	return m_obj->get_expiry_time();
}
void ActivationCode::set_expiry_time(boost::uint64_t expiry_time){
	m_obj->set_expiry_time(expiry_time);

	ActivationCodeMap::update(virtual_shared_from_this<ActivationCode>(), false);
}

AccountUuid ActivationCode::get_used_by_account() const {
	return AccountUuid(m_obj->unlocked_get_used_by_account());
}
void ActivationCode::set_used_by_account(AccountUuid account_uuid){
	m_obj->set_used_by_account(account_uuid.get());

	ActivationCodeMap::update(virtual_shared_from_this<ActivationCode>(), false);
}

void ActivationCode::delete_from_game() noexcept {
	set_expiry_time(0);
}

}
