#include "Editor/Editor.h"
#include "Debug/log.h"
#include "Editor/ImGuiWidget.h"
#include "Application.h"

// widgets
#include "Editor/SceneGraphWidget.h"
#include "Editor/LogWidget.h"
#include "Editor/CameraWidget.h"


namespace AIngine::Editor {

	Editor* Editor::Editor::s_instance = nullptr;

	void Editor::OnAttach()
	{
		//activate widgets?
	}

	void Editor::OnDetach()
	{
	}

	void Editor::OnUpdate(float delta)
	{
	}

	void Editor::OnEvent(AIngine::Events::Event & e)
	{
		auto it = m_widgets.begin();
		while (it != m_widgets.end()) {
			(*it._Ptr)->OnEvent(e);
			it++;
		}

	}

	void Editor::OnImGuiRender()
	{
		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			(*it._Ptr)->OnImGuiRender();
			it++;
		}
	}

	Editor::Editor() : m_app(AIngine::Application::Get())
	{
		ASSERT(!s_instance, "Editor already running");
		s_instance = this;

		// allocate widget memory?

		m_widgets.push_back(new SceneGraphWidget(*m_app.m_world->m_sceneGraph));
		m_widgets.push_back(new LogWidget());
		m_widgets.push_back(new CameraWidget(*m_app.m_camera));
	}

	glm::vec4 Editor::GetEditorBounds() const
	{
		return glm::vec4();
	}

	Editor::~Editor()
	{
		s_instance = nullptr;

		for (unsigned int i = 0; i < m_widgets.size(); i++) {
			delete m_widgets[i];
		}
		m_widgets.clear();
	}

}