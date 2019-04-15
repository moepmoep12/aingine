#pragma once

// this header is to be included by Applications created with AIngine

#include "Application.h"
#include "Events/Event.h"
#include "log.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Assets.h"

// EntryPoint
#include "EntryPoint.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)


