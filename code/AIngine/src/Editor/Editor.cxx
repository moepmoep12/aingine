#include "Editor/Editor.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "AIngine/KeyCodes.h"

// widgets
#include "Editor/Widgets/EditorWidget.h"
#include "Editor/Widgets/SceneGraphWidget.h"
#include "Editor/Widgets/LogWidget.h"
#include "Editor/Widgets/CameraWidget.h"
#include "Editor/Widgets/MenuBarWidget.h"

#include <glm/geometric.hpp>

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
		if (DidAnyDockedWidgetChangeSize()) {
			AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
			AIngine::Events::ViewportChangedEvent e(viewportRect);
			m_app.OnEvent(e);
		}
	}

	void Editor::OnEvent(AIngine::Events::Event & e)
	{
		AIngine::Events::EventDispatcher dispatcher(e);

		// call the OnWindowClose function if its a windowclose event
		dispatcher.Dispatch<AIngine::Events::KeyPressedEvent>(BIND_EVENT_TO_FN(Editor::OnKeyPressed));

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

	static bool s_physicsDraw = false;

	bool Editor::OnKeyPressed(AIngine::Events::KeyPressedEvent & e)
	{
		if (e.GetKeyCode() == AIngine::KeyCodes::F1)
		{
			AIngine::World::SetPhysicsDebugDrawActive(!s_physicsDraw);
			s_physicsDraw = !s_physicsDraw;
			return true;
		}

		return false;
	}

	bool Editor::OnWindowResized(AIngine::Events::WindowResizeEvent & e)
	{
		AIngine::Structures::Rectangle viewportRect = CalculateViewportRect(glm::vec2(m_app.GetWindow().GetWidth(), m_app.GetWindow().GetHeight()));
		AIngine::Events::ViewportChangedEvent ev(viewportRect);
		m_app.OnEvent(ev);
		return true;
	}

	Editor::Editor()
		: m_app(AIngine::Application::Get())
	{
		ASSERT(!s_instance, "Editor already running");
		s_instance = this;

		// allocate widget memory?

		m_widgets.push_back(new SceneGraphWidget(*m_app.m_world->m_sceneGraph));
		m_widgets.push_back(new LogWidget());
		m_widgets.push_back(new CameraWidget(*m_app.m_camera));
		m_widgets.push_back(new MenubarWidget());
	}

	AIngine::Structures::Rectangle Editor::CalculateViewportRect(const glm::vec2& windowSize) const
	{
		using Rectangle = AIngine::Structures::Rectangle;
		using Corner = AIngine::Structures::Rectangle::Corner;

		Rectangle viewportRect(0, m_widgets[3]->GetSize().y , windowSize.x, windowSize.y - m_widgets[3]->GetSize().y);
		Corner corner;

		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const EditorWidget& widget = *(*it._Ptr);
			if (widget.IsWindowDocked()) {
				AIngine::Structures::Rectangle widgetRect = widget.GetRectangle();
				if (viewportRect.Contains(widgetRect)) {
					glm::vec2 windowPos = glm::vec2(m_app.m_window->GetX(), m_app.m_window->GetY());
					glm::vec2 viewportPos = viewportRect.GetPosition();
					glm::vec2 viewportMax = viewportRect.GetMax();
					glm::vec2 widgetPos = widgetRect.GetPosition() - windowPos /*- m_widgets[3]->GetPosition().y - m_widgets[3]->GetSize().y*/;
					glm::vec2 widgetMax = widgetRect.GetMax() - windowPos/* - m_widgets[3]->GetPosition().y - m_widgets[3]->GetSize().y*/;

					if (widgetPos.x > viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.width -= widgetRect.width;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x < viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.x += widgetRect.width;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y > viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y == viewportMax.y)
					{
						viewportRect.height -= widgetRect.height;
					}
					if (widgetPos.x == viewportPos.x
						&& widgetPos.y == viewportPos.y
						&& widgetMax.x == viewportMax.x
						&& widgetMax.y < viewportMax.y)
					{
						viewportRect.y += widgetRect.height;
					}
				}
			}
			it++;
		}

		return viewportRect;
	}

	GameObject * Editor::GetSceneGraphRoot()
	{
		if (s_instance) {
			return &s_instance->m_app.m_world->GetSceneGraph().GetRoot();
		}
		return nullptr;
	}

	bool Editor::IsAnyUIElementHovered()
	{
		return ImGui::IsAnyWindowHovered();
	}

	bool Editor::DidAnyDockedWidgetChangeSize() const
	{
		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const EditorWidget& widget = *(*it._Ptr);
			if (widget.WasWindowSizeChanged() && widget.IsWindowDocked())
				return true;
			it++;
		}
		return false;
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