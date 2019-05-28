#include "Editor/Serialization.h"
#include "Editor/Editor.h"
#include "Rendering/texture.h"
#include "AIngine/Physics.h"
#include "AIngine/GameObject.h"
#include "Structures/SceneGraph.h"
#include "AIngine/World.h"

#include <fstream>
#include <vector>

namespace AIngine::Editor::Serialization {

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

	static 	std::vector<std::pair<AIngine::GameObject*, int>> spawnedObjects;
	static std::ofstream logfile;
	static bool startingGameObject = false;

	AIngine::GameObject * Serializer::DeserializeSceneGraph(const std::string & path)
	{
		using World = AIngine::World;
		using json = nlohmann::json;

		// open the file
		std::ifstream file;
		file.open(path);

		logfile.open("logJson.txt");

		// callback function for parsing the string
		json::parser_callback_t cb = [](int depth, json::parse_event_t event, json & parsed)
		{
			logfile << depth << "    ";

			if (event == json::parse_event_t::object_start)
			{
				logfile << "object_start    " << parsed << "\n";

				// we know we're starting a gameobject, so listen for the next key
				if (parsed != json("a_localRot") && parsed != json("ParallaxX")) {
					startingGameObject = true;
				}

				return true;
			}
			if (event == json::parse_event_t::object_end)
			{
				logfile << "object_end    " << parsed << "\n";

				// we have completed a GameObject
				if (depth == spawnedObjects[spawnedObjects.size()].second) {

				}

				return true;
			}

			if (event == json::parse_event_t::key)
			{
				logfile << "key    " << parsed << "\n";

				if (startingGameObject) {
					spawnedObjects.push_back({ AIngine::World::SpawnObject(parsed),depth });
					startingGameObject = false;
				}

				return true;
			}

			if (event == json::parse_event_t::array_start)
			{
				logfile << "array_start    " << parsed << "\n";

				return true;
			}

			if (event == json::parse_event_t::array_end)
			{
				logfile << "array_end    " << parsed << "\n";

				return true;
			}

			if (event == json::parse_event_t::value)
			{
				logfile << "value    " << parsed << "\n";

				return true;
			}

			return true;
		};

		json j = json::parse(file, cb);
		file.close();
		logfile.close();


		return nullptr;
	}

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

		j["a_name"] = obj.GetName();
		j["a_localposX"] = obj.GetLocalPosition().x;
		j["a_localposY"] = obj.GetLocalPosition().y;
		j["a_localscaleX"] = obj.GetLocalScale().x;
		j["a_localscaleY"] = obj.GetLocalScale().y;
		j["a_localRot"] = obj.GetLocalRotation();

		// serialize texture
		AIngine::Rendering::Texture2D* texture = obj.GetComponent<AIngine::Rendering::Texture2D>();
		if (texture) {
			j["b_components"]["texture"] = SerializeTexture2D(*texture);
		}

		// serialize physComp
		AIngine::PhysicsComponent* physComp = obj.GetComponent<AIngine::PhysicsComponent>();
		if (physComp) {
			j["b_components"]["physics"] = SerializePhysicsComponent(*physComp);
		}

		outer[obj.GetName()] = j;
		return outer;
	}

	nlohmann::json SceneGraphSerializer::SerializeTexture2D(AIngine::Rendering::Texture2D & texture)
	{
		nlohmann::json j;
		j["path"] = texture.GetName();
		j["colorR"] = texture.GetColor().x;
		j["colorG"] = texture.GetColor().y;
		j["colorB"] = texture.GetColor().z;
		j["SizeX"] = texture.GetLocalWorldSize().x;
		j["SizeY"] = texture.GetLocalWorldSize().y;
		j["ParallaxX"] = texture.GetParallaxFactor().x;
		j["ParallaxY"] = texture.GetParallaxFactor().y;

		return j;
	}

	nlohmann::json SceneGraphSerializer::SerializePhysicsComponent(AIngine::PhysicsComponent & physComp)
	{
		return nlohmann::json();
	}

}
