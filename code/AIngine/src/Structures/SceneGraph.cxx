#include "Structures/SceneGraph.h"
#include "Debug/log.h"
#include "imgui.h"
#include "Application.h"
#include"Rendering/texture.h"
#include "AIngine/GameObject.h"
#include "Events/InputEvents.h"
#include <sstream>

namespace AIngine::Structures {

	static unsigned int nodeCount = 1;

	SceneGraph::SceneGraph() : Layer("SceneGraph Layer")
	{
		m_Root = new (m_gameObjectPool.Allocate()) GameObject(nullptr, std::string("Root"));
	}

	SceneGraph::~SceneGraph()
	{
		DeleteTraverser deleteTreeTraverser(m_gameObjectPool);
		deleteTreeTraverser.Traverse(m_Root);
	}

	GameObject * const SceneGraph::SpawnObject(const std::string & name, GameObject * parent, const glm::vec2 & position, const glm::vec2 & scale, const float rotation)
	{
		if (!parent) {
			parent = m_Root;
		}

		GameObject* newObject = new (m_gameObjectPool.Allocate()) GameObject(parent, name);
		newObject->SetLocalPosition(position);
		newObject->SetLocalScale(scale);
		newObject->SetRotation(rotation);

		parent->AddChild(newObject);
		return newObject;
	}

	void SceneGraph::DestroyObject(GameObject & gameobject)
	{
		DeleteTraverser deleteTraverser(m_gameObjectPool);
		deleteTraverser.Traverse(&gameobject);
	}

	void SceneGraph::OnUpdate(float delta)
	{
		UpdateTraverser updateTraverser(delta);
		updateTraverser.Traverse(m_Root);
	}

	void SceneGraph::OnEvent(AIngine::Events::Event & e)
	{

	}

	/********************************** DELETE TRAVERSER ****************************************/

	DeleteTraverser::DeleteTraverser(AIngine::Memory::Pool<GameObject>& gameObjectPool)
		: m_gameObjectPool(&gameObjectPool)
	{

	}

	DeleteTraverser::~DeleteTraverser()
	{
		m_gameObjectsToDelete.clear();
	}

	bool DeleteTraverser::Traverse(GameObject* root)
	{
		GameObject* parent = &root->GetParent();

		if (parent) {
			parent->RemoveChild(root);
		}

		bool result = root->Accept(*this);

		for (auto it = m_gameObjectsToDelete.begin(); it < m_gameObjectsToDelete.end(); it++) {
			m_gameObjectPool->Free(*it);
			(*it)->~GameObject();
		}

		return result;
	}

	bool DeleteTraverser::Enter(GameObject & node)
	{
		m_gameObjectsToDelete.push_back(&node);
		return true;
	}
	bool DeleteTraverser::Leave(GameObject & node)
	{
		return true;
	}

	bool DeleteTraverser::Visit(GameObject & node)
	{
		m_gameObjectsToDelete.push_back(&node);
		return true;
	}

	/********************************** UPDATE TRAVERSER ****************************************/


	UpdateTraverser::UpdateTraverser(float deltaTime) : m_deltaTime(deltaTime)
	{
	}

	UpdateTraverser::~UpdateTraverser()
	{
	}

	bool UpdateTraverser::Traverse(GameObject * root)
	{
		return	root->Accept(*this);
	}

	bool UpdateTraverser::Enter(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnUpdate(m_deltaTime);
			}
			it++;
		}
		return true;
	}

	bool UpdateTraverser::Leave(GameObject & node)
	{
		return true;
	}

	bool UpdateTraverser::Visit(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnUpdate(m_deltaTime);
			}
			it++;
		}
		return true;
	}

}