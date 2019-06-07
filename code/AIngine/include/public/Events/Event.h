#pragma once

#include <functional>
#include <string>
#include <vector>

namespace AIngine::Events {

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		ViewportChanged,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};


	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

	class EventData
	{

	public:
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

	};

	class EventDispatcher
	{
		template<typename T>

		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(EventData& eventData)
			: m_EventData(eventData)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_EventData.GetEventType() == T::GetStaticType())
			{
				m_EventData.Handled = func(*(T*)&m_EventData);
				return true;
			}
			return false;
		}
	private:
		EventData& m_EventData;
	};


	template<typename ReturnType, class ... Args>
	class EventHandler {
	public:
		using Func = typename std::function<ReturnType(Args...)>;

	private:
		Func _func;

	public:
		int id;
		static int counter;

		EventHandler() : id{ 0 } {
		}

		EventHandler(const Func &func) : _func{ func } {
			this->id = ++EventHandler::counter;
		}

		void operator()(Args... args) {
			this->_func(args...);
		}

		void operator=(const EventHandler &func) {
			if (this->_func == nullptr) {
				this->_func = func;
				this->id = ++EventHandler::counter;
			}
		}

		bool operator==(const EventHandler &del) {
			return this->id == del.id;
		}
		bool operator!=(nullptr_t) {
			return this->_func != nullptr;
		}
	};

	template<typename ReturnType, class ... Args>
	class Event {
		using EventHandler = EventHandler<ReturnType, Args...>;
		std::vector<std::unique_ptr<EventHandler>> handlers;

		void notifyHandlers(Args... args) {
			std::vector<std::unique_ptr<EventHandler>>::iterator func = this->handlers.begin();
			for (; func != this->handlers.end(); ++func) {
				if (*func != nullptr && (*func)->id != 0) {
					(*(*func))(args...);
				}
			}
		}
	public:
		void addHandler(const EventHandler &handler) {
			this->handlers.push_back(std::unique_ptr<EventHandler>(new EventHandler{ handler }));
		}

		void removeHandler(const EventHandler &handler) {
			std::vector<std::unique_ptr<EventHandler>>::iterator to_remove = this->handlers.begin();
			for (; to_remove != this->handlers.end(); ++to_remove) {
				if (*(*to_remove) == handler) {
					this->handlers.erase(to_remove);
					break;
				}
			}
		}

		void operator()(Args... args) {
			this->notifyHandlers(args...);
		}

		Event &operator+=(const EventHandler &handler) {
			this->addHandler(handler);

			return *this;
		}

		Event &operator+=(const typename EventHandler::Func &handler) {
			this->addHandler(EventHandler{ handler });

			return *this;
		}

		Event &operator-=(const EventHandler &handler) {
			this->removeHandler(handler);

			return *this;
		}

		//virtual std::string ToString() const = 0;
	};

	template<typename ReturnType, class ... Args>
	inline std::ostream& operator<<(std::ostream& os, const EventData& e)
	{
		return os << e.ToString();
	}

}