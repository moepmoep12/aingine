#include "UI/CheckBox.h"
#include "Rendering/shader.h"
#include "Assets/Assets.h"

namespace AIngine::UI {

	CheckBox::CheckBox()
	{
		TextureOff.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/Editor/icons8-unausgefüllte-checkbox-100.png")->GetBitmap());
		TextureOn.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/Editor/icons8-ausgefüllte-checkbox-100.png")->GetBitmap());
	}

	bool CheckBox::Render(const glm::mat4 & modelMatrix, AIngine::Rendering::GLShaderProgram & shader) const
	{
		if (TextComponent) {
			glm::vec2 pos = GetRectangleAbsolute().GetCenter();
			pos.x *= GetRectangleAbsolute().width * 0.5f;
			TextComponent->SetPosition(pos);
		}

		if (m_bState) {
			TextureOn.Bind();
		}
		else {
			TextureOff.Bind();
		}

		shader.SetVector4f(5 /*spriteColor*/, TintColor);

		return true;
	}

	void CheckBox::PostInit()
	{
		TextComponent = GetOwner()->AddComponent<UIText>();
	}

	void CheckBox::OnClicked()
	{
		m_bState = !m_bState;
		OnStateChangedEvent(m_bState);
	}
	Component * CheckBox::Copy(GameObject * const owner) const
	{
		CheckBox* copy = new CheckBox();
		copy->SetEnabled(IsEnabled());
		copy->m_owner = owner;
		copy->m_isDisabled = m_isDisabled;
		copy->DisabledColor = DisabledColor;
		copy->TintColor = TintColor;
		copy->AnchorPos = AnchorPos;
		copy->SetRectangle(GetRectangleNative());

		copy->m_bState = m_bState;
		return copy;
	}
}