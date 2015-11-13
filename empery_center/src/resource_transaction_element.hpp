#ifndef EMPERY_CENTER_RESOURCE_TRANSACTION_ELEMENT_HPP_
#define EMPERY_CENTER_RESOURCE_TRANSACTION_ELEMENT_HPP_

#include <boost/cstdint.hpp>
#include "id_types.hpp"

namespace EmperyCenter {

class Castle;

class ResourceTransactionElement {
	friend Castle;

public:
	enum Operation {
		OP_NONE             = 0,
		OP_ADD              = 1,
		OP_REMOVE           = 2,
		OP_REMOVE_SATURATED = 3,
	};

private:
	Operation m_operation;
	ResourceId m_resourceId;
	boost::uint64_t m_deltaCount;

	ReasonId m_reason;
	boost::uint64_t m_param1;
	boost::uint64_t m_param2;
	boost::uint64_t m_param3;

public:
	ResourceTransactionElement(Operation operation, ResourceId resourceId, boost::uint64_t deltaCount,
		ReasonId reason, boost::uint64_t param1, boost::uint64_t param2, boost::uint64_t param3)
		: m_operation(operation), m_resourceId(resourceId), m_deltaCount(deltaCount)
		, m_reason(reason), m_param1(param1), m_param2(param2), m_param3(param3)
	{
	}
};

}

#endif
