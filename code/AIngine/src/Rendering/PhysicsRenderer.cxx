#include "Rendering/PhysicsRenderer.h"
#include "AIngine/Graphics.h"

namespace AIngine::Rendering {

	void PhysicsRenderer::DrawPolygon(const b2Vec2 * vertices, int32 vertexCount, const b2Color & color)
	{
		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];
			AIngine::Graphics::Line(p1, p2, color);
			p1 = p2;
		}
	}
	void PhysicsRenderer::DrawSolidPolygon(const b2Vec2 * vertices, int32 vertexCount, const b2Color & color)
	{
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		for (int32 i = 1; i < vertexCount - 1; ++i) 
		{
			AIngine::Graphics::Triangle(vertices[0], vertices[i], vertices[i + 1], fillColor);
		}

		b2Vec2 p1 = vertices[vertexCount - 1];
		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];
			AIngine::Graphics::Line(p1, p2, color);
			p1 = p2;
		}
	}
	void PhysicsRenderer::DrawCircle(const b2Vec2 & center, float32 radius, const b2Color & color)
	{
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 sinInc = sinf(k_increment);
		float32 cosInc = cosf(k_increment);
		b2Vec2 r1(1.0f, 0.0f);
		b2Vec2 v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			AIngine::Graphics::Line(v1, v2, color);
			r1 = r2;
			v1 = v2;
		}
	}
	void PhysicsRenderer::DrawSolidCircle(const b2Vec2 & center, float32 radius, const b2Vec2 & axis, const b2Color & color)
	{
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 sinInc = sinf(k_increment);
		float32 cosInc = cosf(k_increment);
		b2Vec2 v0 = center;
		b2Vec2 r1(cosInc, sinInc);
		b2Vec2 v1 = center + radius * r1;
		b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		for (int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			AIngine::Graphics::Triangle(v0, v1, v2, fillColor);
			r1 = r2;
			v1 = v2;
		}

		r1.Set(1.0f, 0.0f);
		v1 = center + radius * r1;
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			b2Vec2 v2 = center + radius * r2;
			AIngine::Graphics::Line(v1, v2, color);
			r1 = r2;
			v1 = v2;
		}

		// Draw a line fixed in the circle to animate rotation.
		b2Vec2 p = center + radius * axis;
		AIngine::Graphics::Line(center, p, color);
	}
	void PhysicsRenderer::DrawSegment(const b2Vec2 & p1, const b2Vec2 & p2, const b2Color & color)
	{
		AIngine::Graphics::Line(p1, p2, color);
	}
	void PhysicsRenderer::DrawTransform(const b2Transform & xf)
	{
		const float32 k_axisScale = 0.4f;
		b2Color red(1.0f, 0.0f, 0.0f);
		b2Color green(0.0f, 1.0f, 0.0f);
		b2Vec2 p1 = xf.p, p2;

		p2 = p1 + k_axisScale * xf.q.GetXAxis();
		AIngine::Graphics::Line(p1, p2, red);

		p2 = p1 + k_axisScale * xf.q.GetYAxis();
		AIngine::Graphics::Line(p1, p2, green);
	}
	void PhysicsRenderer::DrawPoint(const b2Vec2 & p, float32 size, const b2Color & color)
	{
		AIngine::Graphics::Point(p, size, color);
	}
}