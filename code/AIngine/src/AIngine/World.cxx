#include "AIngine/World.h"
#include "AIngine/GameObject.h"
#include "Debug/log.h"
#include "Application.h"

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
			s_instance->DestroyObject(gameobject);
		}
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
		m_physicsWorld->Step(1.0 / 60.0, 8, 3);
		m_physicsWorld->DrawDebugData();
	}

	void World::OnEvent(AIngine::Events::Event & e)
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
		m_sceneGraph = new SceneGraph();
		m_physRenderer = new AIngine::Rendering::PhysicsRenderer();
		m_physRenderer->SetFlags(b2Draw::e_shapeBit /*+ b2Draw::e_centerOfMassBit*/);
		m_physicsWorld->SetDebugDraw(m_physRenderer);

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
		delete m_physRenderer;
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


		// bottom
		bodydef.position.Set(m_bounds.x + (boundSizeX / 2.0), m_bounds.w);
		shape.Set(b2Vec2(-boundSizeX / 2.0, 0.0), b2Vec2(boundSizeX / 2.0, 0.0));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// top
		bodydef.position.Set(m_bounds.x + (boundSizeX / 2.0), m_bounds.z);
		shape.Set(b2Vec2(-boundSizeX / 2.0, 0.0), b2Vec2(boundSizeX / 2.0, 0.0));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// left
		bodydef.position.Set(m_bounds.x, m_bounds.z + (boundSizeY / 2.0));
		shape.Set(b2Vec2(0.0, -boundSizeY / 2.0), b2Vec2(0.0, boundSizeY / 2.0));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);

		// right
		bodydef.position.Set(m_bounds.y, m_bounds.z + (boundSizeY / 2.0));
		shape.Set(b2Vec2(0.0, -boundSizeY / 2.0), b2Vec2(0.0, boundSizeY / 2.0));
		m_physicsWorld->CreateBody(&bodydef)->CreateFixture(&fixturedef);
	}
}
