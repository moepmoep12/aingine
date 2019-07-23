#pragma once 

#include "Debug/log.h"


#ifdef EDITOR
#define ASSERT(x,...) {if(!(x)) {CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
#define ASSERT(x,...)
#endif

#define BIND_FN_0(x) std::bind(&x, this)
#define BIND_FN_1(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_FN_2(x) std::bind(&x, this, std::placeholders::_1,std::placeholders::_2)
#define BIND_FN_3(x) std::bind(&x, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)
#define BIND_FN_4(x) std::bind(&x, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4)
#define BIND_FN_5(x) std::bind(&x, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5)
