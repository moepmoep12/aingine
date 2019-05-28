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

		AIngine::GameObject* parent;


		json* jj = &j["Root"];

		// create Root
		parent = RestoreGameObject(jj, nullptr);


		/*	while (jj) {*/

		if ((*jj).contains(AttributeNames::GAMEOBJECT_CHILDREN)) {
			for (auto child : (*jj)[AttributeNames::GAMEOBJECT_CHILDREN]) {
				std::string name = child.begin().key();
				AIngine::GameObject * restoredObject = RestoreGameObject(&child[name], parent);
				if (child[name].contains(AttributeNames::GAMEOBJECT_COMPONENTS)) {
					if (child[name][AttributeNames::GAMEOBJECT_COMPONENTS].contains(AttributeNames::COMPONENT_TEXTURE2D)) {
						RestoreTexture2D(&child[name][AttributeNames::GAMEOBJECT_COMPONENTS][AttributeNames::COMPONENT_TEXTURE2D], restoredObject);
					}
				}
			}
		}


		//}
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
		AIngine::PhysicsComponent* physComp = obj.GetComponent<AIngine::PhysicsComponent>();
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

	nlohmann::json SceneGraphSerializer::SerializePhysicsComponent(AIngine::PhysicsComponent & physComp)
	{
		return nlohmann::json();
	}

}
