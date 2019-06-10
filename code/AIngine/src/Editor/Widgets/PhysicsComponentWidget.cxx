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

namespace AIngine::Editor {
	void PhysicsComponentWidget::OnImGuiRender()
	{
		if (m_activeGameObjects.size() == 1) {
			using namespace AIngine::Physics;
			AIngine::GameObject* obj = m_activeGameObjects[0];
			PhysicsComponent* physComp = obj->GetComponent<PhysicsComponent>();
			if (physComp) {
				ImGui::Separator();

				// Title
				ImGui::BulletText("Physics Component");
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
				ImGui::DragFloat("Density", &(float)properties.density, properties.density != 0 ? 0.01 * properties.density : 0.01);
				ImGui::DragFloat("Friction", &(float)properties.friction, properties.friction != 0 ? 0.01 * properties.friction : 0.01);
				ImGui::DragFloat("Restitution", &(float)properties.restitution, properties.restitution != 0 ? 0.01 * properties.restitution : 0.01);

				ImGui::NewLine();
				ImGui::NewLine();

				// PhysicsBodyInformation
				PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;

				// BodyType Combo
				{
					static const PhysicsBodyType choosableBodyTypes[] = { PhysicsBodyType::e_Static, PhysicsBodyType::e_Dynamic, PhysicsBodyType::e_Kinematic };
					static const char* bodyTypesNames[] = { "Static", "Dynamic", "Kinematic" };
					static int currentType = bodyInfo.type;
					if (ImGui::BeginCombo("BodyType", bodyTypesNames[currentType]))
					{
						for (int i = 0; i < IM_ARRAYSIZE(bodyTypesNames); i++) {
							bool isSelected = choosableBodyTypes[i] == currentType;
							if (ImGui::Selectable(bodyTypesNames[i], isSelected)) {
								currentType = choosableBodyTypes[i];
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
					if (ImGui::BeginCombo("Shape", shapeNames[shape])) {
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
					CreateBoxUI(physComp);
					break;

				case PhysicsShape::e_Polygon:
					CreatePolygonUI(physComp);
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
						physComp->CreatePolygonBody(properties, bodyInfo.type, bodyInfo.vertices, bodyInfo.verticesCount, bodyInfo.isTrigger);
						break;
					}
				}
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
		b2Vec2 topleft = b2Vec2(-width / 2.0, -height / 2.0);
		bodyInformation.vertices[0] = glm::vec2(topleft.x, topleft.y);

		b2Vec2 bottomLeft = b2Vec2(-width / 2.0, height / 2.0);
		bodyInformation.vertices[1] = glm::vec2(bottomLeft.x, bottomLeft.y);

		b2Vec2 bottomRight = b2Vec2(width / 2, height / 2.0);
		bodyInformation.vertices[2] = glm::vec2(bottomRight.x, bottomRight.y);

		b2Vec2 topRight = b2Vec2(width / 2.0, -height / 2.0);
		bodyInformation.vertices[3] = glm::vec2(topRight.x, topRight.y);

		bodyInformation.verticesCount = 4;
	}

	void PhysicsComponentWidget::DrawVertex(const glm::vec2 & worldPosition, const glm::vec3& color)
	{
		float vertexSize = 6.0f;
		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
		AIngine::Structures::Rectangle vertexRectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
		glm::vec2 mousePos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);

		if (vertexRectangle.Contains(mousePos)) {
			AIngine::Graphics::Point(worldPosition, vertexSize * 2, glm::vec3(0, 1, 0));
		}
		else {
			AIngine::Graphics::Point(worldPosition, vertexSize * 2, color);
		}
	}

	void PhysicsComponentWidget::CreateCircleUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		float* offset[] = { &physComp->m_offset.x,&physComp->m_offset.y };
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;
		glm::vec3 color;

		// Circle attributes
		ImGui::DragFloat2("Offset", *offset, 0.05f);
		if (ImGui::DragFloat("Radius", &bodyInfo.radius, bodyInfo.radius != 0 ? bodyInfo.radius * 0.01 : 0.01)) {
			color = glm::vec3(0, 1, 0);
		}
		else {
			color = glm::vec3(1, 0, 0);
		}


		// Draw the circle shape
		AIngine::Graphics::Circle(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, bodyInfo.radius, glm::vec3(1, 0, 0));

		// draw the center point
		AIngine::Graphics::Point(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, 5, color);
	}

	void PhysicsComponentWidget::CreateBoxUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;

		// box attributes
		ImGui::DragFloat("Width", &bodyInfo.width, bodyInfo.width != 0 ? bodyInfo.width * 0.01 : 0.01);
		ImGui::DragFloat("Height", &bodyInfo.height, bodyInfo.height != 0 ? bodyInfo.height * 0.01 : 0.01);
		UpdateBoxVertices(*physComp);

		// draw the vertices
		glm::vec2 vertices[4];
		for (int i = 0; i < 4; i++) {
			const b2Transform& xf = physComp->m_body->GetTransform();
			b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
			vertices[i] = glm::vec2(result.x, result.y);
			DrawVertex(vertices[i]);
		}

		// draw the box shape
		AIngine::Graphics::Box(vertices, glm::vec3(0, 0, 1));
	}

	void PhysicsComponentWidget::CreatePolygonUI(AIngine::Physics::PhysicsComponent * physComp)
	{
		AIngine::Physics::PhysicsBodyInformation& bodyInfo = physComp->m_bodyInformation;
		glm::vec2 vertices[AIngine::Physics::maxVertices];
		int vertexUnderChangeIndex = -1;

		// transform the vertices into worldspace
		for (int i = 0; i < bodyInfo.verticesCount; i++) {
			const b2Transform& xf = physComp->m_body->GetTransform();
			b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
			vertices[i] = glm::vec2(result.x, result.y);
		}

		// create dragfloat2 for each vertex to adjust its position
		for (int i = 0; i < bodyInfo.verticesCount; i++) {
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
				glm::vec2 newVertex = bodyInfo.vertices[bodyInfo.verticesCount - 1];
				newVertex += newVertex * 0.1f; // offset the new vertex by 10% of the previous
				bodyInfo.vertices[bodyInfo.verticesCount] = newVertex;
				bodyInfo.verticesCount++;
			}
		}

		// draw the vertices
		for (int i = 0; i < bodyInfo.verticesCount; i++) {
			if (i == vertexUnderChangeIndex) {
				DrawVertex(vertices[i],glm::vec3(0,1,0));
			}
			else {
				DrawVertex(vertices[i]);
			}
		}

		// draw the polygon shape
		AIngine::Graphics::Polygon(vertices, bodyInfo.verticesCount, glm::vec3(0, 0, 1));
	}

}