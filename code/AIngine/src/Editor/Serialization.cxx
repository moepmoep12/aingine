#include "Editor/Serialization.h"
#include "Editor/Editor.h"
#include "Rendering/texture.h"
#include "AIngine/Physics.h"
#include "AIngine/GameObject.h"
#include "Structures/SceneGraph.h"
#include "AIngine/World.h"
#include "Assets/Assets.h"
#include "AIngine/World.h"
#include "AIngine/Sprite.h"
#include "AIngine/SoundComponent.h"
#include "AIngine/Script.h"
#include "Application.h"

#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <filesystem>

namespace AIngine::Editor::Serialization {

	namespace AttributeNames {

		// world
		const char* WORLD_GRAVITY_X = "gravityX";
		const char* WORLD_GRAVITY_Y = "gravityY";
		const char* WORLD_BOTTOMLEFT = "bottomLeft";
		const char* WORLD_BOTTOMRIGHT = "bottomRight";
		const char* WORLD_TOPLEFT = "topleft";
		const char* WORLD_TOPRIGHT = "topRight";

		// GameObject
		const char* GAMEOBJECT_NAME = "a_name";
		const char* GAMEOBJECT_POSITION_X = "a_posX";
		const char* GAMEOBJECT_POSITION_Y = "a_posY";
		const char* GAMEOBJECT_SCALE_X = "a_scaleX";
		const char* GAMEOBJECT_SCALE_Y = "a_scaleY";
		const char* GAMEOBJECT_ROTATION = "a_rotation";
		const char* GAMEOBJECT_COMPONENTS = "b_components";
		const char* GAMEOBJECT_CHILDREN = "c_children";

		// Components
		const char* COMPONENT_ACTIVE = "active";
		const char* COMPONENT_SPRITE = "sprite";
		const char* COMPONENT_PHYSICS = "physics";
		const char* COMPONENT_SOUND = "soundComponent";
		const char* COMPONENT_SCRIPT = "script";

		// Script
		const char* SCRIPT_INDEX = "index";

		// Sprite
		const char* SPRITE_COLOR_R = "colorR";
		const char* SPRITE_COLOR_G = "colorG";
		const char* SPRITE_COLOR_B = "colorB";
		const char* SPRITE_ALPHA = "alpha";
		const char* SPRITE_SIZE_X = "sizeX";
		const char* SPRITE_SIZE_Y = "sizeY";
		const char* SPRITE_PARALLAX_X = "parallaxX";
		const char* SPRITE_PARALLAX_Y = "parallaxY";

		// Texture
		const char* TEXTURE_PATH = "path";
		const char* TEXTURE_WRAP_S = "wrapS";
		const char* TEXTURE_WRAP_T = "wrapT";
		const char* TEXTURE_FILTER_MIN = "filterMin";
		const char* TEXTURE_FILTER_MAX = "filterMax";
		const char* TEXTURE_IMAGEFORMAT = "imageFormat";

		// PhysicsComponent
		const char* PHYSICS_SHAPE = "shape";
		const char* PHYSICS_BODYTYPE = "bodyType";
		const char* PHYSICS_FRICTION = "friction";
		const char* PHYSICS_RESTITUTION = "restitution";
		const char* PHYSICS_DENSITY = "density";
		const char* PHYSICS_RADIUS = "radius";
		const char* PHYSICS_WIDTH = "width";
		const char* PHYSICS_HEIGHT = "height";
		const char* PHYSICS_VERTICES = "vertices";
		const char* PHYSICS_VERTEXCOUNT = "vertexcount";
		const char* PHYSICS_ISTRIGGER = "isTrigger";

		// SoundComponent
		const char* SOUNDCOMP_SOUNDS = "sounds";
		const char* SOUND_LOOPED = "looped";
		const char* SOUND_PAN = "pan";
		const char* SOUND_PITCH = "ptich";
		const char* SOUND_DELAY = "delay";
		const char* SOUND_NAME = "name";
		const char* SOUND_PATH = "path";
		const char* SOUND_VLEFT = "volumeLeft";
		const char* SOUND_VRIGHT = "volumeRight";
	}

	void Serializer::SerializeSceneGraph(const std::string & path)
	{
		SceneGraphSerializer serializer;
		serializer.Traverse(&AIngine::Editor::Editor::GetSceneGraph()->GetRoot());
		std::string result = serializer.Result.dump();
		std::ofstream file;
		file.open(path);
		file << result;
		file.close();
	}


	void Serializer::DeserializeSceneGraph(const std::string & path)
	{
		using json = nlohmann::json;

		// open the file
		std::ifstream file;
		file.open(path);
		if (file.fail()) return;
		json j = json::parse(file);
		file.close();

		AIngine::GameObject* parent = nullptr;
		AIngine::GameObject * restoredObject = nullptr;

		json* currentJson = &j["Root"];

		std::vector<AIngine::Physics::PhysicsComponent*> physComponents;
		std::vector<json*> openJsons;
		std::unordered_map<json*, GameObject*> spawnedObjects;
		openJsons.push_back(currentJson);

		while (openJsons.size() > 0) {

			currentJson = openJsons[0];

			if (spawnedObjects.size() != 0) {
				parent = spawnedObjects[currentJson];
			}

			if ((*currentJson).contains(AttributeNames::GAMEOBJECT_CHILDREN)) {
				int index = 0;
				for (auto child : (*currentJson)[AttributeNames::GAMEOBJECT_CHILDREN]) {
					std::string name = child.begin().key();
					restoredObject = RestoreGameObject(&child[name], parent);
					// does it have any components?
					if (child[name].contains(AttributeNames::GAMEOBJECT_COMPONENTS)) {
						// restore Sprite
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_SPRITE)) {
							RestoreSprite(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SPRITE], restoredObject);
						}
						// restore Physics
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_PHYSICS)) {
							physComponents.push_back(RestorePhysics(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PHYSICS], restoredObject));
						}
						// restore soundComponent
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_SOUND)) {
							RestorySoundComponent(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SOUND], restoredObject);
						}

						//restore scripts
						RestoreScripts(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SCRIPT], restoredObject);
					}

					// does it have children?
					if (child[name].contains(AttributeNames::GAMEOBJECT_CHILDREN)) {
						openJsons.push_back(&(*currentJson)[AttributeNames::GAMEOBJECT_CHILDREN][index][name]);
						spawnedObjects[&(*currentJson)[AttributeNames::GAMEOBJECT_CHILDREN][index][name]] = restoredObject;
					}
					index++;
				}
			}

			openJsons.erase(openJsons.begin());

		}

		// all physComponetns have been restored, activate them
		for (auto comp : physComponents)
			comp->SetActive(true);
	}

	AIngine::GameObject * Serializer::RestoreGameObject(const nlohmann::json* const j, AIngine::GameObject * parent)
	{
		std::string name = (*j)[AttributeNames::GAMEOBJECT_NAME];
		glm::vec2 position = glm::vec2((*j)[AttributeNames::GAMEOBJECT_POSITION_X], (*j)[AttributeNames::GAMEOBJECT_POSITION_Y]);
		glm::vec2 scale = glm::vec2((*j)[AttributeNames::GAMEOBJECT_SCALE_X], (*j)[AttributeNames::GAMEOBJECT_SCALE_Y]);
		float rot = (*j)[AttributeNames::GAMEOBJECT_ROTATION];

		return AIngine::World::SpawnObject(name, parent, position, scale, rot);
	}

	void Serializer::RestoreSprite(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::Sprite* sprite = obj->AddComponent<AIngine::Sprite>();

		glm::vec2 size = glm::vec2((*j)[AttributeNames::SPRITE_SIZE_X], (*j)[AttributeNames::SPRITE_SIZE_Y]);
		glm::vec3 color = glm::vec3((*j)[AttributeNames::SPRITE_COLOR_R], (*j)[AttributeNames::SPRITE_COLOR_G], (*j)[AttributeNames::SPRITE_COLOR_B]);
		glm::vec2 parallax = glm::vec2((*j)[AttributeNames::SPRITE_PARALLAX_X], (*j)[AttributeNames::SPRITE_PARALLAX_Y]);

		AIngine::Rendering::Texture2D& texture = sprite->GetTexture();

		texture.Wrap_S = (*j)[AttributeNames::TEXTURE_WRAP_S];
		texture.Wrap_T = (*j)[AttributeNames::TEXTURE_WRAP_T];
		texture.Filter_Min = (*j)[AttributeNames::TEXTURE_FILTER_MIN];
		texture.Filter_Max = (*j)[AttributeNames::TEXTURE_FILTER_MAX];
		texture.Image_Format = (*j)[AttributeNames::TEXTURE_IMAGEFORMAT];
		sprite->SetLocalWorldSize(size);
		sprite->SetColor(color);
		sprite->SetParallaxFactor(parallax);

		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>((*j)[AttributeNames::TEXTURE_PATH])->GetBitmap();
		texture.Generate(bitmap);

		sprite->SetActive((*j)[AttributeNames::COMPONENT_ACTIVE]);
	}

	AIngine::Physics::PhysicsComponent* Serializer::RestorePhysics(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		Physics::PhysicsComponent* physComp = obj->AddComponent<Physics::PhysicsComponent>();
		Physics::PhysicsProperties properties;
		Physics::PhysicsShape shape = (*j)[AttributeNames::PHYSICS_SHAPE];
		Physics::PhysicsBodyType type = (*j)[AttributeNames::PHYSICS_BODYTYPE];
		bool isTrigger = (*j)[AttributeNames::PHYSICS_ISTRIGGER];

		properties.density = (*j)[AttributeNames::PHYSICS_DENSITY];
		properties.friction = (*j)[AttributeNames::PHYSICS_FRICTION];
		properties.restitution = (*j)[AttributeNames::PHYSICS_RESTITUTION];

		float width = 0.0f;
		float height = 0.0f;
		float32 radius = 0.0f;

		switch (shape) {
		case(Physics::PhysicsShape::e_Box):
			width = (*j)[AttributeNames::PHYSICS_WIDTH];
			height = (*j)[AttributeNames::PHYSICS_HEIGHT];
			physComp->CreateBoxBody(properties, type, width, height, isTrigger);
			break;
		case(Physics::PhysicsShape::e_Circle):
			radius = (*j)[AttributeNames::PHYSICS_RADIUS];
			physComp->CreateCircleBody(properties, type, radius, isTrigger);
			break;
		case(Physics::PhysicsShape::e_Polygon):
			std::vector<glm::vec2> vertices;
			for (auto& vertex : (*j)[AttributeNames::PHYSICS_VERTICES]) {
				vertices.push_back(glm::vec2(vertex.at("x"), vertex.at("y")));
			}

			int vertexCount = (*j)[AttributeNames::PHYSICS_VERTEXCOUNT];
			physComp->CreatePolygonBody(properties, type, vertices.data(), vertexCount, isTrigger);
			break;
		}

		physComp->SetActive(false);

		return physComp;
	}

	AIngine::SoundComponent * Serializer::RestorySoundComponent(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::SoundComponent* soundComp = obj->AddComponent<AIngine::SoundComponent>();

		for (auto& sound : *j) {
			AIngine::Assets::SoundAsset* soundAsset = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::SoundAsset>(sound.at(AttributeNames::SOUND_PATH));
			AIngine::Sound restoredSound(*soundAsset);
			restoredSound.SetLooping(sound.at(AttributeNames::SOUND_LOOPED));
			restoredSound.SetPan(sound.at(AttributeNames::SOUND_PAN));
			restoredSound.SetPitch(sound.at(AttributeNames::SOUND_PITCH));
			restoredSound.SetDelay(sound.at(AttributeNames::SOUND_DELAY));
			float left = sound.at(AttributeNames::SOUND_VLEFT);
			float right = sound.at(AttributeNames::SOUND_VRIGHT);
			restoredSound.SetVolume(left, right);
			soundComp->AddSound(restoredSound);
		}

		soundComp->SetActive((*j)[AttributeNames::COMPONENT_ACTIVE]);

		return soundComp;
	}

	void Serializer::RestoreScripts(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		for (auto index : (*j)) {
			AIngine::OnAddComponent(obj, index);
		}
	}


	/* -------------------------------------------- SCENEGRAPH SERIALIZER -------------------------------------------------------*/

	bool SceneGraphSerializer::Traverse(GameObject * root)
	{
		nlohmann::json serializedObject = SerializeGameObject(*root);
		Result = serializedObject;
		m_prevChildren = nullptr;
		m_children = &Result[root->GetName()]["c_children"];

		bool result = root->Accept(*this);

		return result;
	}

	bool SceneGraphSerializer::Enter(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		if (node.GetName() != "Root") {
			(*m_children).push_back(serializedObject);
			m_prevChildren = m_children;
			size_t count = (*m_children).size();
			m_children = &(*m_children)[count - 1][node.GetName()]["c_children"];
		}

		return true;
	}

	bool SceneGraphSerializer::Leave(GameObject & node)
	{
		if (m_prevChildren) {
			m_children = m_prevChildren;
		}
		return true;
	}

	bool SceneGraphSerializer::Visit(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		(*m_children).push_back(serializedObject);

		return true;
	}

	nlohmann::json SceneGraphSerializer::SerializeScripts(GameObject & obj)
	{
		nlohmann::json outer;

		for (auto& it = obj.GetComponents().begin(); it != obj.GetComponents().end(); it++) {
			AIngine::Script* script = dynamic_cast<AIngine::Script *>((*it._Ptr));
			if (script) {
				outer.push_back(script->ScriptIndex);
			}
		}

		return outer;
	}

	nlohmann::json SceneGraphSerializer::SerializeGameObject(GameObject & obj)
	{
		nlohmann::json outer;
		nlohmann::json j;

		j[AttributeNames::GAMEOBJECT_NAME] = obj.GetName();
		j[AttributeNames::GAMEOBJECT_POSITION_X] = obj.GetLocalPosition().x;
		j[AttributeNames::GAMEOBJECT_POSITION_Y] = obj.GetLocalPosition().y;
		j[AttributeNames::GAMEOBJECT_SCALE_X] = obj.GetLocalScale().x;
		j[AttributeNames::GAMEOBJECT_SCALE_Y] = obj.GetLocalScale().y;
		j[AttributeNames::GAMEOBJECT_ROTATION] = obj.GetLocalRotation();

		// serialize sprite
		AIngine::Sprite* sprite = obj.GetComponent<AIngine::Sprite>();
		if (sprite) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SPRITE] = SerializeSprite(*sprite);
		}

		// serialize physComp
		Physics::PhysicsComponent* physComp = obj.GetComponent<Physics::PhysicsComponent>();
		if (physComp) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PHYSICS] = SerializePhysicsComponent(*physComp);
		}

		AIngine::SoundComponent* soundComp = obj.GetComponent<AIngine::SoundComponent>();
		if (soundComp) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SOUND] = SerializeSoundComponent(*soundComp);
		}

		j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SCRIPT] = SerializeScripts(obj);

		outer[obj.GetName()] = j;
		return outer;
	}

	nlohmann::json SceneGraphSerializer::SerializeWorld()
	{
		nlohmann::json j;
		glm::vec4 bounds = AIngine::World::GetBounds();
		j[AttributeNames::WORLD_BOTTOMLEFT] = bounds.x;
		j[AttributeNames::WORLD_BOTTOMRIGHT] = bounds.y;
		j[AttributeNames::WORLD_TOPLEFT] = bounds.z;
		j[AttributeNames::WORLD_TOPRIGHT] = bounds.w;
		j[AttributeNames::WORLD_GRAVITY_X] = AIngine::World::GetGravity().x;
		j[AttributeNames::WORLD_GRAVITY_Y] = AIngine::World::GetGravity().y;
		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializeSprite(AIngine::Sprite& sprite)
	{
		nlohmann::json j;

		j[AttributeNames::TEXTURE_PATH] = std::filesystem::relative(sprite.GetTexture().FileName).string();
		j[AttributeNames::SPRITE_COLOR_R] = sprite.GetColor().x;
		j[AttributeNames::SPRITE_COLOR_G] = sprite.GetColor().y;
		j[AttributeNames::SPRITE_COLOR_B] = sprite.GetColor().z;
		j[AttributeNames::SPRITE_SIZE_X] = sprite.GetLocalWorldSize().x;
		j[AttributeNames::SPRITE_SIZE_Y] = sprite.GetLocalWorldSize().y;
		j[AttributeNames::SPRITE_PARALLAX_X] = sprite.GetParallaxFactor().x;
		j[AttributeNames::SPRITE_PARALLAX_Y] = sprite.GetParallaxFactor().y;
		j[AttributeNames::TEXTURE_WRAP_S] = sprite.GetTexture().Wrap_S;
		j[AttributeNames::TEXTURE_WRAP_T] = sprite.GetTexture().Wrap_T;
		j[AttributeNames::TEXTURE_FILTER_MIN] = sprite.GetTexture().Filter_Min;
		j[AttributeNames::TEXTURE_FILTER_MAX] = sprite.GetTexture().Filter_Max;
		j[AttributeNames::TEXTURE_IMAGEFORMAT] = sprite.GetTexture().Image_Format;
		j[AttributeNames::COMPONENT_ACTIVE] = sprite.IsActive();

		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializePhysicsComponent(Physics::PhysicsComponent & physComp)
	{
		nlohmann::json j;

		const Physics::PhysicsBodyInformation& bodyInfo = physComp.GetBodyInformation();
		const Physics::PhysicsProperties& properties = physComp.GetProperties();

		if (bodyInfo.shape == Physics::PhysicsShape::e_Box) {
			j[AttributeNames::PHYSICS_WIDTH] = bodyInfo.width;
			j[AttributeNames::PHYSICS_HEIGHT] = bodyInfo.height;
		}
		else if (bodyInfo.shape == Physics::PhysicsShape::e_Circle) {
			j[AttributeNames::PHYSICS_RADIUS] = bodyInfo.radius;
		}
		// polygon
		else {
		}

		j[AttributeNames::PHYSICS_SHAPE] = bodyInfo.shape;
		j[AttributeNames::PHYSICS_BODYTYPE] = bodyInfo.type;
		j[AttributeNames::PHYSICS_ISTRIGGER] = bodyInfo.isTrigger;
		j[AttributeNames::PHYSICS_VERTICES] = bodyInfo.vertices;
		j[AttributeNames::PHYSICS_VERTEXCOUNT] = bodyInfo.verticesCount;
		j[AttributeNames::PHYSICS_DENSITY] = properties.density;
		j[AttributeNames::PHYSICS_FRICTION] = properties.friction;
		j[AttributeNames::PHYSICS_RESTITUTION] = properties.restitution;
		j[AttributeNames::COMPONENT_ACTIVE] = physComp.IsActive();


		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializeSoundComponent(AIngine::SoundComponent & soundComp)
	{
		nlohmann::json outer;

		for (auto& it = soundComp.GetSounds().begin(); it != soundComp.GetSounds().end(); it++)
		{
			nlohmann::json j;
			const AIngine::Sound& sound = *it._Ptr;
			j[AttributeNames::SOUND_DELAY] = sound.GetDelay();
			j[AttributeNames::SOUND_LOOPED] = sound.IsLooping();
			j[AttributeNames::SOUND_PATH] = std::filesystem::relative(sound.GetPath()).string();
			j[AttributeNames::SOUND_PAN] = sound.GetPan();
			j[AttributeNames::SOUND_PITCH] = sound.GetPitch();
			j[AttributeNames::SOUND_VLEFT] = sound.GetVolume().first;
			j[AttributeNames::SOUND_VRIGHT] = sound.GetVolume().second;
			outer[sound.GetName()] = j;
		}

		outer[AttributeNames::COMPONENT_ACTIVE] = soundComp.IsActive();

		return outer;
	}

}

namespace glm
{
	void to_json(nlohmann::json& j, const glm::vec2& vec) {
		j = nlohmann::json{ {"x",vec.x,}, {"y",vec.y} };
	}

	void from_json(const nlohmann::json& j, glm::vec2& vec) {
		j.at("x").get_to(vec.x);
		j.at("y").get_to(vec.y);
	}
}
