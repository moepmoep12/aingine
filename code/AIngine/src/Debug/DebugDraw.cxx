#include "Debug/DebugDraw.h"
#include "Application.h"
#include "Debug/log.h"
#include "imgui.h"
#include <string>
#include <glad/glad.h>
#include <algorithm>

namespace AIngine::Debug {

#define BUFFER_OFFSET(x)  ((const void*) (x))

	//
	struct GLRenderPoints
	{
		void Create()
		{
			std::string vs("assets/Intellgine/shader/debug/vertexPoints.glsl");
			std::string fs("assets/Intellgine/shader/debug/fragment.glsl");

			std::string path;
			path.append(vs).append(";").append(fs);
			m_shader = &AIngine::Application::Get().GetAssetRegistry().Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

			m_vertexAttribute = 0;
			m_colorAttribute = 1;
			m_sizeAttribute = 2;

			// Generate
			glGenVertexArrays(1, &m_vaoId);
			glGenBuffers(3, m_vboIds);

			glBindVertexArray(m_vaoId);
			glEnableVertexAttribArray(m_vertexAttribute);
			glEnableVertexAttribArray(m_colorAttribute);
			glEnableVertexAttribArray(m_sizeAttribute);

			// Vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
			glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

			//sCheckGLError();

			// Cleanup
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			m_count = 0;
		}

		void Destroy()
		{
			if (m_vaoId)
			{
				glDeleteVertexArrays(1, &m_vaoId);
				glDeleteBuffers(2, m_vboIds);
				m_vaoId = 0;
			}

			//if (m_programId)
			//{
			//	glDeleteProgram(m_programId);
			//	m_programId = 0;
			//}
		}

		void Vertex(const b2Vec2& v, const b2Color& c, float32 size)
		{
			if (m_count == e_maxVertices)
				Flush();

			m_vertices[m_count] = v;
			m_colors[m_count] = c;
			m_sizes[m_count] = size;
			++m_count;
		}

		void Flush()
		{
			if (m_count == 0)
				return;

			glm::mat4 projection = AIngine::Application::Get().GetCamera().GetProjectionMatrix();
			glm::mat4 view = AIngine::Application::Get().GetCamera().GetViewMatrix();
			m_shader->SetMatrix4("projectionMatrix", projection, true);
			m_shader->SetMatrix4("view", view);

			glBindVertexArray(m_vaoId);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float32), m_sizes);

			glEnable(GL_PROGRAM_POINT_SIZE);
			glDrawArrays(GL_POINTS, 0, m_count);
			glDisable(GL_PROGRAM_POINT_SIZE);

			//sCheckGLError();

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(0);

			m_count = 0;
		}

		enum { e_maxVertices = 512 };
		b2Vec2 m_vertices[e_maxVertices];
		b2Color m_colors[e_maxVertices];
		float32 m_sizes[e_maxVertices];

		int32 m_count;

		GLuint m_vaoId;
		GLuint m_vboIds[3];
		//GLuint m_programId;
		//GLint m_projectionUniform;
		GLint m_vertexAttribute;
		GLint m_colorAttribute;
		GLint m_sizeAttribute;
		GLShaderProgram* m_shader;
	};

	struct GLRenderLines
	{
		void Create()
		{
			std::string vs("assets/Intellgine/shader/debug/vertex.glsl");
			std::string fs("assets/Intellgine/shader/debug/fragment.glsl");

			std::string path;
			path.append(vs).append(";").append(fs);
			m_shader = &AIngine::Application::Get().GetAssetRegistry().Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

			m_vertexAttribute = 0;
			m_colorAttribute = 1;

			// Generate
			glGenVertexArrays(1, &m_vaoId);
			glGenBuffers(2, m_vboIds);

			glBindVertexArray(m_vaoId);
			glEnableVertexAttribArray(m_vertexAttribute);
			glEnableVertexAttribArray(m_colorAttribute);

			// Vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

			//sCheckGLError();

			// Cleanup
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			m_count = 0;
		}

		void Destroy()
		{
			if (m_vaoId)
			{
				glDeleteVertexArrays(1, &m_vaoId);
				glDeleteBuffers(2, m_vboIds);
				m_vaoId = 0;
			}

			//if (m_shader)
			//{
			//	glDeleteProgram(m_programId);
			//	m_programId = 0;
			//}
		}

		void Vertex(const b2Vec2& v, const b2Color& c)
		{
			if (m_count == e_maxVertices)
				Flush();

			m_vertices[m_count] = v;
			m_colors[m_count] = c;
			++m_count;
		}

		void Flush()
		{
			if (m_count == 0)
				return;

			glm::mat4 projection = AIngine::Application::Get().GetCamera().GetProjectionMatrix();
			glm::mat4 view = AIngine::Application::Get().GetCamera().GetViewMatrix();
			m_shader->SetMatrix4("projectionMatrix", projection, true);
			m_shader->SetMatrix4("view", view);

			glBindVertexArray(m_vaoId);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

			glDrawArrays(GL_LINES, 0, m_count);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(0);

			m_count = 0;
		}

		enum { e_maxVertices = 2 * 512 };
		b2Vec2 m_vertices[e_maxVertices];
		b2Color m_colors[e_maxVertices];

		int32 m_count;

		GLuint m_vaoId;
		GLuint m_vboIds[2];
		//GLuint m_programId;
		GLShaderProgram* m_shader;
		//GLint m_projectionUniform;
		GLint m_vertexAttribute;
		GLint m_colorAttribute;
	};

	//
	struct GLRenderTriangles
	{
		void Create()
		{
			std::string vs("assets/Intellgine/shader/debug/vertex.glsl");
			std::string fs("assets/Intellgine/shader/debug/fragment.glsl");

			std::string path;
			path.append(vs).append(";").append(fs);
			m_shader = &AIngine::Application::Get().GetAssetRegistry().Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

			m_vertexAttribute = 0;
			m_colorAttribute = 1;

			// Generate
			glGenVertexArrays(1, &m_vaoId);
			glGenBuffers(2, m_vboIds);

			glBindVertexArray(m_vaoId);
			glEnableVertexAttribArray(m_vertexAttribute);
			glEnableVertexAttribArray(m_colorAttribute);

			// Vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);


			// Cleanup
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			m_count = 0;
		}

		void Destroy()
		{
			if (m_vaoId)
			{
				glDeleteVertexArrays(1, &m_vaoId);
				glDeleteBuffers(2, m_vboIds);
				m_vaoId = 0;
			}
		}

		void Vertex(const b2Vec2& v, const b2Color& c)
		{
			if (m_count == e_maxVertices)
				Flush();

			m_vertices[m_count] = v;
			m_colors[m_count] = c;
			++m_count;
		}

		void Flush()
		{
			if (m_count == 0)
				return;

			glm::mat4 projection = AIngine::Application::Get().GetCamera().GetProjectionMatrix();
			glm::mat4 view = AIngine::Application::Get().GetCamera().GetViewMatrix();
			m_shader->SetMatrix4("projectionMatrix", projection, true);
			m_shader->SetMatrix4("view", view);
			glm::vec2 screenPos = projection * view *  glm::vec4(glm::vec2(m_vertices[0].x, m_vertices[0].y), 0, 1);

			glBindVertexArray(m_vaoId);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);

			glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDrawArrays(GL_TRIANGLES, 0, m_count);
			glDisable(GL_BLEND);


			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(0);

			m_count = 0;
		}

		enum { e_maxVertices = 3 * 512 };
		b2Vec2 m_vertices[e_maxVertices];
		b2Color m_colors[e_maxVertices];

		int32 m_count;

		GLuint m_vaoId;
		GLuint m_vboIds[2];
		GLint m_vertexAttribute;
		GLint m_colorAttribute;
		GLShaderProgram* m_shader;

	};


	struct GLText {

		struct TextElement {
			TextElement() {}

			TextElement(int x, int y, const char* string)
				: x(x), y(y), msg(string) {}
			int x, y;
			const char* msg;
		};

		void Create() {
			m_count = 0;
		}

		void AddText(int x, int y, const char * string) {
			if (m_count <= e_maxTextElements) {
				textStack[m_count] = TextElement(x, y, string);
				m_count++;
			}
		}

		void Destroy() {

		}

		void Flush() {

			for (int i = 0; i < m_count; ++i)
			{
				ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
				ImGui::SetCursorPos(ImVec2(float(textStack[i].x), float(textStack[i].y)));
				ImGui::TextColored(ImColor(230, 153, 153, 255), textStack[i].msg);
				ImGui::End();
			}
			m_count = 0;
		}

		enum { e_maxTextElements = 1024 };
		int32 m_count;
		TextElement textStack[e_maxTextElements];
	};

	DebugDraw* DebugDraw::s_instance = nullptr;


	DebugDraw::DebugDraw() {
		ASSERT(!s_instance, "An entity of type World already exists!");
		s_instance = this;

		m_points = new GLRenderPoints;
		m_points->Create();
		m_lines = new GLRenderLines;
		m_lines->Create();
		m_triangles = new GLRenderTriangles;
		m_triangles->Create();
		m_text = new GLText;
		m_text->Create();
	}

	DebugDraw::~DebugDraw() {
		s_instance = nullptr;

		m_points->Destroy();
		delete m_points;
		m_points = NULL;

		m_lines->Destroy();
		delete m_lines;
		m_lines = NULL;

		m_triangles->Destroy();
		delete m_triangles;
		m_triangles = NULL;

		m_text->Destroy();
		delete m_text;
		m_text = NULL;
	}

	void DebugDraw::Flush() {
		m_triangles->Flush();
		m_lines->Flush();
		m_points->Flush();
		m_text->Flush();
	}

	void DebugDraw::Line(const glm::vec2 & v1World, const glm::vec2 & v2World, const glm::vec3 & color)
	{
		Line(b2Vec2(v1World.x, v1World.y), b2Vec2(v2World.x, v2World.y), b2Color(color.x, color.y, color.z));
	}

	void DebugDraw::Line(const b2Vec2 & v1World, const b2Vec2 & v2World, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_lines->Vertex(v1World, color);
			s_instance->m_lines->Vertex(v2World, color);
		}
	}

	void DebugDraw::Triangle(const glm::vec2 & v1World, const glm::vec2 & v2World, const glm::vec2 & v3World, const glm::vec3 & color)
	{
		Triangle(b2Vec2(v1World.x, v1World.y), b2Vec2(v2World.x, v2World.y), b2Vec2(v3World.x, v3World.y), b2Color(color.x, color.y, color.z));
	}

	void DebugDraw::Triangle(const b2Vec2 & v1World, const b2Vec2 & v2World, const b2Vec2 & v3World, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_triangles->Vertex(v1World, color);
			s_instance->m_triangles->Vertex(v2World, color);
			s_instance->m_triangles->Vertex(v3World, color);
		}

	}
	void DebugDraw::Point(const glm::vec2 & vWorl, float32 size, const glm::vec3 & color)
	{
		Point(b2Vec2(vWorl.x, vWorl.y), size, b2Color(color.x, color.y, color.z));
	}
	void DebugDraw::Point(const b2Vec2 & vWorl, float32 size, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_points->Vertex(vWorl, color, size);
		}
	}
	void DebugDraw::Text(int x, int y, const char * string, ...)
	{
		if (s_instance) {
			s_instance->m_text->AddText(x, y, string);
		}
	}
}