#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "Shader.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Material;

	//デバッグ描画クラス
	class Debug_Draw_Manager final : public btIDebugDraw
	{
	public:
		Debug_Draw_Manager();
		~Debug_Draw_Manager() override = default;

		void setDebugMode(int debugMode) override { bit_debug_mode = debugMode; }  //Bullet内部から呼び出すための関数
		[[nodiscard]] int getDebugMode() const override { return bit_debug_mode; } //Bullet内部から呼び出すための関数

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override; //ラインを描画リストに登録する
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override; //ラインを描画リストに登録する
		void flushLines() override; // ライン描画実行

		//Bullet内部から呼び出すための関数
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
		{
			drawLine(PointOnB, PointOnB + normalOnB * distance, color);
			const btVector3 n_color(0, 0, 0);
			drawLine(PointOnB, PointOnB + normalOnB * 0.01f, n_color);
		}

		[[nodiscard]] DefaultColors getDefaultColors() const override { return default_color; } //Bullet内部から呼び出すための関数

		DefaultColors default_color;

		void reportErrorWarning(const char* warningString) override {} //未対応
		void draw3dText(const btVector3& location, const char* textString) override {} //未対応

		void Render_Collider() const; //Bulletのコライダーデバッグ描画を呼ぶ
		void Render_Grid(std::shared_ptr<Transform>& trans) const; //シーンビューのグリッド線を描画する

	private:
		void Set_Dx_Settings() const; //シェーダのセットなど 描画実行の前に呼ぶ

		int bit_debug_mode = 0;
		std::shared_ptr<Material> material;

		//Bulletで使用する頂点データ
		struct Vertex
		{
			Vertex() = default;
			Vertex(const btVector3& p, const btVector3& c);
			float pos[3]{};
			float color[4]{};
		};
		//描画線
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
		//エンジン側で使用する頂点データ
		struct VertexData
		{
			Vector3 pos;
			Vector4 col;
		};
		std::vector<Line> lines{};

		int grid_length;
	};
}