#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "Shader.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Material;

	class Debug_Draw_Manager final : public btIDebugDraw
	{
	public:
		Debug_Draw_Manager();
		~Debug_Draw_Manager() override = default;

		void setDebugMode(int debugMode) override { bit_Debug_Mode = debugMode; }
		[[nodiscard]] int getDebugMode() const override { return bit_Debug_Mode; }

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void flushLines() override;

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
		{
			drawLine(PointOnB, PointOnB + normalOnB * distance, color);
			const btVector3 n_color(0, 0, 0);
			drawLine(PointOnB, PointOnB + normalOnB * 0.01f, n_color);
		}

		[[nodiscard]] DefaultColors getDefaultColors() const override { return default_color; }

		DefaultColors default_color;

		//未対応
		void reportErrorWarning(const char* warningString) override {}
		void draw3dText(const btVector3& location, const char* textString) override {}

		void Render_Collider() const;
		void Render_Grid(std::shared_ptr<Transform>& trans) const;

	private:
		void Set_Dx_Settings() const;

		int bit_Debug_Mode = 0;
		std::shared_ptr<Material> material;

		struct Vertex
		{
			Vertex() = default;
			Vertex(const btVector3& p, const btVector3& c);
			float pos[3]{};
			float color[4]{};
		};
		struct Line
		{
			Line() = default;
			Line(const Vertex& f, const Vertex& t) : from(f), to(t) {}
			Line(const btVector3& f, const btVector3& t, const btVector3& fc, const btVector3& tc)
				: from(f, fc), to(t, tc)
			{
			}

			Vertex from, to;
		};
		//頂点データ構造体
		struct VertexData
		{
			Vector3 pos;
			Vector4 col;
		};
		std::vector<Line> lines{};

		int grid_length;
	};
}