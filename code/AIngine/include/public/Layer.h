#pragma once

#include <string>
#include "Events/Event.h"

namespace AIngine {

	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate();
		virtual void OnEvent(AIngine::Events::Event& e);

		inline const std::string& GetName() const { return m_name; }

	private:
		std::string m_name;
	};
}