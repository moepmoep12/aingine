#pragma once

// this header is to be included by Applications created with AIngine

#include "Application.h"
#include "Events/Event.h"
#include "Debug/log.h"
#include "AIngine/Input.h"
#include "KeyCodes.h"
#include "Assets/Assets.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "World.h"

// EntryPoint
#include "EntryPoint.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif
const float D2R = 180.0 / M_PI;
const float R2D = M_PI / 180.0;

