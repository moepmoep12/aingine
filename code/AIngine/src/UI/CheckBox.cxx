#include "UI/CheckBox.h"
#include "Rendering/shader.h"
#include "Assets/Assets.h"

namespace AIngine::UI {

	CheckBox::CheckBox()
	{
		TextureOff.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/Editor/icons8-unausgefüllte-checkbox-100.png")->GetBitmap());
		TextureOn.Generate(AIngine::Assets::AssetRegistry::Load<AIngine::Assets::BitmapAsset>("AIngine/textures/Editor/icons8-ausgefüllte-checkbox-100.png")->GetBitmap());
	}

	bool CheckBox::Render(AIngine::Rendering::GLShaderProgram & shader) const
	{
		if (TextComponent) {
			auto rect = GetRectangleNative();
			glm::vec2 pos = rect.GetPosition();
			pos.x += GetRectangle().width;
			TextComponent->SetPosition(pos);
			TextComponent->SetHeight(rect.height);
			TextComponent->SetWidth(TextComponent->GetTextSize().x);
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

		//  Component 
		copy->SetEnabled(IsEnabled());
		copy->m_owner = owner;
		copy->PostInit();

		// UIElement
		copy->m_isDisabled = m_isDisabled;
		copy->DisabledColor = DisabledColor;
		copy->TintColor = TintColor;
		copy->SetAnchor(AnchorPos);
		copy->SetRectangle(GetRectangleNative());


		copy->TextComponent->Text = TextComponent->Text;
		copy->TextComponent->SetFont(&TextComponent->GetFont());
		copy->TextComponent->AlignHorizontal = TextComponent->AlignHorizontal;
		copy->TextComponent->AlignVertical = TextComponent->AlignVertical;

		copy->m_bState = m_bState;

		return copy;
	}
}

// initialize EventHandler for OnStateChangedEvent
int AIngine::Events::EventHandler<void, bool>::counter = 0;