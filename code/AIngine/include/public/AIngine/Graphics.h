#pragma once

#include "Rendering/Font.h"
#include "Structures/Rectangle.h"

#include <glm/glm.hpp>
#include <vector>

struct b2Vec2;
struct b2Color;

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

		/* Draws a line given in world coordinates */
		static void Line(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec4& color = glm::vec4(0, 1, 0, 1), bool instant = false);
		static void Line(const b2Vec2& v1World, const b2Vec2& v2World, const b2Color& color, bool instant = false);
		static void Lines(const std::vector<glm::vec2>& v1World, const std::vector<glm::vec2>& v2World, const std::vector<glm::vec4>& colors, bool instant = false);

		/* Draws a line given in Screen Coordinates */
		static void LineScreen(const glm::vec2& v1Screen, const glm::vec2& v2Screen, const glm::vec4& color = glm::vec4(0, 1, 0, 1), bool instant = false);
		static void LinesScreen(const std::vector<glm::vec2>& pos1, const std::vector<glm::vec2>& pos2, const std::vector<glm::vec4>& colors, bool instant = false);

		/* Draws a Triangle in World Coordinates */
		static void Triangle(const glm::vec2& v1World, const glm::vec2& v2World, const glm::vec2& v3World, const glm::vec4& color = glm::vec4(0, 1, 0, 1), bool instant = false);
		static void Triangle(const b2Vec2& v1World, const b2Vec2& v2World, const b2Vec2& v3World, const b2Color& color, bool instant = false);
		static void Triangles(const std::vector<glm::vec2>& v1World, const std::vector<glm::vec2>& v2World, const std::vector<glm::vec2>& v3World, const std::vector<glm::vec4>& colors, bool instant = false);

		/* Draws a Triangle in Screen Coordinates*/
		static void TriangleScreen(const glm::vec2& v1Screen, const glm::vec2& v2Scsreen, const glm::vec2& v3Screen, const glm::vec4& color = glm::vec4(0, 1, 0, 1), bool instant = false);
		static void TrianglesScreen(const std::vector<glm::vec2>& v1Screen, const std::vector<glm::vec2>& v2Scsreen, const std::vector<glm::vec2>& v3Screen, const std::vector<glm::vec4>& color, bool instant = false);

		static void Point(const glm::vec2& vWorl, float size, const glm::vec4& color, bool instant = false);
		static void Point(const b2Vec2& vWorl, float size, const b2Color& color, bool instant = false);
		static void Points(const std::vector<glm::vec2>& vWorl, const std::vector<float>& sizes, const std::vector< glm::vec4>& colors, bool instant = false);

		static void PointScreen(const glm::vec2& vScreen, float size, const glm::vec4& color, bool instant = false);
		static void PointsScreen(const std::vector<glm::vec2>& vScreen,const std::vector<float>& size, const std::vector<glm::vec4>& colors, bool instant = false);

		static void Text(const std::string& text, const glm::vec2& screenPosition, const glm::vec2& scale = glm::vec2(1.0), const glm::vec3& color = glm::vec3(1.0), float alpha = 1.0f, AIngine::Rendering::Font* font = nullptr);
		static void TextInstant(const std::string& text, const glm::vec2& screenPosition, const glm::vec2& scale = glm::vec2(1.0), const glm::vec3& color = glm::vec3(1.0), float alpha = 1.0f, AIngine::Rendering::Font* font = nullptr);
		static glm::vec2 GetTextSize(const std::string& text, const glm::vec2 scale = glm::vec2(1.0), AIngine::Rendering::Font* font = nullptr);

		/* high level API for drawing*/

		static void CircleWorld(const glm::vec2& center, float radius, const glm::vec4& color);
		static void CircleScreen(const glm::vec2& center, float radius, const glm::vec4& color);

		static void BoxWorld(const glm::vec2* vertices, const glm::vec4& color);
		static void BoxWorld(const AIngine::Structures::RectangleI& rect, const glm::vec4& color);
		static void BoxWorld(const AIngine::Structures::RectangleF& rect, const glm::vec4& color);
		static void BoxScreen(const glm::vec2* vertices, const glm::vec4& color);
		static void BoxScreen(const AIngine::Structures::RectangleI& rect, const glm::vec4& color);

		static void PolygonWorld(const glm::vec2* vertices, unsigned int count, const glm::vec4& color);
		static void PolygonScreen(const glm::vec2* vertices, unsigned int count, const glm::vec4& color);

	private:
		GLRenderPoints* m_points;
		GLRenderLines* m_lines;
		GLRenderTriangles* m_triangles;
		GLText*  m_text;
		static Graphics* s_instance;
	};
}