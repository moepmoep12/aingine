#pragma once

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>


namespace AIngine {

	struct GLRenderPoints;
	struct GLRenderLines;
	struct GLRenderTriangles;
	struct GLText;

	class Graphics {
	public:
		Graphics();
		~Graphics();

		void Flush();

		static void Line(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void Line(const b2Vec2& v1World, const b2Vec2& v2World, const b2Color& color = b2Color(0.0, 1.0, 0.0));

		static void Triangle(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec2& v3World, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void Triangle(const b2Vec2& v1World, const b2Vec2& v2World, const b2Vec2& v3World, const b2Color& color = b2Color(0.0, 1.0, 0.0));

		static void Point(const glm::vec2& vWorl, float32 size, const glm::vec3& color);
		static void Point(const b2Vec2& vWorl, float32 size, const b2Color& color);

		static void Text(int x, int y, const char *string, ...);

	private:
		GLRenderPoints* m_points;
		GLRenderLines* m_lines;
		GLRenderTriangles* m_triangles;
		GLText*  m_text;
		static Graphics* s_instance;
	};
}