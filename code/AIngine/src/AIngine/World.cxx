#include "AIngine/World.h"
#include "AIngine/Macros.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "Structures/SceneGraph.h"
#include "Rendering/PhysicsRenderer.h"
#include "Editor/Editor.h"


namespace AIngine {

	World* World::s_instance = nullptr;

	GameObject * const AIngine::World::SpawnObject(const std::string & name, GameObject * parent, const glm::vec2 & position, const glm::vec2 & scale, const float rotation)
	{
		if (s_instance) {
			return s_instance->m_sceneGraph->SpawnObject(name, parent, position, scale, rotation);
		}

		return nullptr;
	}

	void World::DestroyObject(GameObject & gameobject)
	{
		if (s_instance) {
			s_instance->m_sceneGraph->DestroyObject(gameobject);
		}
	}

	void World::SetGravity(const glm::vec2 & gravity)
	{
		if (s_instance) {
			s_instance->m_physicsWorld->SetGravity(b2Vec2(gravity.x, gravity.y));
			s_instance->m_gravity = gravity;
		}
	}

	void World::SetPhysicsDebugDrawActive(const bool & active)
	{
		if (s_instance) {
			s_instance->m_isPhysicsDebugDrawn = active;
		}
	}

	AIngine::Structures::SceneGraph & World::GetSceneGraph()
	{
		return *m_sceneGraph;
	}

	void World::OnAttach()
	{
		m_sceneGraph->OnAttach();
	}

	void World::OnDetach()
	{
		m_sceneGraph->OnDetach();
	}

	void World::OnUpdate(float delta)
	{
		m_sceneGraph->OnUpdate(delta);

		if ((AIngine::Editor::Editor::GetIsInPlayMode()))
			m_physicsWorld->Step(1.0f / 60.0f, 8, 3);

		if (m_isPhysicsDebugDrawn)
			m_physicsWorld->DrawDebugData();
	}

	void World::OnEvent(AIngine::Events::EventData & e)
	{
		m_sceneGraph->OnEvent(e);
	}

	void World::OnImGuiRender()
	{
		m_sceneGraph->OnImGuiRender();
	}

	World::World(const glm::vec4 & bounds, const glm::vec2 & gravity)
		: m_bounds(bounds), m_gravity(gravity)
	{
		ASSERT(!s_instance, "An entity of type World already exists!");
		s_instance = this;

		m_physicsWorld = new b2World(b2Vec2(gravity.x, gravity.y));
		m_sceneGraph = new AIngine::Structures::SceneGraph();

		// when we're in Release don't do debug drawing
#ifdef _DEBUG 
		m_physRenderer = new AIngine::Rendering::PhysicsRenderer();
		m_physRenderer->SetFlags(b2Draw::e_shapeBit /*+ b2Draw::e_centerOfMassBit*/);
		m_physicsWorld->SetDebugDraw(m_physRenderer);
		m_isPhysicsDebugDrawn = true;
#endif

		m_contactListener = new Physics::ContactListener();
		m_physicsWorld->SetContactListener(m_contactListener);

		CreateWorldBounds();
	}

	World::World(float bottomLeft, float bottomRight, float topLeft, float topRight, const glm::vec2 & gravity)
		: World(glm::vec4(bottomLeft, bottomRight, topLeft, topRight), gravity)
	{
	}

	World::~World()
	{
		s_instance = nullptr;
		delete m_physicsWorld;
		delete m_sceneGraph;
		delete m_contactListener;
#ifdef _DEBUG
		if (m_physRenderer) delete m_physRenderer;
#endif
	}
	void World::CreateWorldBounds()
	{
		float boundSizeX = abs(m_bounds.y - m_bounds.x);
		float boundSizeY = abs(m_bounds.w - m_bounds.z);

		b2BodyDef bodydef;
		bodydef.type = b2_staticBody;
		b2EdgeShape shape;
		b2FixtureDef fixturedef;
		fixturedef.shape = &shape;
		fixturedef.density = 1.0;
		fixturedef.isSensor = true;


		// bottom
		bodydef.position.Set(m_bounds.x + (boundSizeX / 2.0f), m_bounds.w);
		shape.Set(b2Vec2(-boundSizeX / 2.0f, 0.0f), b2Vec2(boundSizeX / 2.0f, 0.0f));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// top
		bodydef.position.Set(m_bounds.x + (boundSizeX / 2.0f), m_bounds.z);
		shape.Set(b2Vec2(-boundSizeX / 2.0f, 0.0), b2Vec2(boundSizeX / 2.0f, 0.0f));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// left
		bodydef.position.Set(m_bounds.x, m_bounds.z + (boundSizeY / 2.0f));
		shape.Set(b2Vec2(0.0f, -boundSizeY / 2.0f), b2Vec2(0.0f, boundSizeY / 2.0f));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// right
		bodydef.position.Set(m_bounds.y, m_bounds.z + (boundSizeY / 2.0f));
		shape.Set(b2Vec2(0.0f, -boundSizeY / 2.0f), b2Vec2(0.0f, boundSizeY / 2.0f));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);
	}
}
