#include "Editor/Widgets/PhysicsComponentWidget.h"
#include "AIngine/GameObject.h"
#include "AIngine/Physics.h"
#include "AIngine/Graphics.h"
#include "Structures/Rectangle.h"
#include "Rendering/Camera.h"
#include "AIngine/Input.h"

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

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
				struct FuncHolder { static bool ItemGetter(void* data, int idx, const char** out_str) { *out_str = ((const char**)data)[idx]; return true; } };

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
					// Circle attributes
					ImGui::DragFloat2("Offset", *pos, 0.05f);
					ImGui::DragFloat("Radius", &bodyInfo.radius, bodyInfo.radius != 0 ? bodyInfo.radius * 0.01 : 0.01);
					// Draw the circle
					AIngine::Graphics::Circle(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, bodyInfo.radius, glm::vec3(1, 0, 0));
					AIngine::Graphics::Point(physComp->GetOwner()->GetWorldPosition() + physComp->m_offset, 5, glm::vec3(1, 0, 0));
					break;

				case PhysicsShape::e_Box:
					// box attributes
					ImGui::DragFloat("Width", &bodyInfo.width, bodyInfo.width != 0 ? bodyInfo.width * 0.01 : 0.01);
					ImGui::DragFloat("Height", &bodyInfo.height, bodyInfo.height != 0 ? bodyInfo.height * 0.01 : 0.01);
					UpdateBoxVertices(*physComp);
					// draw the box
					glm::vec2 vertices[4];
					for (int i = 0; i < 4; i++) {
						const b2Transform& xf = physComp->m_body->GetTransform();
						b2Vec2 result = b2Mul(xf, b2Vec2(bodyInfo.vertices[i].x, bodyInfo.vertices[i].y));
						vertices[i] = glm::vec2(result.x, result.y);
						DrawVertex(vertices[i]);
					}
					AIngine::Graphics::Box(vertices, glm::vec3(0, 0, 1));
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

	void PhysicsComponentWidget::DrawVertex(const glm::vec2 & worldPosition)
	{
		float vertexSize = 6.0f;
		glm::vec2 screenPos = AIngine::Rendering::Camera::Get().WorldToScreenPoint(worldPosition);
		AIngine::Structures::Rectangle vertexRectangle(screenPos.x - vertexSize, screenPos.y - vertexSize, 2 * vertexSize, 2 * vertexSize);
		glm::vec2 mousePos = glm::vec2(AIngine::Input::GetMousePosition().first, AIngine::Input::GetMousePosition().second);

		if (vertexRectangle.Contains(mousePos)) {
			AIngine::Graphics::Point(worldPosition, vertexSize * 2, glm::vec3(0, 1, 0));
		}
		else {
			AIngine::Graphics::Point(worldPosition, vertexSize * 2, glm::vec3(1, 0, 0));
		}


		glm::vec2 verts[4];
		verts[0] = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertexRectangle.GetPosition());
		verts[1] = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertexRectangle.GetBottomLeft());
		verts[2] = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertexRectangle.GetMax());
		verts[3] = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertexRectangle.GetTopRight());
		AIngine::Graphics::Box(verts, glm::vec3(0, 0, 0));
	}

}