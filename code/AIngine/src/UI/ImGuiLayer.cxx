#include "UI/ImGuiLayer.h"
#include "imgui.h"
#include "UI/imgui_impl_opengl3.h"
#include "Application.h"
#include "KeyCodes.h"

namespace AIngine::UI {

#define BIND_EVENT_TO_FN(fn) std::bind(&fn, this, std::placeholders::_1)

	ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer")
	{
		
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.KeyMap[ImGuiKey_Tab] = KeyCodes::TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = KeyCodes::LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = KeyCodes::RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = KeyCodes::UP;
		io.KeyMap[ImGuiKey_DownArrow] = KeyCodes::DOWN;
		io.KeyMap[ImGuiKey_PageUp] = KeyCodes::PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = KeyCodes::PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = KeyCodes::HOME;
		io.KeyMap[ImGuiKey_End] = KeyCodes::END;
		io.KeyMap[ImGuiKey_Insert] = KeyCodes::INSERT;
		io.KeyMap[ImGuiKey_Delete] = KeyCodes::DEL;
		io.KeyMap[ImGuiKey_Backspace] = KeyCodes::BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = KeyCodes::SPACE;
		io.KeyMap[ImGuiKey_Enter] = KeyCodes::ENTER;
		io.KeyMap[ImGuiKey_Escape] = KeyCodes::ESCAPE;
		io.KeyMap[ImGuiKey_A] = KeyCodes::A;
		io.KeyMap[ImGuiKey_C] = KeyCodes::C;
		io.KeyMap[ImGuiKey_V] = KeyCodes::V;
		io.KeyMap[ImGuiKey_X] = KeyCodes::X;
		io.KeyMap[ImGuiKey_Y] = KeyCodes::Y;
		io.KeyMap[ImGuiKey_Z] = KeyCodes::Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		
		float time = app.GetDeltaTime();
		io.DeltaTime = time > 0.0f ? time : (1.0f / 60.0f);

		//CORE_INFO("Updating ImGuiLayer. DisplaySize {0}x{1} , Time {2}ms ", app.GetWindow().GetWidth(), app.GetWindow().GetHeight(), time);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(AIngine::Events::Event & e)
	{
		using namespace AIngine::Events;
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_TO_FN(ImGuiLayer::OnWindowResizeEvent));
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(AIngine::Events::MouseButtonPressedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(AIngine::Events::MouseButtonReleasedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(AIngine::Events::MouseMovedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(AIngine::Events::MouseScrolledEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(AIngine::Events::KeyPressedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[KeyCodes::LEFT_CONTROL] || io.KeysDown[KeyCodes::RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[KeyCodes::LEFT_SHIFT] || io.KeysDown[KeyCodes::RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[KeyCodes::LEFT_ALT] || io.KeysDown[KeyCodes::RIGHT_ALT];
		io.KeySuper = io.KeysDown[KeyCodes::LEFT_SUPER] || io.KeysDown[KeyCodes::RIGHT_SUPER];
		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(AIngine::Events::KeyReleasedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;

		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(AIngine::Events::KeyTypedEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(AIngine::Events::WindowResizeEvent & e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}
}