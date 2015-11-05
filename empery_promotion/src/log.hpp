#ifndef EMPERY_PROMOTION_LOG_HPP_
#define EMPERY_PROMOTION_LOG_HPP_

#include <poseidon/log.hpp>

namespace EmperyPromotion {

const unsigned long long LOG_CATEGORY = 0x00010100;

}

#define LOG_EMPERY_PROMOTION(level_, ...)	\
	LOG_MASK(::EmperyPromotion::LOG_CATEGORY | (level_), __VA_ARGS__)

#define LOG_EMPERY_PROMOTION_FATAL(...)     LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_FATAL,      __VA_ARGS__)
#define LOG_EMPERY_PROMOTION_ERROR(...)     LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_ERROR,      __VA_ARGS__)
#define LOG_EMPERY_PROMOTION_WARNING(...)   LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_WARNING,    __VA_ARGS__)
#define LOG_EMPERY_PROMOTION_INFO(...)      LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_INFO,       __VA_ARGS__)
#define LOG_EMPERY_PROMOTION_DEBUG(...)     LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_DEBUG,      __VA_ARGS__)
#define LOG_EMPERY_PROMOTION_TRACE(...)     LOG_EMPERY_PROMOTION(::Poseidon::Logger::LV_TRACE,      __VA_ARGS__)

#endif
