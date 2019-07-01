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
#include "AIngine/ParticleEmitter.h"
#include "AIngine/Script.h"
#include "UI/UIELement.h"
#include "UI/Button.h"
#include "Rendering/UIRenderer.h"
#include "Util/Project.h"

#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <filesystem>
#include <algorithm>

namespace AIngine::Editor::Serialization {

	namespace AttributeNames {

		// world
		const char* WORLD_GRAVITY_X = "gravityX";
		const char* WORLD_GRAVITY_Y = "gravityY";
		const char* WORLD_BOTTOMLEFT = "bottomLeft";
		const char* WORLD_BOTTOMRIGHT = "bottomRight";
		const char* WORLD_TOPLEFT = "topleft";
		const char* WORLD_TOPRIGHT = "topRight";

		// Editor settings
		const char* EDITOR_PHYSRENDERING = "physrendering";
		const char* EDITOR_SHOWFPS = "showFPS";
		const char* EDITOR_SHOWFPSGRAPH = "showFPSGraph";
		const char* EDITOR_OUTLINEANIM = "outlineAnimation";

		// Scene
		const char* SCENE_NAME = "name";
		const char* SCENE_PATH = "path";

		// GameObject
		const char* GAMEOBJECT_ACTIVE = "a_active";
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
		const char* COMPONENT_PARTICLEEMITTER = "particleemitter";
		const char* COMPONENT_CANVAS = "canvas";
		const char* COMPONENT_BUTTON = "button";

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

		// ParticleEmitter
		const char* PARTICLEEMITTER_AMOUNT = "amount";

		// UIElement
		const char* UIELEMENT_RECT = "rectangle";
		const char* UIELEMENT_ISDISABLED = "isdisabled";
		const char* UIELEMENT_COLORTINT = "tintColor";
		const char* UIELEMENT_COLORDISABLED = "disabledColor";

		// Button
		const char* BUTTON_COLORCLICKED = "colorClicked";
		const char* BUTTON_COLORHOVERED = "colorHovered";
		const char* BUTTON_TEXT = "text";
		const char* BUTTON_TEXTSCALE = "textScale";
		const char* BUTTON_TEXTOFFSET = "textOffset";
		const char* BUTTON_TEXTCOLOR = "textColor";

	}

	void Serializer::SerializeSceneGraph(const std::string & path)
	{
		SceneGraphSerializer serializer;
		serializer.Traverse(&AIngine::Editor::Editor::GetSceneGraph()->GetRoot());
		std::string result = serializer.Result.dump(0);
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
					restoredObject->SetActive(child[name][AttributeNames::GAMEOBJECT_ACTIVE]);
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
						// restore particleEmitter
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_PARTICLEEMITTER)) {
							RestoreParticleEmitter(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PARTICLEEMITTER], restoredObject);
						}
						// restore canvas
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_CANVAS)) {
							RestoreCanvas(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_CANVAS], restoredObject);
						}
						// restore button
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_BUTTON)) {
							RestoreButton(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_BUTTON], restoredObject);
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
			comp->SetEnabled(true);
	}

	static const char* editorSettingsPath = "Editor/Settings.ini";

	void Serializer::SaveEditorSettings()
	{
		nlohmann::json j;
		j[AttributeNames::EDITOR_PHYSRENDERING] = AIngine::World::IsDebugPhysicsDrawn();
		j[AttributeNames::EDITOR_SHOWFPSGRAPH] = AIngine::Editor::Editor::IsFpsGraphVisible();
		j[AttributeNames::EDITOR_SHOWFPS] = AIngine::Editor::Editor::IsFramerateDisplayed();
		std::string result = j.dump();
		std::ofstream file;
		file.open(editorSettingsPath);
		file << result;
		file.close();
	}

	void Serializer::LoadEditorSettings()
	{
		// open the file
		std::ifstream file;
		file.open(editorSettingsPath);
		if (file.fail()) return;
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();

		AIngine::World::SetPhysicsDebugDrawActive(j[AttributeNames::EDITOR_PHYSRENDERING]);
		AIngine::Editor::Editor::SetShowFpsGraph(j[AttributeNames::EDITOR_SHOWFPSGRAPH]);
		AIngine::Editor::Editor::SetShowFramerate(j[AttributeNames::EDITOR_SHOWFPS]);
	}

	static const std::string s_BuildScenesPath = "Editor/ScenesBuild.json";

	std::vector<Scene> Serializer::LoadBuildScenes()
	{
		std::vector<Scene> scenes;

		// open the file
		std::ifstream file;
		file.open(s_BuildScenesPath);
		if (file.fail()) return scenes;
		nlohmann::json j = nlohmann::json::parse(file);
		file.close();


		for (auto& scene : j) {
			scenes.push_back(Scene{
				scene.at(AttributeNames::SCENE_NAME),
				scene.at(AttributeNames::SCENE_PATH)
				});
		}

		return scenes;
	}

	void Serializer::SaveBuildScenes(const std::vector<Scene>& scenes)
	{
		nlohmann::json outer;

		for (auto& scene : scenes) {
			nlohmann::json j;
			j[AttributeNames::SCENE_NAME] = scene.Name;
			j[AttributeNames::SCENE_PATH] = std::filesystem::canonical(scene.Path).string();
			outer.push_back(j);
		}

		std::string result = outer.dump();
		std::ofstream file;
		file.open(s_BuildScenesPath);
		file << result;
		file.close();
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
		sprite->SetColor(glm::vec4(color, (*j)[AttributeNames::SPRITE_ALPHA]));
		sprite->SetParallaxFactor(parallax);

		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>((*j)[AttributeNames::TEXTURE_PATH])->GetBitmap();
		texture.Generate(bitmap);

		sprite->SetEnabled((*j)[AttributeNames::COMPONENT_ACTIVE]);
	}

	void Serializer::RestoreParticleEmitter(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::ParticleEmitter* emitter = obj->AddComponent<AIngine::ParticleEmitter>();
		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>((*j)[AttributeNames::TEXTURE_PATH])->GetBitmap();
		AIngine::Rendering::Texture2D& texture = emitter->GetTexture();

		texture.Wrap_S = (*j)[AttributeNames::TEXTURE_WRAP_S];
		texture.Wrap_T = (*j)[AttributeNames::TEXTURE_WRAP_T];
		texture.Filter_Min = (*j)[AttributeNames::TEXTURE_FILTER_MIN];
		texture.Filter_Max = (*j)[AttributeNames::TEXTURE_FILTER_MAX];
		texture.Image_Format = (*j)[AttributeNames::TEXTURE_IMAGEFORMAT];
		texture.Generate(bitmap);

		emitter->SetAmount((*j)[AttributeNames::PARTICLEEMITTER_AMOUNT]);

		emitter->SetEnabled((*j)[AttributeNames::COMPONENT_ACTIVE]);
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
		std::vector<glm::vec2> vertices;
		int vertexCount = 0;

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

		case Physics::PhysicsShape::e_Edge:
			for (auto& vertex : (*j)[AttributeNames::PHYSICS_VERTICES]) {
				vertices.push_back(glm::vec2(vertex.at("x"), vertex.at("y")));
			}
			vertexCount = (*j)[AttributeNames::PHYSICS_VERTEXCOUNT];
			physComp->CreateEdgeBody(properties, type, vertices[0], vertices[1], isTrigger);
			break;

		case(Physics::PhysicsShape::e_Polygon):
			for (auto& vertex : (*j)[AttributeNames::PHYSICS_VERTICES]) {
				vertices.push_back(glm::vec2(vertex.at("x"), vertex.at("y")));
			}

			vertexCount = (*j)[AttributeNames::PHYSICS_VERTEXCOUNT];
			physComp->CreatePolygonBody(properties, type, vertices.data(), vertexCount, isTrigger);
			break;
		}

		physComp->SetEnabled(false);

		return physComp;
	}

	AIngine::SoundComponent * Serializer::RestorySoundComponent(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::SoundComponent* soundComp = obj->AddComponent<AIngine::SoundComponent>();

		for (auto& sound : *j) {
			if (sound.contains(AttributeNames::SOUND_PATH)) {
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
		}

		soundComp->SetEnabled((*j)[AttributeNames::COMPONENT_ACTIVE]);

		return soundComp;
	}

	void Serializer::RestoreScripts(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		for (auto index : (*j)) {
			AIngine::OnAddComponent(obj, index);
		}
	}

	void Serializer::RestoreCanvas(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::UI::Canvas* canvas = obj->AddComponent<AIngine::UI::Canvas>();
		canvas->SetRectangle((*j)[AttributeNames::UIELEMENT_RECT]);
		AIngine::Rendering::UIRenderer::canvas = obj;
	}

	void Serializer::RestoreButton(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::UI::Button* button = obj->AddComponent<AIngine::UI::Button>();
		button->SetRectangle((*j)[AttributeNames::UIELEMENT_RECT]);
		button->ClickedColor = (*j)[AttributeNames::BUTTON_COLORCLICKED];
		button->DisabledColor = (*j)[AttributeNames::UIELEMENT_COLORDISABLED];
		button->SetDisabled((*j)[AttributeNames::UIELEMENT_ISDISABLED]);
		button->HoverColor = (*j)[AttributeNames::BUTTON_COLORHOVERED];
		button->SetEnabled((*j)[AttributeNames::COMPONENT_ACTIVE]);
		button->Text = (*j)[AttributeNames::BUTTON_TEXT];
		button->TintColor = (*j)[AttributeNames::UIELEMENT_COLORTINT];
		button->TextOffset = (*j)[AttributeNames::BUTTON_TEXTOFFSET];
		button->TextScale = (*j)[AttributeNames::BUTTON_TEXTSCALE];
		button->TextColor = (*j)[AttributeNames::BUTTON_TEXTCOLOR];

		AIngine::Rendering::Texture2D& texture = button->Texture;

		texture.Wrap_S = (*j)[AttributeNames::TEXTURE_WRAP_S];
		texture.Wrap_T = (*j)[AttributeNames::TEXTURE_WRAP_T];
		texture.Filter_Min = (*j)[AttributeNames::TEXTURE_FILTER_MIN];
		texture.Filter_Max = (*j)[AttributeNames::TEXTURE_FILTER_MAX];
		texture.Image_Format = (*j)[AttributeNames::TEXTURE_IMAGEFORMAT];
		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>((*j)[AttributeNames::TEXTURE_PATH])->GetBitmap();
		texture.Generate(bitmap);
	}


	/* -------------------------------------------- SCENEGRAPH SERIALIZER -------------------------------------------------------*/

	bool SceneGraphSerializer::Traverse(GameObject * root)
	{
		nlohmann::json serializedObject = SerializeGameObject(*root);
		Result = serializedObject;
		//m_prevChildren = nullptr;
		m_children = &Result[root->GetName()]["c_children"];

		bool result = root->Accept(*this);

		return result;
	}

	bool SceneGraphSerializer::Enter(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		if (node.GetName() != "Root") {
			(*m_children).push_back(serializedObject);
			m_prevChildren.push_back(m_children);
			size_t count = (*m_children).size();
			m_children = &(*m_children)[count - 1][node.GetName()]["c_children"];
		}

		return true;
	}

	bool SceneGraphSerializer::Leave(GameObject & node)
	{
		if (m_prevChildren.size() > 0) {
			m_children = m_prevChildren.back();
			m_prevChildren.pop_back();
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

		j[AttributeNames::GAMEOBJECT_ACTIVE] = obj.IsActive();
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

		// serialize soundcomp
		AIngine::SoundComponent* soundComp = obj.GetComponent<AIngine::SoundComponent>();
		if (soundComp) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SOUND] = SerializeSoundComponent(*soundComp);
		}

		// serialize emitter
		AIngine::ParticleEmitter* emitter = obj.GetComponent<AIngine::ParticleEmitter>();
		if (emitter) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PARTICLEEMITTER] = SerializeParticleEmitter(*emitter);
		}

		// serialize canvas
		AIngine::UI::Canvas* canvas = obj.GetComponent<AIngine::UI::Canvas>();
		if (canvas) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_CANVAS] = SerializeCanvas(*canvas);
		}

		// serialize button
		AIngine::UI::Button* button = obj.GetComponent<AIngine::UI::Button>();
		if (button) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_BUTTON] = SerializeButton(*button);
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

		j[AttributeNames::SPRITE_COLOR_R] = sprite.GetColor().x;
		j[AttributeNames::SPRITE_COLOR_G] = sprite.GetColor().y;
		j[AttributeNames::SPRITE_COLOR_B] = sprite.GetColor().z;
		j[AttributeNames::SPRITE_ALPHA] = sprite.GetColor().w;
		j[AttributeNames::SPRITE_SIZE_X] = sprite.GetLocalWorldSize().x;
		j[AttributeNames::SPRITE_SIZE_Y] = sprite.GetLocalWorldSize().y;
		j[AttributeNames::SPRITE_PARALLAX_X] = sprite.GetParallaxFactor().x;
		j[AttributeNames::SPRITE_PARALLAX_Y] = sprite.GetParallaxFactor().y;
		j[AttributeNames::TEXTURE_PATH] = SerializePath(sprite.GetTexture().FileName);
		j[AttributeNames::TEXTURE_WRAP_S] = sprite.GetTexture().Wrap_S;
		j[AttributeNames::TEXTURE_WRAP_T] = sprite.GetTexture().Wrap_T;
		j[AttributeNames::TEXTURE_FILTER_MIN] = sprite.GetTexture().Filter_Min;
		j[AttributeNames::TEXTURE_FILTER_MAX] = sprite.GetTexture().Filter_Max;
		j[AttributeNames::TEXTURE_IMAGEFORMAT] = sprite.GetTexture().Image_Format;
		j[AttributeNames::COMPONENT_ACTIVE] = sprite.IsEnabled();
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

		// polygon or edge
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
		j[AttributeNames::COMPONENT_ACTIVE] = physComp.IsEnabled();


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
			j[AttributeNames::SOUND_PATH] = SerializePath(sound.GetPath());
			j[AttributeNames::SOUND_PAN] = sound.GetPan();
			j[AttributeNames::SOUND_PITCH] = sound.GetPitch();
			j[AttributeNames::SOUND_VLEFT] = sound.GetVolume().first;
			j[AttributeNames::SOUND_VRIGHT] = sound.GetVolume().second;
			outer[sound.GetName()] = j;
		}

		outer[AttributeNames::COMPONENT_ACTIVE] = soundComp.IsEnabled();

		return outer;
	}

	nlohmann::json SceneGraphSerializer::SerializeParticleEmitter(AIngine::ParticleEmitter & emitter)
	{
		nlohmann::json j;
		j[AttributeNames::COMPONENT_ACTIVE] = emitter.IsEnabled();
		j[AttributeNames::TEXTURE_PATH] = SerializePath(emitter.GetTexture().FileName);
		j[AttributeNames::TEXTURE_WRAP_S] = emitter.GetTexture().Wrap_S;
		j[AttributeNames::TEXTURE_WRAP_T] = emitter.GetTexture().Wrap_T;
		j[AttributeNames::TEXTURE_FILTER_MIN] = emitter.GetTexture().Filter_Min;
		j[AttributeNames::TEXTURE_FILTER_MAX] = emitter.GetTexture().Filter_Max;
		j[AttributeNames::TEXTURE_IMAGEFORMAT] = emitter.GetTexture().Image_Format;
		j[AttributeNames::PARTICLEEMITTER_AMOUNT] = emitter.GetAmount();

		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializeCanvas(AIngine::UI::Canvas & canvas)
	{
		nlohmann::json j;
		j[AttributeNames::UIELEMENT_RECT] = canvas.GetRectangle();
		j[AttributeNames::COMPONENT_ACTIVE] = canvas.IsEnabled();

		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializeButton(AIngine::UI::Button & button)
	{
		nlohmann::json j;
		j[AttributeNames::UIELEMENT_RECT] = button.GetRectangle();
		j[AttributeNames::UIELEMENT_COLORDISABLED] = button.DisabledColor;
		j[AttributeNames::UIELEMENT_COLORTINT] = button.TintColor;
		j[AttributeNames::UIELEMENT_ISDISABLED] = button.IsDisabled();
		j[AttributeNames::BUTTON_COLORCLICKED] = button.ClickedColor;
		j[AttributeNames::BUTTON_COLORHOVERED] = button.HoverColor;
		j[AttributeNames::BUTTON_TEXTOFFSET] = button.TextOffset;
		j[AttributeNames::BUTTON_TEXTSCALE] = button.TextScale;
		j[AttributeNames::BUTTON_TEXTCOLOR] = button.TextColor;
		j[AttributeNames::BUTTON_TEXT] = button.Text;
		j[AttributeNames::TEXTURE_PATH] = SerializePath(button.Texture.FileName);
		j[AttributeNames::TEXTURE_WRAP_S] = button.Texture.Wrap_S;
		j[AttributeNames::TEXTURE_WRAP_T] = button.Texture.Wrap_T;
		j[AttributeNames::TEXTURE_FILTER_MIN] = button.Texture.Filter_Min;
		j[AttributeNames::TEXTURE_FILTER_MAX] = button.Texture.Filter_Max;
		j[AttributeNames::TEXTURE_IMAGEFORMAT] = button.Texture.Image_Format;
		j[AttributeNames::COMPONENT_ACTIVE] = button.IsEnabled();
		return j;
	}

	std::string SceneGraphSerializer::SerializePath(const std::string & path)
	{
		// it's an Engine resource and thus relative to the install dir
		if (path.find("AIngine") != std::string::npos) {
			return std::filesystem::relative(path, AIngine::Util::Project::GetEngineInstallDirectory() + "\\" + "Resources").string();
		}
		else return std::filesystem::relative(path, AIngine::Util::Project::GetResourceDirectory()).string();

		return std::string();
	}

	std::vector<std::string> ExtractPathsFromScene(const std::string & sceneFilePath)
	{
		std::vector<std::string> result;

		using json = nlohmann::json;

		// open the file
		std::ifstream file;
		file.open(sceneFilePath);
		if (file.fail()) return result;
		json j = json::parse(file);
		file.close();


		json* currentJson = &j["Root"];
		std::vector<json*> openJsons;
		openJsons.push_back(currentJson);

		while (openJsons.size() > 0) {

			currentJson = openJsons[0];

			if ((*currentJson).contains(AttributeNames::GAMEOBJECT_CHILDREN)) {
				int index = 0;
				for (auto child : (*currentJson)[AttributeNames::GAMEOBJECT_CHILDREN]) {
					std::string name = child.begin().key();
					// does it have any components?
					if (child[name].contains(AttributeNames::GAMEOBJECT_COMPONENTS)) {
						// texturepath
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_SPRITE)) {
							std::string path = child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SPRITE][AttributeNames::TEXTURE_PATH];
							if (std::find(result.begin(), result.end(), path) == result.end())
								result.push_back(path);
						}
						// sound path
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_SOUND)) {

							for (auto& sound : child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_SOUND]) {
								if (sound.contains(AttributeNames::SOUND_PATH)) {
									std::string path = sound.at(AttributeNames::SOUND_PATH);
									if (std::find(result.begin(), result.end(), path) == result.end())
										result.push_back(path);
								}
							}

						}
						// texturepath
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_PARTICLEEMITTER)) {
							std::string path = child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PARTICLEEMITTER][AttributeNames::TEXTURE_PATH];
							if (std::find(result.begin(), result.end(), path) == result.end())
								result.push_back(path);
						}
					}

					// does it have children?
					if (child[name].contains(AttributeNames::GAMEOBJECT_CHILDREN)) {
						openJsons.push_back(&(*currentJson)[AttributeNames::GAMEOBJECT_CHILDREN][index][name]);
					}
					index++;
				}
			}

			openJsons.erase(openJsons.begin());

		}
		return result;
	}

}

