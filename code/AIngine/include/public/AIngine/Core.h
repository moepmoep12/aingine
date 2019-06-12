#pragma once

// this header is to be included by Applications created with AIngine

#include "Constants.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "Events/Event.h"
#include "Debug/log.h"
#include "AIngine/Input.h"
#include "KeyCodes.h"
#include "Assets/Assets.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "AIngine/Sprite.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/Graphics.h"
#include "World.h"
#include "Sounds.h"
#include "Rendering/Font.h"

// EntryPoint
#include "EntryPoint.h"


typedef AIngine::SoundEngine SoundEngine;
typedef AIngine::GameObject GameObject;
typedef AIngine::Graphics Graphics;
typedef AIngine::Assets::AssetRegistry Assets;

typedef AIngine::Assets::BitmapAsset BitmapAsset;
typedef AIngine::Assets::SoundAsset SoundAsset;
typedef AIngine::Assets::FontAsset FontAsset;

typedef AIngine::Physics::PhysicsComponent PhysicsComponent;
typedef AIngine::Sprite Sprite;
typedef AIngine::Sound SoundComponent;


