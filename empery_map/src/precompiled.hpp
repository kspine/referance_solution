#ifndef EMPERY_MAP_PRECOMPILED_HPP_
#define EMPERY_MAP_PRECOMPILED_HPP_

#include <poseidon/precompiled.hpp>

#include <poseidon/shared_nts.hpp>
#include <poseidon/exception.hpp>
#include <poseidon/log.hpp>
#include <poseidon/profiler.hpp>
#include <poseidon/errno.hpp>
#include <poseidon/time.hpp>
#include <poseidon/random.hpp>
#include <poseidon/flags.hpp>
#include <poseidon/module_config.hpp>
#include <poseidon/mutex.hpp>
#include <poseidon/condition_variable.hpp>
#include <poseidon/uuid.hpp>

#include "log.hpp"

namespace EmperyMap {

typedef Poseidon::Exception		Exception;
typedef Poseidon::SharedNts		SharedNts;

DECLARE_MODULE_CONFIG(getConfig, getConfigV)

using Poseidon::sslit;

}

#endif
