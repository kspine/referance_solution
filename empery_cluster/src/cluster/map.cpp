#include "../precompiled.hpp"
#include "common.hpp"

#include <poseidon/singletons/timer_daemon.hpp>

namespace EmperyCluster {

// FIXME  REMOVE TEST

namespace Msg {
#define MESSAGE_NAME TestMessage
#define MESSAGE_ID   101
#define MESSAGE_FIELDS \
  FIELD_STRING(str)
#include <poseidon/cbpp/message_generator.hpp>

#define MESSAGE_NAME TestNestedMessage
#define MESSAGE_ID   102
#define MESSAGE_FIELDS \
  FIELD_STRING(str)
#include <poseidon/cbpp/message_generator.hpp>
}

CLUSTER_SERVLET(Msg::TestNestedMessage, client, req){
    LOG_EMPERY_CLUSTER_ERROR("Received nested message: ", req);
    return std::make_pair(999, "world!");
}

MODULE_RAII(handles){
	auto client = boost::make_shared<ClusterClient>(Poseidon::IpPort(sslit("127.0.0.1"), 13217), false, 15000);
	client->send(Msg::TestMessage(std::string("meow!")));
	client->goResident();

	auto timer = Poseidon::TimerDaemon::registerTimer(1000, 0, std::bind([=](){
		LOG_EMPERY_CLUSTER_WARNING("Shutdown client now!");
		client->shutdownWrite();
	}));
	handles.push(timer);
}

}
