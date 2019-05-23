#pragma once

#include "Editor/Editor.h"
#include "Editor/ImGuiWidget.h"

#include "spdlog/details/null_mutex.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/fmt/ostr.h"
#include <mutex>

namespace AIngine::Editor {

	class LogWidget : public ImGuiWidget
	{
	private:
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
		bool                AutoScroll;
		bool                ScrollToBottom;

	public:
		LogWidget()
		{
			AutoScroll = true;
			ScrollToBottom = false;
			Clear();
		}

		~LogWidget()
		{
			Clear();
		}

		void    Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
			if (AutoScroll)
				ScrollToBottom = true;
		}

		virtual void OnImGuiRender() override
		{
			static char const* title = "Log";
			static bool p_open = true;

			if (!ImGui::Begin(title, &p_open))
			{
				ImGui::End();
				return;
			}

			m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			m_position = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);


			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				if (ImGui::Checkbox("Auto-scroll", &AutoScroll))
					if (AutoScroll)
						ScrollToBottom = true;
				ImGui::EndPopup();
			}

			// Main window
			//if (ImGui::Button("Options"))
			//	ImGui::OpenPopup("Options");
			//ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);

			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have a random access on the result on our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
				// especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
				// Here we instead demonstrate using the clipper to only process lines that are within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
				// Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
				// both of which we can handle since we an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
				// Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			if (ScrollToBottom)
				ImGui::SetScrollHereY(1.0f);
			ScrollToBottom = false;
			ImGui::EndChild();
			ImGui::End();
		}

	private:
		ImVec4 GetColor(const std::string msg) {
			ImVec4 color(1, 1, 1, 1);

			if (msg.find("Warning") != std::string::npos)
				color = ImVec4(1, 1, 0, 1);

			if (msg.find("Error") != std::string::npos)
				color = ImVec4(1, 0, 0, 1);

			return color;
		}
	};


	template<typename Mutex>
	class LogWidgetSink : public spdlog::sinks::base_sink <Mutex>
	{
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			LogWidget* widget = dynamic_cast<LogWidget*>(AIngine::Editor::Editor::GetWidget<LogWidget>());
			if (widget) {
				fmt::memory_buffer formatted;
				sink::formatter_->format(msg, formatted);

				widget->AddLog(fmt::to_string(formatted).c_str());
			}
		}

		void flush_() override {
			LogWidget* widget = dynamic_cast<LogWidget*>(AIngine::Editor::Editor::GetWidget<LogWidget>());
			if (widget) {
				widget->Clear();
			}
		}

	};

	using LogWidgetSink_mt = LogWidgetSink<std::mutex>;
	using LogWidgetSink_st = LogWidgetSink<spdlog::details::null_mutex>;

}