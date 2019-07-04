#pragma once

#include "Structures/Traverser.h"
#include "Structures/Rectangle.h"
#include "Assets/Assets.h"
#include "Rendering/texture.h"

#include "nlohmann/json.hpp"
#include "glm/glm.hpp"
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

	namespace Editor {
		struct Scene;
	}

	namespace UI {
		class Canvas;
		class Button;
		class Image;
		class UIText;
		class CheckBox;
	}
}

namespace AIngine::Editor::Serialization
{
	std::vector<std::string> ExtractPathsFromScene(const std::string& sceneFilePath);
	std::string SerializePath(const std::string& path);

	class Serializer {
	public:
		static void SerializeSceneGraph(const std::string& path);
		static void DeserializeSceneGraph(const std::string& path);
		static void SaveEditorSettings();
		static void LoadEditorSettings();
		static std::vector<Scene> LoadBuildScenes();
		static void SaveBuildScenes(const std::vector<Scene>& scenes);

	private:
		static AIngine::GameObject* RestoreGameObject(const nlohmann::json* const j, AIngine::GameObject* parent);
		static void RestoreSprite(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreParticleEmitter(const nlohmann::json* const j, AIngine::GameObject* obj);
		static AIngine::Physics::PhysicsComponent* RestorePhysics(const nlohmann::json* const j, AIngine::GameObject* obj);
		static AIngine::SoundComponent* RestorySoundComponent(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreScripts(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreCanvas(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreButton(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreImage(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreUIText(const nlohmann::json* const j, AIngine::GameObject* obj);
		static void RestoreCheckBox(const nlohmann::json* const j, AIngine::GameObject* obj);
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
		nlohmann::json SerializeCanvas(AIngine::UI::Canvas& canvas);
		nlohmann::json SerializeButton(AIngine::UI::Button& button);
		nlohmann::json SerializeImage(AIngine::UI::Image& image);
		nlohmann::json SerializeText(AIngine::UI::UIText& uitext);
		nlohmann::json SerializeCheckBox(AIngine::UI::CheckBox& checkbox);
		std::vector<nlohmann::json*> m_prevChildren;
		nlohmann::json* m_children;
		std::vector<AIngine::GameObject*> m_spawnedObjects;


	};

}

// Serialization of glm's datatypes
namespace glm
{
	inline void to_json(nlohmann::json& j, const glm::vec2& vec) {
		j = nlohmann::json{ {"x",vec.x,}, {"y",vec.y} };
	}

	inline void from_json(const nlohmann::json& j, glm::vec2& vec) {
		j.at("x").get_to(vec.x);
		j.at("y").get_to(vec.y);
	}

	inline void to_json(nlohmann::json& j, const glm::vec3& vec) {
		j = nlohmann::json{ {"x",vec.x,}, {"y",vec.y}, {"z",vec.z} };
	}

	inline void from_json(const nlohmann::json& j, glm::vec3& vec) {
		j.at("x").get_to(vec.x);
		j.at("y").get_to(vec.y);
		j.at("z").get_to(vec.z);
	}

	inline void to_json(nlohmann::json& j, const glm::vec4& vec) {
		j = nlohmann::json{ {"x",vec.x,}, {"y",vec.y}, {"z",vec.z},{"w",vec.w} };
	}

	inline void from_json(const nlohmann::json& j, glm::vec4& vec) {
		j.at("x").get_to(vec.x);
		j.at("y").get_to(vec.y);
		j.at("z").get_to(vec.z);
		j.at("w").get_to(vec.w);
	}
}

// Serialization of Rectangle
namespace AIngine::Structures
{
	template<typename T>
	inline void to_json(nlohmann::json& j, const AIngine::Structures::Rectangle<T>& rect) {
		j = nlohmann::json{ {"x",rect.x,}, {"y",rect.y}, {"width",rect.width}, {"height",rect.height} };
	}

	template<typename T>
	inline void from_json(const nlohmann::json& j, AIngine::Structures::Rectangle<T>& rect) {
		j.at("x").get_to(rect.x);
		j.at("y").get_to(rect.y);
		j.at("width").get_to(rect.width);
		j.at("height").get_to(rect.height);
	}
}

namespace AIngine::Rendering
{
	inline void to_json(nlohmann::json& j, const AIngine::Rendering::Texture2D& texture)
	{
		j = nlohmann::json{
			{"path", AIngine::Editor::Serialization::SerializePath(texture.FileName)},
			{"wrapS", texture.Wrap_S},
			{"wrapT", texture.Wrap_T},
			{"filterMin", texture.Filter_Min},
			{"filterMax", texture.Filter_Max},
			{"imageFormat", texture.Image_Format},
		};
	}

	inline void from_json(const nlohmann::json& j, AIngine::Rendering::Texture2D& texture) {
		j.at("wrapS").get_to(texture.Wrap_S);
		j.at("wrapT").get_to(texture.Wrap_T);
		j.at("filterMin").get_to(texture.Filter_Min);
		j.at("filterMax").get_to(texture.Filter_Max);
		j.at("imageFormat").get_to(texture.Image_Format);
		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>(j.at("path"))->GetBitmap();
		texture.Generate(bitmap);
	}
}