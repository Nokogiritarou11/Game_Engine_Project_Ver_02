#pragma once
#include <memory>
#include <vector>
#include <array>
#include <list>
#include "DxSystem.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Camera;
	class Mesh_Renderer;
	class SkinMesh_Renderer;
	class Sprite_Renderer;
	class Renderer;
	class Render_Texture;
	class SkyBox;
	class Material;

	class Render_Manager
	{
	public:
		Render_Manager();

		void Reset(); //���Z�b�g
		void Add(const std::shared_ptr<Mesh_Renderer>& m_rend);     //�}�l�[�W���[�Ƀ����_���[��o�^����
		void Add(const std::shared_ptr<SkinMesh_Renderer>& m_rend); //�}�l�[�W���[�Ƀ����_���[��o�^����
		void Add(const std::shared_ptr<Sprite_Renderer>& m_rend);   //�}�l�[�W���[�Ƀ����_���[��o�^����
		void Add(const std::shared_ptr<Camera>& mono);              //�}�l�[�W���[�Ƀ����_���[��o�^����
		void Render(); //�`����s

		bool render_game = true;   //�Q�[���r���[��`�悷�邩
		bool render_scene = false; //�V�[���r���[��`�悷�邩
		std::shared_ptr<Render_Texture> scene_texture{}; //�V�[���r���[�̃e�N�X�`��
		std::shared_ptr<Render_Texture> game_texture{};  //�Q�[���r���[�̃e�N�X�`��
		std::shared_ptr<Render_Texture> staging_texture{}; //�X�e�[�W�Ƀo�C���h���̃e�N�X�`��

		std::shared_ptr<Material> sprite_material;

	private:
		//�����_�����O���ɕK�v�ȃf�[�^
		struct Render_Obj
		{
			std::weak_ptr<Renderer> renderer;
			int subset_number = 0;
			int queue = 0;
			float z_distance = 0;
		};

		std::unique_ptr<SkyBox> skybox{};
		std::shared_ptr<Transform> shadow_camera_transform;
		std::shared_ptr<Camera> shadow_camera;

		std::vector<Render_Obj> opaque_list{}; //�s�����ȃ����_���[
		std::vector<Render_Obj> alpha_list{};  //�������ȃ����_���[

		std::vector<std::weak_ptr<Renderer>> renderer_3D_list{};
		std::vector<std::weak_ptr<Renderer>> renderer_2D_list{};
		std::vector<std::weak_ptr<Camera>> camera_list{};

		//�����_�����O���ʃR���X�^���g�o�b�t�@
		struct Constant_Buffer_Scene
		{
			Matrix	view_projection_matrix;
			Matrix	shadow_matrix;
			Vector4 camera_direction;
			Vector4 camera_position;
			Vector4 light_direction;
			Vector3 light_color;
			float	bias = 0;
		};
		Constant_Buffer_Scene buffer_scene;
		Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_scene;

		void Check_Renderer(); //Renderer�̐������`�F�b�N����
		void Culling_Renderer(const Vector3& view_pos, const std::array<Vector4, 6>& planes); //������J�����O���s��
		void Sort_Renderer(); //�����_���[������&�L���[�\�[�g����

		void Render_Scene(); //�V�[���r���[�̕`����s
		void Render_Game();  //�Q�[���r���[�̕`����s

		void Render_Sky(const Vector3& pos) const; //�X�J�C�{�b�N�X�̕`����s
		void Render_Shadow(const std::shared_ptr<Transform>& camera_transform); //�V���h�E�}�b�v�̕`����s
		void Render_Shadow_Directional(const Vector3& color, const float& intensity, const std::shared_ptr<Transform>& light_transform, const std::shared_ptr<Transform>& camera_transform); //�f�B���N�V���i�����C�g�̕`����s
		void Render_3D(const std::shared_ptr<Camera>& camera); //3D�I�u�W�F�N�g�̃����_�����O
		void Render_2D(); //2D�I�u�W�F�N�g�̃����_�����O

		void Update_Constant_Buffer() const; //�R���X�^���g�o�b�t�@�̍X�V
	};
}