#include "AIngine/Graphics.h"
#include "AIngine/Macros.h"
#include "Assets/Assets.h"
#include "Rendering/Camera.h"
#include "Debug/log.h"

#include "imgui.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <glad/glad.h>
#include <algorithm>
#include <map>
#include <sstream>

namespace AIngine {

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
			m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

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

			glm::mat4 projection = AIngine::Rendering::Camera::Get().GetProjectionMatrix();
			glm::mat4 view = AIngine::Rendering::Camera::Get().GetViewMatrix();
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
		AIngine::Rendering::GLShaderProgram* m_shader;
	};

	struct GLRenderLines
	{
		void Create()
		{
			std::string vs("assets/Intellgine/shader/debug/vertex.glsl");
			std::string fs("assets/Intellgine/shader/debug/fragment.glsl");

			std::string path;
			path.append(vs).append(";").append(fs);
			m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

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

			glm::mat4 projection = AIngine::Rendering::Camera::Get().GetProjectionMatrix();
			glm::mat4 view = AIngine::Rendering::Camera::Get().GetViewMatrix();
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
		AIngine::Rendering::GLShaderProgram* m_shader;
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
			m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(path)->GetShader();

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

			glm::mat4 projection = AIngine::Rendering::Camera::Get().GetProjectionMatrix();
			glm::mat4 view = AIngine::Rendering::Camera::Get().GetViewMatrix();
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
		AIngine::Rendering::GLShaderProgram* m_shader;

	};

	struct GLText {

		struct TextElement {
			std::string msg;
			AIngine::Rendering::Font* font;
			glm::vec2 position;
			glm::vec2 scale;
			glm::vec3 color;
			float alpha;
		};

		void Create() {
			//"assets/Intellgine/fonts/arial.ttf"
			// load shader
			std::string vs("assets/Intellgine/shader/screenshader/vertexText.glsl");
			std::string fs("assets/Intellgine/shader/screenshader/fragmentText.glsl");
			std::string path;
			path.append(vs).append(";").append(fs);
			m_shader = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::ShaderAsset>(path)->GetShader();
			m_shader->SetInteger("text", 0, false);
			std::stringstream ss;
			ss << "assets/Intellgine/fonts/arial.ttf";
			ss << "\n";
			ss << "50";
			m_standardFont = &AIngine::Assets::AssetRegistry::Load<AIngine::Assets::FontAsset>(ss.str())->GetFont();


			// Configure VAO/VBO for texture quads
			glGenVertexArrays(1, &m_vao);
			glGenBuffers(1, &m_vbo);
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			m_count = 0;
		}

		void AddText(std::string text, glm::vec2 screenPosition, glm::vec2 scale, glm::vec3 color, float alpha, AIngine::Rendering::Font* font = nullptr) {
			if (m_count <= e_maxTextElements) {
				if (!font)
					font = m_standardFont;
				textStack[m_count] = TextElement
				{ text,
					font,
					screenPosition,
					scale,
					color,
					alpha
				};
				m_count++;
			}
		}

		void Destroy() {
			if (m_vao)
			{
				glDeleteVertexArrays(1, &m_vao);
				glDeleteBuffers(1, &m_vbo);
				m_vao = 0;
				m_vbo = 0;
				m_count = 0;
			}
		}

		void Flush() {
			if (m_count == 0)
				return;

			glm::mat4 projection = AIngine::Rendering::Camera::Get().GetProjectionMatrix();
			m_shader->SetMatrix4("projection", projection, true);
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(m_vao);

			// Set OpenGL options
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			for (TextElement& textElement : textStack)
			{
				m_shader->SetVector3f("textColor", textElement.color);
				m_shader->SetFloat("alpha", textElement.alpha);
				GLfloat x = textElement.position.x;
				GLfloat y = textElement.position.y;

				// Iterate through all characters
				std::string::const_iterator c;
				for (c = textElement.msg.begin(); c != textElement.msg.end(); c++)
				{
					AIngine::Rendering::RenderableCharacter ch = textElement.font->Characters[*c];

					GLfloat xpos = x + ch.Bearing.x * textElement.scale.x;
					GLfloat ypos = y + (textElement.font->Characters['H'].Bearing.y - ch.Bearing.y) * textElement.scale.y;

					GLfloat w = ch.Size.x * textElement.scale.x;
					GLfloat h = ch.Size.y * textElement.scale.y;
					// Update VBO for each character
					GLfloat vertices[6][4] = {
						{ xpos,     ypos + h,   0.0, 1.0 },
						{ xpos + w, ypos,       1.0, 0.0 },
						{ xpos,     ypos,       0.0, 0.0 },

						{ xpos,     ypos + h,   0.0, 1.0 },
						{ xpos + w, ypos + h,   1.0, 1.0 },
						{ xpos + w, ypos,       1.0, 0.0 }
					};
					// Render glyph texture over quad
					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					// Update content of VBO memory
					glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

					glBindBuffer(GL_ARRAY_BUFFER, 0);
					// Render quad
					glDrawArrays(GL_TRIANGLES, 0, 6);
					// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.Advance >> 6) * textElement.scale.x; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
				}
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_count = 0;
		}

		GLuint m_vao;
		GLuint m_vbo;
		enum { e_maxTextElements = 1024 };
		int32 m_count;
		TextElement textStack[e_maxTextElements];
		AIngine::Rendering::GLShaderProgram* m_shader;
		AIngine::Rendering::Font* m_standardFont;

	};
	Graphics* Graphics::s_instance = nullptr;


	Graphics::Graphics() {
		ASSERT(!s_instance, "An entity of type Graphics already exists!");
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

	Graphics::~Graphics() {
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

	void Graphics::Flush() {
		m_triangles->Flush();
		m_lines->Flush();
		m_points->Flush();
		m_text->Flush();
	}

	void Graphics::Line(const glm::vec2 & v1World, const glm::vec2 & v2World, const glm::vec3 & color)
	{
		Line(b2Vec2(v1World.x, v1World.y), b2Vec2(v2World.x, v2World.y), b2Color(color.x, color.y, color.z));
	}

	void Graphics::LineScreen(const glm::vec2 & v1Screen, const glm::vec2 & v2Screen, const glm::vec3 & color)
	{
		Line(AIngine::Rendering::Camera::Get().ScreenToWorldPoint(v1Screen), AIngine::Rendering::Camera::Get().ScreenToWorldPoint(v2Screen), color);
	}

	void Graphics::Line(const b2Vec2 & v1World, const b2Vec2 & v2World, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_lines->Vertex(v1World, color);
			s_instance->m_lines->Vertex(v2World, color);
		}
	}

	void Graphics::Triangle(const glm::vec2 & v1World, const glm::vec2 & v2World, const glm::vec2 & v3World, const glm::vec3 & color)
	{
		Triangle(b2Vec2(v1World.x, v1World.y), b2Vec2(v2World.x, v2World.y), b2Vec2(v3World.x, v3World.y), b2Color(color.x, color.y, color.z));
	}

	void Graphics::TriangleScreen(const glm::vec2 & v1Screen, const glm::vec2 & v2Scsreen, const glm::vec2 & v3Screen, const glm::vec3 & color)
	{
		Triangle(AIngine::Rendering::Camera::Get().ScreenToWorldPoint(v1Screen),
			AIngine::Rendering::Camera::Get().ScreenToWorldPoint(v2Scsreen),
			AIngine::Rendering::Camera::Get().ScreenToWorldPoint(v3Screen),
			color);
	}

	void Graphics::Triangle(const b2Vec2 & v1World, const b2Vec2 & v2World, const b2Vec2 & v3World, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_triangles->Vertex(v1World, color);
			s_instance->m_triangles->Vertex(v2World, color);
			s_instance->m_triangles->Vertex(v3World, color);
		}

	}

	void Graphics::Point(const glm::vec2 & vWorl, float32 size, const glm::vec3 & color)
	{
		Point(b2Vec2(vWorl.x, vWorl.y), size, b2Color(color.x, color.y, color.z));
	}

	void Graphics::PointScreen(const glm::vec2 & vScreen, float32 size, const glm::vec3 & color)
	{
		Point(AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vScreen), size, color);
	}

	void Graphics::Point(const b2Vec2 & vWorl, float32 size, const b2Color & color)
	{
		if (s_instance) {
			s_instance->m_points->Vertex(vWorl, color, size);
		}
	}

	void Graphics::Text(const std::string& text, const glm::vec2 & screenPosition, const glm::vec2 & scale, const glm::vec3 & color, float alpha, AIngine::Rendering::Font* font)
	{
		if (s_instance) {
			s_instance->m_text->AddText(text, screenPosition, scale, color, alpha, font);
		}
	}

	void Graphics::CircleWorld(const glm::vec2 & center, float radius, const glm::vec3 & color)
	{
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 sinInc = sinf(k_increment);
		float32 cosInc = cosf(k_increment);
		glm::vec2 r1(1.0f, 0.0f);
		glm::vec2 v1 = center + radius * r1;
		for (int i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = center + radius * r2;
			Line(v1, v2, color);
			r1 = r2;
			v1 = v2;
		}
	}

	void Graphics::CircleScreen(const glm::vec2 & center, float radius, const glm::vec3 & color)
	{
		CircleWorld(AIngine::Rendering::Camera::Get().ScreenToWorldPoint(center), radius, color);
	}

	void Graphics::BoxWorld(const glm::vec2 * vertices, const glm::vec3 color)
	{
		glm::vec2 p1 = vertices[4 - 1];
		for (int i = 0; i < 4; ++i)
		{
			glm::vec2 p2 = vertices[i];
			Line(p1, p2, color);
			p1 = p2;
		}
	}

	void Graphics::BoxWorld(const AIngine::Structures::RectangleI & rect, const glm::vec3 color)
	{
		glm::vec2 vertices[4];
		vertices[0] = rect.GetPosition();
		vertices[1] = rect.GetBottomLeft();
		vertices[2] = rect.GetMax();
		vertices[3] = rect.GetTopRight();

		glm::vec2 p1 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[4 - 1]);
		for (int i = 0; i < 4; ++i)
		{
			glm::vec2 p2 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[i]);
			Line(p1, p2, color);
			p1 = p2;
		}
	}

	void Graphics::BoxScreen(const glm::vec2 * vertices, const glm::vec3 color)
	{
		glm::vec2 p1 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[4 - 1]);
		for (int i = 0; i < 4; ++i)
		{
			glm::vec2 p2 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[i]);
			Line(p1, p2, color);
			p1 = p2;
		}
	}

	void Graphics::BoxScreen(const AIngine::Structures::RectangleI & rect, const glm::vec3 color)
	{
		glm::vec2 vertices[4];
		vertices[0] = rect.GetPosition();
		vertices[1] = rect.GetBottomLeft();
		vertices[2] = rect.GetMax();
		vertices[3] = rect.GetTopRight();

		glm::vec2 p1 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[4 - 1]);
		for (int i = 0; i < 4; ++i)
		{
			glm::vec2 p2 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[i]);
			Line(p1, p2, color);
			p1 = p2;
		}
	}

	void Graphics::PolygonWorld(const glm::vec2 * vertices, unsigned int count, const glm::vec3 & color)
	{
		glm::vec2 p1 = vertices[count - 1];
		for (unsigned int i = 0; i < count; ++i)
		{
			glm::vec2 p2 = vertices[i];
			Line(p1, p2, color);
			p1 = p2;
		}
	}

	void Graphics::PolygonScreen(const glm::vec2 * vertices, unsigned int count, const glm::vec3 & color)
	{
		glm::vec2 p1 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[4 - 1]);
		for (unsigned int i = 0; i < count; ++i)
		{
			glm::vec2 p2 = AIngine::Rendering::Camera::Get().ScreenToWorldPoint(vertices[i]);
			Line(p1, p2, color);
			p1 = p2;
		}
	}

}