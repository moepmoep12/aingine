#include "Editor/Serialization.h"
#include "Editor/Editor.h"
#include "Rendering/texture.h"
#include "AIngine/Physics.h"
#include "AIngine/GameObject.h"
#include "Structures/SceneGraph.h"
#include "AIngine/World.h"
#include "Assets/Assets.h"

#include <fstream>
#include <vector>
#include <glm/glm.hpp>

namespace AIngine::Editor::Serialization {

	namespace AttributeNames {

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
		const char* COMPONENT_TEXTURE2D = "texture";
		const char* COMPONENT_PHYSICS = "physics";

		// Texture2D
		const char* TEXTURE_PATH = "path";
		const char* TEXTURE_COLOR_R = "colorR";
		const char* TEXTURE_COLOR_G = "colorG";
		const char* TEXTURE_COLOR_B = "colorB";
		const char* TEXTURE_SIZE_X = "sizeX";
		const char* TEXTURE_SIZE_Y = "sizeY";
		const char* TEXTURE_PARALLAX_X = "parallaxX";
		const char* TEXTURE_PARALLAX_Y = "parallaxY";
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
						// restore Texture2D
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_TEXTURE2D)) {
							RestoreTexture2D(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_TEXTURE2D], restoredObject);
						}
						// restore Physics
						if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_PHYSICS)) {
							physComponents.push_back(RestorePhysics(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PHYSICS], restoredObject));
						}
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

	void Serializer::RestoreTexture2D(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		AIngine::Rendering::Texture2D* texture = obj->AddComponent<AIngine::Rendering::Texture2D>();

		glm::vec2 size = glm::vec2((*j)[AttributeNames::TEXTURE_SIZE_X], (*j)[AttributeNames::TEXTURE_SIZE_Y]);
		glm::vec3 color = glm::vec3((*j)[AttributeNames::TEXTURE_COLOR_R], (*j)[AttributeNames::TEXTURE_COLOR_G], (*j)[AttributeNames::TEXTURE_COLOR_B]);
		glm::vec2 parallax = glm::vec2((*j)[AttributeNames::TEXTURE_PARALLAX_X], (*j)[AttributeNames::TEXTURE_PARALLAX_Y]);

		texture->Wrap_S = (*j)[AttributeNames::TEXTURE_WRAP_S];
		texture->Wrap_T = (*j)[AttributeNames::TEXTURE_WRAP_T];
		texture->Filter_Min = (*j)[AttributeNames::TEXTURE_FILTER_MIN];
		texture->Filter_Max = (*j)[AttributeNames::TEXTURE_FILTER_MAX];
		texture->Image_Format = (*j)[AttributeNames::TEXTURE_IMAGEFORMAT];
		texture->SetLocalWorldSize(size);
		texture->SetColor(color);
		texture->SetParallaxFactor(parallax);

		AIngine::Rendering::Bitmap& bitmap = AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>((*j)[AttributeNames::TEXTURE_PATH])->GetBitmap();
		texture->Generate(bitmap);
	}

	AIngine::Physics::PhysicsComponent* Serializer::RestorePhysics(const nlohmann::json * const j, AIngine::GameObject * obj)
	{
		Physics::PhysicsComponent* physComp = obj->AddComponent<Physics::PhysicsComponent>();
		Physics::PhysicsProperties properties;
		Physics::PhysicsShape shape = (*j)[AttributeNames::PHYSICS_SHAPE];
		Physics::PhysicsBodyType type = (*j)[AttributeNames::PHYSICS_BODYTYPE];

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
			physComp->CreateBoxBody(properties, type, width, height);
			break;
		case(Physics::PhysicsShape::e_Circle):
			radius = (*j)[AttributeNames::PHYSICS_RADIUS];
			physComp->CreateCircleBody(properties, type, radius);
			break;
		case(Physics::PhysicsShape::e_Polygon):
			break;
		}

		physComp->SetActive(false);

		return physComp;
	}


	/* -------------------------------------------- SCENEGRAPH SERIALIZER -------------------------------------------------------*/

	bool SceneGraphSerializer::Traverse(GameObject * root)
	{
		bool result = root->Accept(*this);

		return result;
	}

	bool SceneGraphSerializer::Enter(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		// root
		if (!node.GetParent()) {
			Result = serializedObject;
			m_children = &Result[node.GetName()]["c_children"];
		}
		else {
			(*m_children).push_back(serializedObject);
			int count = (*m_children).size();
			m_children = &(*m_children)[count - 1][node.GetName()]["c_children"];
		}

		return true;
	}

	bool SceneGraphSerializer::Leave(GameObject & node)
	{
		return true;
	}

	bool SceneGraphSerializer::Visit(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		(*m_children).push_back(serializedObject);

		return true;
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

		// serialize texture
		AIngine::Rendering::Texture2D* texture = obj.GetComponent<AIngine::Rendering::Texture2D>();
		if (texture) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_TEXTURE2D] = SerializeTexture2D(*texture);
		}

		// serialize physComp
		Physics::PhysicsComponent* physComp = obj.GetComponent<Physics::PhysicsComponent>();
		if (physComp) {
			j[AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_PHYSICS] = SerializePhysicsComponent(*physComp);
		}

		outer[obj.GetName()] = j;
		return outer;
	}

	nlohmann::json SceneGraphSerializer::SerializeTexture2D(AIngine::Rendering::Texture2D & texture)
	{
		nlohmann::json j;

		j[AttributeNames::TEXTURE_PATH] = texture.GetName();
		j[AttributeNames::TEXTURE_COLOR_R] = texture.GetColor().x;
		j[AttributeNames::TEXTURE_COLOR_G] = texture.GetColor().y;
		j[AttributeNames::TEXTURE_COLOR_B] = texture.GetColor().z;
		j[AttributeNames::TEXTURE_SIZE_X] = texture.GetLocalWorldSize().x;
		j[AttributeNames::TEXTURE_SIZE_Y] = texture.GetLocalWorldSize().y;
		j[AttributeNames::TEXTURE_PARALLAX_X] = texture.GetParallaxFactor().x;
		j[AttributeNames::TEXTURE_PARALLAX_Y] = texture.GetParallaxFactor().y;
		j[AttributeNames::TEXTURE_WRAP_S] = texture.Wrap_S;
		j[AttributeNames::TEXTURE_WRAP_T] = texture.Wrap_T;
		j[AttributeNames::TEXTURE_FILTER_MIN] = texture.Filter_Min;
		j[AttributeNames::TEXTURE_FILTER_MAX] = texture.Filter_Max;
		j[AttributeNames::TEXTURE_IMAGEFORMAT] = texture.Image_Format;

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
		j[AttributeNames::PHYSICS_DENSITY] = properties.density;
		j[AttributeNames::PHYSICS_FRICTION] = properties.friction;
		j[AttributeNames::PHYSICS_RESTITUTION] = properties.restitution;

		return j;
	}

}
