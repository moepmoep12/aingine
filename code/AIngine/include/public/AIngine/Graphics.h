#pragma once

#include "Rendering/Font.h"
#include "Structures/Rectangle.h"

#include <glm/glm.hpp>
#include <Box2D/Box2D.h>

namespace AIngine {

	struct GLRenderPoints;
	struct GLRenderLines;
	struct GLRenderTriangles;
	struct GLText;

	class Graphics final {
	public:
		Graphics();
		~Graphics();

		void Flush();

		/* Low level API for drawing primitives*/

		static void Line(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void LineScreen(const glm::vec2& v1Screen, const glm::vec2& v2Screen, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void Line(const b2Vec2& v1World, const b2Vec2& v2World, const b2Color& color = b2Color(0.0, 1.0, 0.0));

		static void Triangle(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec2& v3World, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void TriangleScreen(const glm::vec2& v1Screen, const glm::vec2& v2Scsreen, const glm::vec2& v3Screen, const glm::vec3& color = glm::vec3(0, 1, 0));
		static void Triangle(const b2Vec2& v1World, const b2Vec2& v2World, const b2Vec2& v3World, const b2Color& color = b2Color(0.0, 1.0, 0.0));

		static void Point(const glm::vec2& vWorl, float32 size, const glm::vec3& color);
		static void PointScreen(const glm::vec2& vScreen, float32 size, const glm::vec3& color);
		static void Point(const b2Vec2& vWorl, float32 size, const b2Color& color);

		static void Text(const char* text, const glm::vec2& screenPosition, const glm::vec2& scale = glm::vec2(1.0), const glm::vec3& color = glm::vec3(1.0), float alpha = 1.0f, AIngine::Rendering::Font* font = nullptr);

		/* high level API for drawing*/

		static void CircleWorld(const glm::vec2& center, float radius, const glm::vec3& color);
		static void CircleScreen(const glm::vec2& center, float radius, const glm::vec3& color);

		static void BoxWorld(const glm::vec2* vertices, const glm::vec3 color);
		static void BoxWorld(const AIngine::Structures::RectangleI& rect, const glm::vec3 color);
		static void BoxScreen(const glm::vec2* vertices, const glm::vec3 color);
		static void BoxScreen(const AIngine::Structures::RectangleI& rect, const glm::vec3 color);

		static void PolygonWorld(const glm::vec2* vertices, unsigned int count, const glm::vec3& color);
		static void PolygonScreen(const glm::vec2* vertices, unsigned int count, const glm::vec3& color);

	private:
		GLRenderPoints* m_points;
		GLRenderLines* m_lines;
		GLRenderTriangles* m_triangles;
		GLText*  m_text;
		static Graphics* s_instance;
	};
}