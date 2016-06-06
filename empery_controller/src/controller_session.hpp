#ifndef EMPERY_CONTROLLER_CONTROLLER_SESSION_HPP_
#define EMPERY_CONTROLLER_CONTROLLER_SESSION_HPP_

#include <poseidon/cbpp/session.hpp>
#include <poseidon/fwd.hpp>
#include <poseidon/mutex.hpp>
#include <cstdint>
#include <boost/function.hpp>
#include <boost/container/flat_map.hpp>
#include <utility>

namespace EmperyController {

class ControllerSession : public Poseidon::Cbpp::Session {
public:
	using Result = std::pair<Poseidon::Cbpp::StatusCode, std::string>;

	using ServletCallback = boost::function<
		Result (const boost::shared_ptr<ControllerSession> &, Poseidon::StreamBuffer)>;

public:
	static boost::shared_ptr<const ServletCallback> create_servlet(std::uint16_t message_id, ServletCallback callback);
	static boost::shared_ptr<const ServletCallback> get_servlet(std::uint16_t message_id);

private:
	struct RequestElement {
		Result *result;
		boost::shared_ptr<Poseidon::JobPromise> promise;

		RequestElement(Result *result_, boost::shared_ptr<Poseidon::JobPromise> promise_)
			: result(result_), promise(std::move(promise_))
		{
		}
	};

private:
	volatile std::uint64_t m_serial;
	mutable Poseidon::Mutex m_request_mutex;
	boost::container::flat_multimap<std::uint64_t, RequestElement> m_requests;

	std::string m_name;
	std::uint64_t m_created_time = 0;

public:
	explicit ControllerSession(Poseidon::UniqueFile socket);
	~ControllerSession();

protected:
	void on_connect() override;
	void on_close(int err_code) noexcept override;

	bool on_low_level_data_message_end(std::uint64_t payload_size) override;

	void on_sync_data_message(std::uint16_t message_id, Poseidon::StreamBuffer payload) override;
	void on_sync_control_message(Poseidon::Cbpp::ControlCode control_code, std::int64_t vint_param, std::string string_param) override;

public:
	bool send(std::uint16_t message_id, Poseidon::StreamBuffer payload);
	void shutdown(const char *message) noexcept;
	void shutdown(int code, const char *message) noexcept;

	// 警告：不能在 servlet 中调用，否则会造成死锁。
	Result send_and_wait(std::uint16_t message_id, Poseidon::StreamBuffer payload);

	template<typename MsgT>
	bool send(const MsgT &msg){
		return send(MsgT::ID, Poseidon::StreamBuffer(msg));
	}
	template<typename MsgT>
	Result send_and_wait(const MsgT &msg){
		return send_and_wait(MsgT::ID, Poseidon::StreamBuffer(msg));
	}

	const std::string &get_name() const {
		return m_name;
	}
	void set_name(std::string name){
		m_name.swap(name);
	}

	std::uint64_t get_created_time() const {
		return m_created_time;
	}
	void set_created_time(std::uint64_t created_time){
		m_created_time = created_time;
	}
};

}

#endif