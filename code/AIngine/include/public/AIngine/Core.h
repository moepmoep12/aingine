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
#include "AIngine/ParticleEmitter.h"
#include "AIngine/Graphics.h"
#include "AIngine/Script.h"
#include "Rendering/Font.h"
#include "Sounds.h"
#include "World.h"
#include "AIngine/Math.h"
#include "Rendering/texture.h"

#include <glm/glm.hpp>

/* Engine systems*/

typedef AIngine::SoundEngine SoundEngine;
typedef AIngine::Sound Sound;
typedef AIngine::Graphics Graphics;
typedef AIngine::Input Input;

/* Assets */

typedef AIngine::Assets::AssetRegistry Assets;
typedef AIngine::Assets::BitmapAsset BitmapAsset;
typedef AIngine::Assets::SoundAsset SoundAsset;
typedef AIngine::Assets::FontAsset FontAsset;

/* Component typedefs*/

typedef AIngine::GameObject GameObject;
typedef AIngine::Physics::PhysicsComponent PhysicsComponent;
typedef AIngine::Sprite Sprite;
typedef AIngine::Script Script;
typedef AIngine::SoundComponent SoundComponent;
typedef AIngine::ParticleEmitter ParticleEmitter;

/* Misc*/

typedef AIngine::Particle Particle;
typedef AIngine::Rendering::Texture2D Texture2D;


