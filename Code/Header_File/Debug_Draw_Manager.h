#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "Shader.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Material;

	//�f�o�b�O�`��N���X
	class Debug_Draw_Manager final : public btIDebugDraw
	{
	public:
		Debug_Draw_Manager();
		~Debug_Draw_Manager() override = default;

		void setDebugMode(int debugMode) override { bit_debug_mode = debugMode; }  //Bullet��������Ăяo�����߂̊֐�
		[[nodiscard]] int getDebugMode() const override { return bit_debug_mode; } //Bullet��������Ăяo�����߂̊֐�

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override; //���C����`�惊�X�g�ɓo�^����
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override; //���C����`�惊�X�g�ɓo�^����
		void flushLines() override; // ���C���`����s

		//Bullet��������Ăяo�����߂̊֐�
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override
		{
			drawLine(PointOnB, PointOnB + normalOnB * distance, color);
			const btVector3 n_color(0, 0, 0);
			drawLine(PointOnB, PointOnB + normalOnB * 0.01f, n_color);
		}

		[[nodiscard]] DefaultColors getDefaultColors() const override { return default_color; } //Bullet��������Ăяo�����߂̊֐�

		DefaultColors default_color;

		void reportErrorWarning(const char* warningString) override {} //���Ή�
		void draw3dText(const btVector3& location, const char* textString) override {} //���Ή�

		void Render_Collider() const; //Bullet�̃R���C�_�[�f�o�b�O�`����Ă�
		void Render_Grid(std::shared_ptr<Transform>& trans) const; //�V�[���r���[�̃O���b�h����`�悷��

	private:
		void Set_Dx_Settings() const; //�V�F�[�_�̃Z�b�g�Ȃ� �`����s�̑O�ɌĂ�

		int bit_debug_mode = 0;
		std::shared_ptr<Material> material;

		//Bullet�Ŏg�p���钸�_�f�[�^
		struct Vertex
		{
			Vertex() = default;
			Vertex(const btVector3& p, const btVector3& c);
			float pos[3]{};
			float color[4]{};
		};
		//�`���
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
		//�G���W�����Ŏg�p���钸�_�f�[�^
		struct VertexData
		{
			Vector3 pos;
			Vector4 col;
		};
		std::vector<Line> lines{};

		int grid_length;
	};
}