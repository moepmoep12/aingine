namespace AIngine::Editor {

	class ImGuiWidget {
	public:
		virtual void OnImGuiRender() = 0;
		virtual ~ImGuiWidget() {}
	};
}