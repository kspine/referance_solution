#ifndef EMPERY_CLUSTER_DATA_COMMON_HPP_
#define EMPERY_CLUSTER_DATA_COMMON_HPP_

#include <cstddef>
#include <boost/shared_ptr.hpp>
#include <poseidon/fwd.hpp>
#include "../id_types.hpp"

namespace EmperyCluster {

namespace Data {
	extern Poseidon::CsvParser sync_load_data(const char *file);
}

}

#endif
