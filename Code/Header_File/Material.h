#pragma once
#include "DxSystem.h"
#include "Shader.h"
#include <unordered_map>
#include <cstddef>

namespace BeastEngine
{
	class SkinMesh_Renderer;
	class Mesh_Renderer;
	class Sprite_Renderer;
	class Gaussian_Filter;
	class SkyBox;
	class Render_Manager;
	class Shadow_Manager;
	class Debug_Draw_Manager;
	class Texture;

	//�}�e���A��
	class Material : public Object
	{
	public:
		//�����_�����O�̎��
		enum class Rendering_Mode
		{
			Opaque,
			Cutout,
			Transparent
		};

		int render_queue = 2000; //�����_�[�L���[ �l���Ⴂ�قǐ�ɕ`�悳���

		static std::shared_ptr<Material> Create(const std::string& vertex_path, const std::string& pixel_path = "", const std::string& geometry_path = ""); //�V�K�쐬
		static std::shared_ptr<Material> Load_Material(const std::string& full_path); //�t�@�C���p�X����ǂݍ���

		void Save(const std::string& path = ""); //�t�@�C���p�X�Ƀt�@�C�������� �������ȗ������ꍇ�㏑���Z�[�u

		void Set_Rendering_Mode(Rendering_Mode mode); //�����_�����O���[�h��ݒ肷��
		void Set_Shader(const std::string& path, Shader::Shader_Type shader_type, int pass = 0); //�V�F�[�_�[��ǂݍ��݃Z�b�g����
		void Set_Blend_State(BS_State state, int pass = 0);         //�u�����h�X�e�[�g��ݒ肷��
		void Set_Rasterizer_State(RS_State state, int pass = 0);    //���X�^���C�U�[�X�e�[�g��ݒ肷��
		void Set_Depth_Stencil_State(DS_State state, int pass = 0); //�f�v�X�X�e���V���X�e�[�g��ݒ肷��

		void Set_Texture(const std::string& texture_name, const std::shared_ptr<Texture>& texture); //�e�N�X�`����ݒ肷��
		void Set_Int(const std::string& int_name, const int& value);            //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Set_Float(const std::string& float_name, const float& value);      //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Set_Vector2(const std::string& vector_name, const Vector2& value); //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Set_Vector3(const std::string& vector_name, const Vector3& value); //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Set_Vector4(const std::string& vector_name, const Vector4& value); //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Set_Matrix(const std::string& matrix_name, const Matrix& value);   //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��

		void Add_Pass(const std::string& vertex_path, const std::string& pixel_path = "", const std::string& geometry_path = ""); //�����_�����O�p�X��ǉ�����
		void Remove_Pass(); //�����_�����O�p�X���폜����

		std::shared_ptr<Texture> Get_Texture(const std::string& texture_name); //�e�N�X�`����ϐ�������擾����
		int Get_Int(const std::string& int_name);                              //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����
		float Get_Float(const std::string& float_name);                        //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����
		Vector2 Get_Vector2(const std::string& vector_name);                   //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����
		Vector3 Get_Vector3(const std::string& vector_name);                   //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����
		Vector4 Get_Vector4(const std::string& vector_name);                   //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����
		Matrix Get_Matrix(const std::string& matrix_name);                     //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������擾����

	private:
		//�V�F�[�_�[���
		struct Shader_Info
		{
			std::string shader_path;
			std::shared_ptr<Shader> shader;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(shader_path);
			}
		};

		//�R���X�^���g�o�b�t�@
		struct ConstantBuffer_Info
		{
			UINT register_number = 0;
			std::vector<Shader::Shader_Type> staging_shader{};
			std::vector<std::byte> byte_data{};
			Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(register_number, staging_shader, byte_data);
			}
		};

		//�R���X�^���g�o�b�t�@���̃p�����[�^
		struct Parameter_Info
		{
			Shader::Parameter_Type type;
			std::string parent_name;
			UINT size = 0;
			UINT offset = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(type, parent_name, size, offset);
			}
		};

		//�e�N�X�`�����
		struct Texture_Info
		{
			UINT register_number = 0;
			std::vector<Shader::Shader_Type> staging_shader{};
			std::string texture_path;
			std::shared_ptr<Texture> texture;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(register_number, staging_shader, texture_path);
			}
		};

		//�����_�����O�p�X
		struct Render_Pass
		{
			Shader_Info shader_info[5];

			std::unordered_map<std::string, ConstantBuffer_Info> constant_buffer_info{};
			std::unordered_map<std::string, Parameter_Info> parameter_info{};
			std::unordered_map<std::string, Texture_Info> texture_info{};

			std::shared_ptr<Texture> main_texture;

			BS_State blend_state = BS_State::Off;
			RS_State rasterizer_state = RS_State::Cull_Back;
			DS_State depth_stencil_state = DS_State::GEqual;

			void Reflect_Shader();                          //�R���X�^���g�o�b�t�@�������t���N�V��������
			void Reflect_Shader(Shader::Shader_Type type);  //�R���X�^���g�o�b�t�@�������t���N�V��������
			void Reflect_Texture();                         //�V�F�[�_�[���e�N�X�`���������t���N�V��������
			void Reflect_Texture(Shader::Shader_Type type); //�V�F�[�_�[���e�N�X�`���������t���N�V��������

			static void Create_ConstantBuffer(ConstantBuffer_Info& info, const UINT& size);
			void Initialize_Texture();   //�e�N�X�`�����̏�����
			void Initialize_Shader();    //�V�F�[�_�[���̏�����
			void Activate();             //�����_�����O�p�X���A�N�e�B�u�ɂ���
			void Activate_Buffer();      //�R���X�^���g�o�b�t�@���Z�b�g
			void Activate_Texture();     //�e�N�X�`�����Z�b�g
			void Activate_Shader();      //�V�F�[�_�[���X�e�[�W����
			void Activate_State() const; //�e�X�e�[�g���Z�b�g����

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive);
		};

		std::vector<Render_Pass> render_pass;
		std::string self_save_path;
		Rendering_Mode rendering_mode = Rendering_Mode::Opaque;

		static BS_State binding_blend_state;
		static RS_State binding_rasterizer_state;
		static DS_State binding_depth_stencil_state;

		void Set_Parameter(const std::string& parameter_name, const void* value, const Shader::Parameter_Type& type); //�R���X�^���g�o�b�t�@�̃p�����[�^��ϐ�������ݒ肷��
		void Activate(int pass = 0);         //�w�肵�������_�����O�p�X���A�N�e�B�u��
		void Activate_Buffer(int pass = 0);  //�w�肵�������_�����O�p�X�̃R���X�^���g�o�b�t�@���Z�b�g����
		void Activate_Texture(int pass = 0); //�w�肵�������_�����O�p�X�̃e�N�X�`�����Z�b�g����
		void Activate_Shader(int pass = 0);  //�w�肵�������_�����O�p�X�̃V�F�[�_�[���X�e�[�W����
		void Activate_State(int pass = 0);   //�w�肵�������_�����O�p�X�̊e�X�e�[�g���Z�b�g����
		void Draw_ImGui(); //ImGui�`��

		friend class Render_Manager;
		friend class Shadow_Manager;
		friend class Debug_Draw_Manager;
		friend class Gaussian_Filter;
		friend class SkyBox;
		friend class SkinMesh_Renderer;
		friend class Mesh_Renderer;
		friend class Sprite_Renderer;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), rendering_mode, render_queue, self_save_path, render_pass);
		}
	};

	template <class Archive>
	void Material::Render_Pass::serialize(Archive& archive)
	{
		archive(shader_info, constant_buffer_info, parameter_info, texture_info, blend_state, rasterizer_state, depth_stencil_state);
	}
}

CEREAL_REGISTER_TYPE(BeastEngine::Material)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Material)
CEREAL_CLASS_VERSION(BeastEngine::Material, 1)