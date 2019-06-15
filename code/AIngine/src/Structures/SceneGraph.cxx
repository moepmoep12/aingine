#include "Structures/SceneGraph.h"
#include "AIngine/GameObject.h"
#include "AIngine/Component.h"
#include "AIngine/Script.h"

#include "imgui.h"
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

	GameObject & SceneGraph::Copy(GameObject & other)
	{
		CopyTraverser copyTraverser(*this);
		copyTraverser.Traverse(&other);
		return *copyTraverser.Result;
	}

	void SceneGraph::DestroyObject(GameObject & gameobject)
	{
		DeleteTraverser deleteTraverser(m_gameObjectPool);
		deleteTraverser.Traverse(&gameobject);
	}

	GameObject * const SceneGraph::GetGameObject(const std::string & name)
	{
		SearchForNameTraverser traverser(name);
		traverser.Traverse(m_Root);
		return traverser.Result;
	}

	void SceneGraph::Reset()
	{
		DeleteTraverser deleteTreeTraverser(m_gameObjectPool);
		deleteTreeTraverser.Traverse(m_Root);
		m_Root = new (m_gameObjectPool.Allocate()) GameObject(nullptr, std::string("Root"));

	}

	GameObject & SceneGraph::CopySingle(GameObject & other, bool keepParent)
	{
		GameObject& copy = *new (m_gameObjectPool.Allocate()) GameObject(keepParent ? other.m_parent : m_Root, other.GetName());
		copy.SetLocalPosition(other.GetLocalPosition());
		copy.SetLocalScale(other.GetLocalScale());
		copy.SetRotation(other.GetLocalRotation());

		if (keepParent) copy.GetParent()->AddChild(&copy);

		for (auto& comp : other.GetComponents()) {
			Component* compCopy = comp->Copy(&copy);
			if (compCopy) {
				copy.m_components.push_back(compCopy);
				compCopy->SetEnabled(comp->IsEnabled());
			}
		}

		copy.SetActive(other.IsActive());

		return copy;
	}

	void SceneGraph::OnUpdate(float delta)
	{
		UpdateTraverser updateTraverser(delta);
		updateTraverser.Traverse(m_Root);
	}

	void SceneGraph::OnEvent(AIngine::Events::EventData & e)
	{
		EventTraverser et(e);
		et.Traverse(m_Root);
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
		GameObject* parent = root->GetParent();

		if (parent) {
			parent->RemoveChild(root);
		}

		bool result = root->Accept(*this);
		std::reverse(m_gameObjectsToDelete.begin(), m_gameObjectsToDelete.end());
		for (auto it = m_gameObjectsToDelete.begin(); it < m_gameObjectsToDelete.end(); it++)
		{
			GameObject* obj = *it._Ptr;
			if (obj->GetParent()) {
				obj->GetParent()->RemoveChild(obj);
			}
			(*it)->~GameObject();
			m_gameObjectPool->Free(obj);
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
			if (comp->m_wantsDestroy) {
				it = node.m_components.erase(it);
				node.RemoveComponent(comp);
			}
			else {
				it++;
			}
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
			if (comp->m_wantsDestroy) {
				it = node.m_components.erase(it);
				delete comp;
			}
			else {
				it++;
			}
		}
		return true;
	}

	/********************************** EVENT TRAVERSER ****************************************/


	EventTraverser::EventTraverser(AIngine::Events::EventData & e)
		:m_eventData(e)
	{
	}

	EventTraverser::~EventTraverser()
	{
	}

	bool EventTraverser::Traverse(GameObject * root)
	{
		return root->Accept(*this);
	}

	bool EventTraverser::Enter(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnEvent(m_eventData);
			}
			it++;
		}
		return true;
	}

	bool EventTraverser::Leave(GameObject & node)
	{
		return false;
	}

	bool EventTraverser::Visit(GameObject & node)
	{
		auto it = node.GetComponents().begin();

		while (it != node.GetComponents().end()) {
			Component* comp = *it._Ptr;
			if (comp->IsActive()) {
				comp->OnEvent(m_eventData);
			}
			it++;
		}
		return true;
	}

	/********************************** OnStart TRAVERSER ****************************************/

	bool OnStartTraverser::Traverse(GameObject * root)
	{
		return root->Accept(*this);
	}

	bool OnStartTraverser::Enter(GameObject & node)
	{
		auto it = node.GetComponents().begin();
		while (it != node.GetComponents().end()) {
			AIngine::Script* comp = dynamic_cast<AIngine::Script*>(*it._Ptr);
			if (comp && comp->IsActive()) comp->OnStart();
			it++;
		}
		return true;
	}

	bool OnStartTraverser::Visit(GameObject & node)
	{
		auto it = node.GetComponents().begin();
		while (it != node.GetComponents().end()) {
			AIngine::Script* comp = dynamic_cast<AIngine::Script*>(*it._Ptr);
			if (comp && comp->IsActive()) comp->OnStart();
			it++;
		}
		return true;
	}

	/********************************** OnEnd TRAVERSER ****************************************/

	bool OnEndTraverser::Traverse(GameObject * root)
	{
		return root->Accept(*this);
	}
	bool OnEndTraverser::Enter(GameObject & node)
	{
		auto it = node.GetComponents().begin();
		while (it != node.GetComponents().end()) {
			AIngine::Script* comp = dynamic_cast<AIngine::Script*>(*it._Ptr);
			if (comp) comp->OnEnd();
			it++;
		}
		return true;
	}
	bool OnEndTraverser::Visit(GameObject & node)
	{
		auto it = node.GetComponents().begin();
		while (it != node.GetComponents().end()) {
			AIngine::Script* comp = dynamic_cast<AIngine::Script*>(*it._Ptr);
			if (comp && comp->IsActive()) comp->OnEnd();
			it++;
		}
		return true;
	}

	/********************************** SEARCHFORNAME TRAVERSER ****************************************/


	SearchForNameTraverser::SearchForNameTraverser(const std::string & name)
		: m_name(name)
	{
	}

	bool SearchForNameTraverser::Traverse(GameObject * root)
	{
		return root->Accept(*this);
	}

	bool SearchForNameTraverser::Enter(GameObject & node)
	{
		if (node.GetName() == m_name) {
			Result = &node;
			return false;
		}
		return true;
	}

	bool SearchForNameTraverser::Visit(GameObject & node)
	{
		if (node.GetName() == m_name) {
			Result = &node;
			return false;
		}
		return true;
	}

	/********************************** Copy TRAVERSER ****************************************/


	SceneGraph::CopyTraverser::CopyTraverser(SceneGraph & graph)
		: m_sceneGraph(graph)
	{
	}

	bool SceneGraph::CopyTraverser::Traverse(GameObject * root)
	{
		bool result = root->Accept(*this);
		Result->SetParent(*root->GetParent());
		Result->GetParent()->AddChild(Result);
		return result;
	}

	bool SceneGraph::CopyTraverser::Enter(GameObject & node)
	{
		bool isFirst = m_parentStack.size() == 0;

		GameObject& copy = m_sceneGraph.CopySingle(node, !isFirst);

		if (m_parentStack.size() > 0) {
			GameObject* copiedParent = m_parentStack.back();
			copy.SetParent(*copiedParent, false);
		}
		m_parentStack.push_back(&copy);
		return true;
	}

	bool SceneGraph::CopyTraverser::Leave(GameObject & node)
	{
		if (m_parentStack.size() == 1) {
			Result = m_parentStack.back();
		}
		m_parentStack.pop_back();
		return true;
	}

	bool SceneGraph::CopyTraverser::Visit(GameObject & node)
	{
		GameObject& copy = m_sceneGraph.CopySingle(node, false);
		if (m_parentStack.size() > 0) {
			GameObject* copiedParent = m_parentStack.back();
			copy.SetParent(*copiedParent, false);
			copy.GetParent()->AddChild(&copy);
		}
		else {
			Result = &copy;
		}
		return true;
	}
}