#pragma once 

#include "Debug/log.h"


#ifdef _DEBUG
#define ASSERT(x,...) {if(!(x)) {CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
#define ASSERT(x,...)
#endif

#define BIND_EVENT_TO_FN(x) std::bind(&x, this, std::placeholders::_1)
