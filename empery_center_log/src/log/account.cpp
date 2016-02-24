#include "../precompiled.hpp"
#include "common.hpp"
#include <poseidon/singletons/mysql_daemon.hpp>
#include <poseidon/singletons/job_dispatcher.hpp>
#include <poseidon/mysql/connection.hpp>
#include <poseidon/mysql/utilities.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include "../checked_arithmetic.hpp"

namespace EmperyCenterLog {

LOG_SERVLET("account/realtime_online", root, session, params){
	const auto &since_str    = params.get("since");
	const auto &duration_str = params.get("duration");

	std::uint64_t since = 0, duration = UINT64_MAX;
	if(!since_str.empty()){
		since = boost::lexical_cast<std::uint64_t>(since_str);
	}
	if(!duration_str.empty()){
		duration = boost::lexical_cast<std::uint64_t>(duration_str);
	}

	struct CounterElement {
		std::uint64_t interval;
		std::uint64_t account_count;
	};
	boost::container::flat_map<std::uint64_t, CounterElement> counter_map;
	{
		std::ostringstream oss;
		oss <<"SELECT * FROM `CenterLog_AccountNumberOnline` WHERE " <<Poseidon::MySql::DateTimeFormatter(since) <<" <= `timestamp` "
		    <<"  AND `timestamp` < " <<Poseidon::MySql::DateTimeFormatter(saturated_add(since, duration)) <<" ";
		const auto promise = Poseidon::MySqlDaemon::enqueue_for_batch_loading(
			[&](const boost::shared_ptr<Poseidon::MySql::Connection> &conn){
				const auto timestamp = conn->get_datetime("timestamp");
				auto &counters = counter_map[timestamp];
				counters.interval = conn->get_unsigned("interval");
				counters.account_count = conn->get_unsigned("account_count");
			}, "CenterLog_AccountNumberOnline", oss.str());
		Poseidon::JobDispatcher::yield(promise, true);
	}

	Poseidon::JsonArray realtime_online;
	for(auto it = counter_map.begin(); it != counter_map.end(); ++it){
		Poseidon::JsonObject object;
		object[sslit("timestamp")]     = it->first;
		object[sslit("interval")]      = it->second.interval;
		object[sslit("account_count")] = it->second.account_count;
		realtime_online.emplace_back(std::move(object));
	}
	root[sslit("realtime_online")] = std::move(realtime_online);

	return Response();
}

LOG_SERVLET("account/daily_logged_in", root, session, params){
	const auto &since_str     = params.get("since");
	const auto &duration_str  = params.get("duration");
	const auto &tz_offset_str = params.get("tz_offset");

	std::uint64_t since = 0, duration = UINT64_MAX;
	if(!since_str.empty()){
		since = boost::lexical_cast<std::uint64_t>(since_str);
	}
	if(!duration_str.empty()){
		duration = boost::lexical_cast<std::uint64_t>(duration_str);
	}
	std::uint64_t tz_offset = 0;
	if(!tz_offset_str.empty()){
		const auto delta = boost::lexical_cast<std::int64_t>(tz_offset_str);
		tz_offset = static_cast<std::uint64_t>(86400000 + delta) % 86400000;
	}

	struct CounterElement {
		boost::container::flat_set<std::string> accounts;
		boost::container::flat_set<std::string> ips;
	};
	boost::container::flat_map<std::uint64_t, CounterElement> counter_map;
	{
		std::ostringstream oss;
		oss <<"SELECT * FROM `CenterLog_AccountLoggedIn`  WHERE " <<Poseidon::MySql::DateTimeFormatter(since) <<" <= `timestamp` "
		    <<"  AND `timestamp` < " <<Poseidon::MySql::DateTimeFormatter(saturated_add(since, duration)) <<" ";
		const auto promise = Poseidon::MySqlDaemon::enqueue_for_batch_loading(
			[&](const boost::shared_ptr<Poseidon::MySql::Connection> &conn){
				const auto timestamp = conn->get_datetime("timestamp");
				const auto rounded_timestamp = (timestamp + tz_offset) / 86400000 * 86400000 - tz_offset;
				auto &counters = counter_map[rounded_timestamp];
				counters.accounts.insert(conn->get_string("account_uuid"));
				counters.ips.insert(conn->get_string("remote_ip"));
			}, "CenterLog_AccountLoggedIn", oss.str());
		Poseidon::JobDispatcher::yield(promise, true);
	}

	Poseidon::JsonArray daily_logged_in;
	for(auto it = counter_map.begin(); it != counter_map.end(); ++it){
		Poseidon::JsonObject object;
		object[sslit("timestamp")]     = it->first;
		object[sslit("accounts")]      = it->second.accounts.size();
		object[sslit("ips")]           = it->second.ips.size();
		daily_logged_in.emplace_back(std::move(object));
	}
	root[sslit("daily_logged_in")] = std::move(daily_logged_in);

	return Response();
}

LOG_SERVLET("account/daily_created", root, session, params){
	const auto &since_str     = params.get("since");
	const auto &duration_str  = params.get("duration");
	const auto &tz_offset_str = params.get("tz_offset");

	std::uint64_t since = 0, duration = UINT64_MAX;
	if(!since_str.empty()){
		since = boost::lexical_cast<std::uint64_t>(since_str);
	}
	if(!duration_str.empty()){
		duration = boost::lexical_cast<std::uint64_t>(duration_str);
	}
	std::uint64_t tz_offset = 0;
	if(!tz_offset_str.empty()){
		const auto delta = boost::lexical_cast<std::int64_t>(tz_offset_str);
		tz_offset = static_cast<std::uint64_t>(86400000 + delta) % 86400000;
	}

	struct CounterElement {
		boost::container::flat_set<std::string> accounts;
		boost::container::flat_set<std::string> ips;
	};
	boost::container::flat_map<std::uint64_t, CounterElement> counter_map;
	{
		std::ostringstream oss;
		oss <<"SELECT * FROM `CenterLog_AccountLoggedIn` AS `a` INNER JOIN ( "
		    <<"  SELECT `account_uuid` AS `earliest_account_uuid`, MIN(`timestamp`) AS `earliest` FROM `CenterLog_AccountLoggedIn` "
		    <<"  GROUP BY `timestamp` "
		    <<") AS `z` ON `account_uuid` = `earliest_account_uuid` AND `timestamp` = `earliest` "
		    <<"WHERE " <<Poseidon::MySql::DateTimeFormatter(since) <<" <= `timestamp` "
		    <<"  AND `timestamp` < " <<Poseidon::MySql::DateTimeFormatter(saturated_add(since, duration)) <<" ";
		const auto promise = Poseidon::MySqlDaemon::enqueue_for_batch_loading(
			[&](const boost::shared_ptr<Poseidon::MySql::Connection> &conn){
				const auto timestamp = conn->get_datetime("timestamp");
				const auto rounded_timestamp = (timestamp + tz_offset) / 86400000 * 86400000 - tz_offset;
				auto &counters = counter_map[rounded_timestamp];
				counters.accounts.insert(conn->get_string("account_uuid"));
				counters.ips.insert(conn->get_string("remote_ip"));
			}, "CenterLog_AccountNumberOnline", oss.str());
		Poseidon::JobDispatcher::yield(promise, true);
	}

	Poseidon::JsonArray daily_created;
	for(auto it = counter_map.begin(); it != counter_map.end(); ++it){
		Poseidon::JsonObject object;
		object[sslit("timestamp")]     = it->first;
		object[sslit("accounts")]      = it->second.accounts.size();
		object[sslit("ips")]           = it->second.ips.size();
		daily_created.emplace_back(std::move(object));
	}
	root[sslit("daily_created")] = std::move(daily_created);

	return Response();
}

}
