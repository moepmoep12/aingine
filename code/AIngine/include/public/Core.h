#pragma once

// this header is to be included by Applications created with AIngine

#include "Application.h"
#include "Events/Event.h"
#include "log.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Assets.h"
#include "GameObject.h"

// EntryPoint
#include "EntryPoint.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif
const float D2R = 180.0 / M_PI;
const float R2D = M_PI / 180.0;

