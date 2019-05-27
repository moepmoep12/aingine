#include "Editor/Serialization.h"
#include "Editor/Editor.h"
#include "Rendering/texture.h"
#include "AIngine/Physics.h"

namespace AIngine::Editor::Serialization {

	void Serializer::SerializeSceneGraph(const std::string & path)
	{
		SceneGraphSerializer serializer;
		serializer.Traverse(AIngine::Editor::Editor::GetSceneGraphRoot());
		std::string result = serializer.Result.dump();
	}

	bool SceneGraphSerializer::Traverse(GameObject * root)
	{
		bool result = root->Accept(*this);

		return result;
	}

	bool SceneGraphSerializer::Enter(GameObject & node)
	{
		nlohmann::json serializedObject = SerializeGameObject(node);

		for (auto child : node.GetChildren()) {

			nlohmann::json c = SerializeGameObject(*child);
			serializedObject["children"].push_back(c);
		}

		if (node.GetParent()) {
			Result[node.GetParent()->GetName()][0]["children"].push_back(serializedObject);
		}
		else {
			Result[node.GetName()].push_back(serializedObject);
		}

		return true;
	}

	bool SceneGraphSerializer::Leave(GameObject & node)
	{
		return true;
	}

	bool SceneGraphSerializer::Visit(GameObject & node)
	{
		return true;
	}

	nlohmann::json SceneGraphSerializer::SerializeGameObject(GameObject & obj)
	{
		nlohmann::json j;

		j["name"] = obj.GetName();
		j["localposX"] = obj.GetLocalPosition().x;
		j["localposY"] = obj.GetLocalPosition().y;
		j["localscaleX"] = obj.GetLocalScale().x;
		j["localscaleY"] = obj.GetLocalScale().y;
		j["localRot"] = obj.GetLocalRotation();

		// serialize texture
		AIngine::Rendering::Texture2D* texture = obj.GetComponent<AIngine::Rendering::Texture2D>();
		if (texture) {
			j["components"]["texture"] = SerializeTexture2D(*texture);
		}

		// serialize physComp
		AIngine::PhysicsComponent* physComp = obj.GetComponent<AIngine::PhysicsComponent>();
		if (physComp) {
			j["components"]["physics"] = SerializePhysicsComponent(*physComp);
		}

		return j;
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
