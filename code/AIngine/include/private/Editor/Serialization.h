#pragma once

#include "Structures/Traverser.h"

#include "nlohmann/json.hpp"
#include <string>
#include <vector>

namespace AIngine {
	class GameObject;
	class Sprite;
	class SoundComponent;
	class ParticleEmitter;

	namespace Rendering {
		class Texture2D;
	}

	namespace Physics {
		class PhysicsComponent;
	}
}

namespace AIngine::Editor::Serialization
{
	class Serializer {
	public:
		static void SerializeSceneGraph(const std::string& path);
		static void DeserializeSceneGraph(const std::string& path);

	private:
		static AIngine::GameObject* RestoreGameObject(const nlohmann::json* const j, AIngine::GameObject* parent);
		static void RestoreSprite(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreParticleEmitter(const nlohmann::json* const j, AIngine::GameObject* obj);
		static AIngine::Physics::PhysicsComponent* RestorePhysics(const nlohmann::json* const j, AIngine::GameObject* obj);
		static AIngine::SoundComponent* RestorySoundComponent(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreScripts(const nlohmann::json* const j, AIngine::GameObject* obj);
	};

	class SceneGraphSerializer : public AIngine::Traverser {
	public:
		// Inherited via Traverser
		virtual bool Traverse(GameObject * root) override;
		virtual bool Enter(GameObject & node) override;
		virtual bool Leave(GameObject & node) override;
		virtual bool Visit(GameObject & node) override;

		nlohmann::json Result;

	private:
		nlohmann::json SerializeScripts(GameObject& obj);
		nlohmann::json SerializeGameObject(GameObject& obj);
		nlohmann::json SerializeWorld();
		nlohmann::json SerializeSprite(AIngine::Sprite& sprite);
		nlohmann::json SerializePhysicsComponent(AIngine::Physics::PhysicsComponent& physComp);
		nlohmann::json SerializeSoundComponent(AIngine::SoundComponent& soundComp);
		nlohmann::json SerializeParticleEmitter(AIngine::ParticleEmitter& emitter);
		nlohmann::json* m_prevChildren;
		nlohmann::json* m_children;
		std::vector<AIngine::GameObject*> m_spawnedObjects;

	};

}