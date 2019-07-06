#include "Editor/Widgets/PhysicsComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "AIngine/Graphics.h"
#include "Structures/Rectangle.h"
#include "Rendering/Camera.h"
#include "AIngine/Input.h"

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include <sstream>
#include "Box2D/Box2D.h"

namespace AIngine::Editor::Widget::Component {
	void PhysicsComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			using namespace AIngine::Physics;
			AIngine::GameObject* obj = m_activeGameObjects[0];
			PhysicsComponent* physComp = obj->GetComponent<PhysicsComponent>();
			if (physComp) {
				ImGui::Separator();

				// Title
				bool active = physComp->IsEnabled();
				if (ImGui::Checkbox("Active##physcomp", &active)) {
					physComp->SetEnabled(active);
				}
				ImGui::SameLine();
				float textWidth = ImGui::CalcTextSize("Physics Component").x;
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) * 0.5f);
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Physics Component");
				if (ImGui::BeginPopupContextItem("Physics Component")) {
					if (ImGui::Selectable("Delete##physComp")) {
						obj->RemoveComponent<AIngine::Physics::PhysicsComponent>();
						ImGui::EndPopup();
						return;
					}
					ImGui::EndPopup();
				}
				ImGui::NewLine();


				// PhysicsProperties
				const PhysicsProperties& properties = physComp->GetProperties();
				ImGui::DragFloat("Density", &(float)properties.density, properties.density != 0 ? 0.01f * properties.density : 0.01f);
				ImGui::DragFloat("Friction", &(float)properties.friction, properties.friction != 0 ? 0.01f * properties.friction : 0.01f);
				ImGui::DragFloat("Restitution", &(float)properties.restitution, properties.restitution != 0 ? 0.01f * properties.restitution : 0.01f);

				ImGui::NewLine();
				ImGui::NewLine();

				// PhysicsBodyInformation
				PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;

				// BodyType Combo
				{
					static const PhysicsBodyType choosableBodyTypes[] = { PhysicsBodyType::e_Static, PhysicsBodyType::e_Dynamic, PhysicsBodyType::e_Kinematic };
					static const char* bodyTypesNames[] = { "Static", "Dynamic", "Kinematic" };
					static int currentType = (int)bodyInfo.type;
					if (ImGui::BeginCombo("BodyType", bodyTypesNames[currentType]))
					{
						for (int i = 0; i < IM_ARRAYSIZE(bodyTypesNames); i++) {
							bool isSelected = (int)choosableBodyTypes[i] == currentType;
							if (ImGui::Selectable(bodyTypesNames[i], isSelected)) {
								currentType = (int)choosableBodyTypes[i];
								bodyInfo.type = choosableBodyTypes[i];
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}

				// IsTrigger CheckBox
				ImGui::Checkbox("Is Trigger", &bodyInfo.isTrigger);

				// Shape Combo
				PhysicsShape& shape = bodyInfo.shape;
				{
					static const PhysicsShape choosablePhysicsShapes[] = { PhysicsShape::e_Circle, PhysicsShape::e_Box, PhysicsShape::e_Polygon,  PhysicsShape::e_Edge, };
					static const char* shapeNames[] = { "Circle","Box", "Polygon", "Edge" };
					if (ImGui::BeginCombo("Shape", shapeNames[(int)shape])) {
						for (int i = 0; i < IM_ARRAYSIZE(shapeNames); i++) {
							bool isSelected = choosablePhysicsShapes[i] == shape;
							if (ImGui::Selectable(shapeNames[i], isSelected)) {
								shape = choosablePhysicsShapes[i];
								bodyInfo.shape = choosablePhysicsShapes[i];
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
				}

				float* pos[] = { &physComp->m_offset.x,&physComp->m_offset.y };

				switch (bodyInfo.shape) {
				case PhysicsShape::e_Circle:
					CreateCircleUI(physComp);
					break;

				case PhysicsShape::e_Box:
					if (bodyInfo.width == 0 || bodyInfo.height == 0) {
						physComp->CreateBoxBody(properties, bodyInfo.type, 1, 1, bodyInfo.isTrigger);
					}
					CreateBoxUI(physComp);
					break;

				case PhysicsShape::e_Polygon:
					CreatePolygonUI(physComp);
					break;

				case PhysicsShape::e_Edge:
					if (bodyInfo.verticesCount < 2) {
						physComp->CreateEdgeBody(properties, bodyInfo.type, glm::vec2(-0.5, 0), glm::vec2(0.5, 0));
					}
					CreateEdgeUI(physComp);
					break;
				}

				ImGui::NewLine();
				ImGui::NewLine();

				if (ImGui::Button("Apply Changes##physComp"))
				{
					switch (shape) {
					case PhysicsShape::e_Circle:
						physComp->CreateCircleBody(properties, bodyInfo.type, bodyInfo.radius, bodyInfo.isTrigger);
						break;

					case PhysicsShape::e_Box:
						physComp->CreateBoxBody(properties, bodyInfo.type, bodyInfo.width, bodyInfo.height, bodyInfo.isTrigger);
						break;

					case PhysicsShape::e_Polygon:
						if (bodyInfo.verticesCount < 3)
						{
							// to create a valid polygon we need valid vertices. so create a box first
							physComp->CreateBoxBody(properties, bodyInfo.type, 1, 1, bodyInfo.isTrigger);
						}
						physComp->CreatePolygonBody(properties, bodyInfo.type, bodyInfo.vertices, bodyInfo.verticesCount, bodyInfo.isTrigger);
						break;

					case PhysicsShape::e_Edge:
						physComp->CreateEdgeBody(properties, bodyInfo.type, bodyInfo.vertices[0], bodyInfo.vertices[1], bodyInfo.isTrigger);
						break;
					}
				}
				ImGui::Separator();
			}
		}
	}

	void PhysicsComponentWidget::UpdateBoxVertices(AIngine::Physics::PhysicsComponent & physComp) const
	{
		b2Body* body = physComp.m_body;
		AIngine::Physics::PhysicsBodyInformation& bodyInformation = physComp.m_bodyInformation;
		float width = physComp.m_bodyInformation.width;
		float height = physComp.m_bodyInformation.height;

		// counter clock wise
		b2Vec2 topleft = b2Vec2(-width / 2.0f, -height / 2.0f);
		bodyInformation.vertices[0] = glm::vec2(topleft.x, topleft.y);

		b2Vec2 bottomLeft = b2Vec2(-width / 2.0f, height / 2.0f);
		bodyInformation.vertices[1] = glm::vec2(bottomLeft.x, bottomLeft.y);

		b2Vec2 bottomRight = b2Vec2(width / 2.0f, height / 2.0f);
		bodyInformation.vertices[2] = glm::vec2(bottomRight.x, bottomRight.y);

		b2Vec2 topRight = b2Vec2(width / 2.0f, -height / 2.0f);
		bodyInformation.vertices[3] = glm::vec2(topRight.x, topRight.y);

		bodyInformation.verticesCount = 4;
	}

	void PhysicsComponentWidget::CreateMoveableVertex(glm::vec2 & localPosition, const b2Transform & transform, float size, const glm::vec4 & colorInteract, const glm::vec4 & colorNormal)
	{
		glm::vec2 mouseScreenPos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);
		glm::vec2 mouseWorldPos = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(mouseScreenPos);
		glm::vec2 vertexWorldPos = CalculateWorldPosition(localPosition, transform);
		glm::vec2 vertexScreenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(vertexWorldPos);
		AIngine::Structures::RectangleF vertexRectangle(vertexScreenPos.x - size, vertexScreenPos.y - size, 2 * size, 2 * size);

		if (vertexRectangle.Contains(mouseScreenPos))
		{
			if (AIngine::Input::IsMouseButtonPressed(0)) {
				glm::vec2 diff = mouseWorldPos - vertexWorldPos;
				localPosition += diff;
				vertexWorldPos = CalculateWorldPosition(localPosition, transform);
				vertexScreenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(vertexWorldPos);
				vertexRectangle = AIngine::Structures::Rectangle(vertexScreenPos.x - size, vertexScreenPos.y - size, 2 * size, 2 * size);
			}

			AIngine::Graphics::Point(CalculateWorldPosition(localPosition, transform), size, colorInteract);
		}
		else {
			AIngine::Graphics::Point(vertexWorldPos, size, colorNormal);
		}

		AIngine::Structures::RectangleI rect((int)vertexRectangle.x, (int)vertexRectangle.y, (int)vertexRectangle.width, (int)
			vertexRectangle.height);
		AIngine::Graphics::BoxScreen(rect, glm::vec4(1, 1, 0, 1));
	}

	void PhysicsComponentWidget::CreateCircleUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		float* offset[] = { &physComp->m_offset.x,&physComp->m_offset.y };
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;
		glm::vec4 color;

		// Circle attributes
		ImGui::DragFloat2("Offset", *offset, 0.05f);
		if (ImGui::DragFloat("Radius", &bodyInfo.radius, bodyInfo.radius != 0 ? bodyInfo.radius * 0.01f : 0.01f)) {
			color = glm::vec4(0, 1, 0, 1);
		}
		else {
			color = glm::vec4(1, 0, 0, 1);
		}


		// Draw the circle shape
		AIngine::Graphics::CircleWorld(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, bodyInfo.radius, glm::vec4(1, 0, 0, 1));

		// draw the center point
		AIngine::Graphics::Point(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, 5, color);
	}

	void PhysicsComponentWidget::CreateBoxUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;

		// box attributes
		ImGui::DragFloat("Width", &bodyInfo.width, bodyInfo.width != 0 ? bodyInfo.width * 0.01f : 0.01f);
		ImGui::DragFloat("Height", &bodyInfo.height, bodyInfo.height != 0 ? bodyInfo.height * 0.01f : 0.01f);
		UpdateBoxVertices(*physComp);

		// draw the vertices
		glm::vec2 vertices[4];
		for (int i = 0; i < 4; i++) {
			const b2Transform& xf = physComp->m_body->GetTransform();
			CreateMoveableVertex(bodyInfo.vertices[i], xf, 7);
			b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
			vertices[i] = glm::vec2(result.x, result.y);
		}

		// draw the box shape
		AIngine::Graphics::BoxWorld(vertices, glm::vec4(0, 0, 1, 1));
	}

	void PhysicsComponentWidget::CreatePolygonUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;
		glm::vec2 vertices[AIngine::Physics::maxVertices];
		int vertexUnderChangeIndex = -1;


		// create dragfloat2 for each vertex to adjust its position
		for (unsigned int i = 0; i < bodyInfo.verticesCount; i++) {
			std::stringstream ss;
			ss << "Vertex" << i << "##vertices";
			float* pos[] = { &bodyInfo.vertices[i].x, &bodyInfo.vertices[i].y };
			if (ImGui::DragFloat2(ss.str().c_str(), *pos, 0.01f) || ImGui::IsItemHovered()) {
				vertexUnderChangeIndex = i;
			}
		}

		// add vertex button
		if (ImGui::Button("Add Vertex##physPolygon")) {
			if (bodyInfo.verticesCount < AIngine::Physics::maxVertices) {
				glm::vec2 newVertex = bodyInfo.vertices[std::clamp(bodyInfo.verticesCount - 1, (unsigned int)0, (unsigned int)AIngine::Physics::maxVertices)];
				newVertex += newVertex * 0.1f; // offset the new vertex by 10% of the previous
				bodyInfo.vertices[bodyInfo.verticesCount] = newVertex;
				bodyInfo.verticesCount++;
			}
		}

		// remove vertex button
		ImGui::SameLine();
		if (ImGui::Button("Remove Vertex##physPolygon")) {
			if (bodyInfo.verticesCount > 3) {
				bodyInfo.verticesCount--;
				physComp->CreatePolygonBody(physComp->m_properties, bodyInfo.type, bodyInfo.vertices, bodyInfo.verticesCount, bodyInfo.isTrigger);
			}
		}

		// draw the vertices
		for (unsigned int i = 0; i < bodyInfo.verticesCount; i++) {
			const b2Transform& xf = physComp->m_body->GetTransform();
			if (i == vertexUnderChangeIndex) {
				CreateMoveableVertex(bodyInfo.vertices[i], xf, 7, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1));
			}
			else {
				CreateMoveableVertex(bodyInfo.vertices[i], xf, 7);
			}
			b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
			vertices[i] = glm::vec2(result.x, result.y);
		}

		AIngine::Graphics::PolygonWorld(vertices, bodyInfo.verticesCount, glm::vec4(0, 0, 1, 1));
	}

	void PhysicsComponentWidget::CreateEdgeUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;
		glm::vec2 vertices[AIngine::Physics::maxVertices];
		int vertexUnderChangeIndex = -1;


		// create dragfloat2 for each vertex to adjust its position
		for (unsigned int i = 0; i < 2; i++) {
			std::stringstream ss;
			ss << "Vertex" << i << "##vertices";
			float* pos[] = { &bodyInfo.vertices[i].x, &bodyInfo.vertices[i].y };
			if (ImGui::DragFloat2(ss.str().c_str(), *pos, 0.01f) || ImGui::IsItemHovered()) {
				vertexUnderChangeIndex = i;
			}
		}

		// draw the vertices
		for (unsigned int i = 0; i < 2; i++) {
			const b2Transform& xf = physComp->m_body->GetTransform();
			if (i == vertexUnderChangeIndex) {
				CreateMoveableVertex(bodyInfo.vertices[i], xf, 7, glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 1));
			}
			else {
				CreateMoveableVertex(bodyInfo.vertices[i], xf, 7);
			}
			b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
			vertices[i] = glm::vec2(result.x, result.y);
		}

		AIngine::Graphics::Line(vertices[0], vertices[1], glm::vec4(0, 0, 1, 1));
	}

	glm::vec2 PhysicsComponentWidget::CalculateWorldPosition(const glm::vec2 & localPos, const b2Transform & transform)
	{
		b2Vec2 temp = b2Mul(transform, b2Vec2(localPos.x, localPos.y));
		return glm::vec2(temp.x, temp.y);
	}

}