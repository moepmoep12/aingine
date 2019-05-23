#include "Editor/Editor.h"
#include "AIngine/Macros.h"
#include "Application.h"
#include "AIngine/KeyCodes.h"

// widgets
#include "Editor/ImGuiWidget.h"
#include "Editor/SceneGraphWidget.h"
#include "Editor/LogWidget.h"
#include "Editor/CameraWidget.h"

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

	glm::vec2 Editor::ClosestPoint(const AIngine::Structures::Rectangle & rect, const glm::vec2 & point, AIngine::Structures::Rectangle::Corner* corner) const
	{
		glm::vec2 corners[] = {
			rect.GetPosition(),
			rect.GetPosition() + glm::vec2(rect.width,0),
			rect.GetMax(),
			rect.GetPosition() + glm::vec2(0, rect.height)
		};
		float minDistance = std::numeric_limits<float>::max();

		unsigned int index = 0;

		for (int i = 0; i < 4; ++i) {
			float distance = glm::distance(point, corners[i]);
			if (distance < minDistance) {
				index = i;
				minDistance = distance;
			}
		}
		*corner = (AIngine::Structures::Rectangle::Corner)index;
		return corners[index];
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
	}

	AIngine::Structures::Rectangle Editor::CalculateViewportRect(const glm::vec2& windowSize) const
	{
		using Rectangle = AIngine::Structures::Rectangle;
		using Corner = AIngine::Structures::Rectangle::Corner;

		Rectangle viewportRect(0, 0, windowSize.x, windowSize.y);
		Corner corner;

		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const ImGuiWidget& widget = *(*it._Ptr);
			if (widget.IsWindowDocked()) {
				glm::vec2 points[] = {
					widget.GetPosition(),
					widget.GetPosition() + glm::vec2(widget.GetSize().x,0)
				};

				for (glm::vec2 point : points) {
					if (viewportRect.Contains(point)) {
						glm::vec2 closestCorner = ClosestPoint(viewportRect, point, &corner);
						glm::vec2 distance = point - closestCorner;
						closestCorner += distance;

						if (corner == Corner::BottomRight) {
							viewportRect.width += distance.x;
						}
						if (corner == Corner::BottomLeft) {
							viewportRect.height += distance.y;
						}
						if (corner == Corner::TopRight) {
							viewportRect.width += distance.x;
						}
						if (corner == Corner::TopLeft) {
							viewportRect.width += distance.x;
							viewportRect.x -= distance.x;
						}

					}
				}
			}
			it++;
		}

		return viewportRect;
	}

	bool Editor::IsAnyUIElementHovered()
	{
		return ImGui::IsAnyWindowHovered();
	}

	bool Editor::DidAnyDockedWidgetChangeSize() const
	{
		auto it = m_widgets.begin();

		while (it != m_widgets.end()) {
			const ImGuiWidget& widget = *(*it._Ptr);
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