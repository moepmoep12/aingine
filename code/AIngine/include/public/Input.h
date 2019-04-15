#pragma once

#include <utility>

namespace AIngine {

	class Input {
	public:
		inline static bool IsKeyPressed(int keycode) { return getInstance().IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return getInstance().IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return getInstance().GetMousePositionImpl(); }
		inline static float GetMouseX() { return getInstance().GetMouseXImpl(); }
		inline static float GetMouseY() { return getInstance().GetMouseYImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input& getInstance();

	};


	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}