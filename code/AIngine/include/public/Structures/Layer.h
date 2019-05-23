#pragma once

#include <string>
#include "Events/Event.h"

namespace AIngine::Structures {

	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float delta) {}
		virtual void OnEvent(AIngine::Events::Event& e) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
	};
}