#pragma once
#include "AIngine/KeyCodes.h"

#include <utility>

namespace AIngine {

	class Input {
	public:
		inline static bool IsKeyPressed(KeyCode key) { return getInstance().IsKeyPressedImpl(key); }

		inline static bool IsMouseButtonPressed(MouseButton mouseButton) { return getInstance().IsMouseButtonPressedImpl(mouseButton); }
		inline static std::pair<float, float> GetMousePosition() { return getInstance().GetMousePositionImpl(); }
		inline static float GetMouseX() { return getInstance().GetMouseXImpl(); }
		inline static float GetMouseY() { return getInstance().GetMouseYImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;

		virtual bool IsMouseButtonPressedImpl(MouseButton mouseButton) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input& getInstance();

	};


	class WindowsInput final : public Input {
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) override;

		virtual bool IsMouseButtonPressedImpl(MouseButton mouseButton) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}