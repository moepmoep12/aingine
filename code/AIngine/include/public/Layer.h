#pragma once

#include <string>
#include "Events/Event.h"

namespace AIngine {

	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnEvent(AIngine::Events::Event& e) = 0;

		inline const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
	};
}