#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "DxSystem.h"
#include "Shader.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;

	class Debug_Draw_Manager : public btIDebugDraw
	{
	public:
		Debug_Draw_Manager();
		virtual ~Debug_Draw_Manager() {}

		virtual void setDebugMode(int debugMode) { bit_Debug_Mode = debugMode; }
		virtual int	 getDebugMode() const { return bit_Debug_Mode; }

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
		virtual void flushLines();
		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
		{
			drawLine(PointOnB, PointOnB + normalOnB * distance, color);
			btVector3 ncolor(0, 0, 0);
			drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
		}
		virtual DefaultColors getDefaultColors() const{	return default_color;}

		DefaultColors default_color;

		//未対応
		virtual void reportErrorWarning(const char* warningString) {}
		virtual void draw3dText(const btVector3& location, const char* textString) {}

		void Render_Collider();
		void Render_Grid(std::shared_ptr<BeastEngine::Transform>& trans);

	private:
		void Set_Dx_Settings();

		int bit_Debug_Mode = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_debug;
		std::shared_ptr<BeastEngine::Shader> shader;

		struct Vertex
		{
			Vertex() {}
			Vertex(const btVector3& p, const btVector3& c);
			float pos[3];
			float color[3];
		};
		struct Line
		{
			Line() {}
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
			BeastEngine::Vector3 pos;
			BeastEngine::Vector3 col;
		};
		const size_t max_line = 30000;
		std::vector<Line> lines;

		int grid_length;
	};
}