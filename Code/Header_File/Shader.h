#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include "Object.h"

namespace BeastEngine
{
	class Material;

	//�V�F�[�_�[
	class Shader
	{
	public:
		virtual ~Shader() = default;

		//�V�F�[�_�[�̎��
		enum class Shader_Type
		{
			Vertex,
			Geometry,
			Pixel,
			Hull,
			Domain
		};

		//�t�@�C���p�X����V�F�[�_�[���R���p�C������������
		static std::shared_ptr<Shader> Create(const std::string& shader_path, const Shader_Type& shader_type);

	protected:
		//���t���N�V�������ꂽ�R���X�^���g�o�b�t�@�̕ϐ��̌^
		enum class Parameter_Type
		{
			Int,
			Float,
			Vector2,
			Vector3,
			Vector4,
			Matrix
		};

		//�R���X�^���g�o�b�t�@�̕ϐ��̃��t���N�V�������
		struct Parameter_Info
		{
			std::string name;
			Parameter_Type type;
			std::vector<std::byte> default_value;
			UINT size;
			UINT offset;
		};

		//�e�N�X�`���ϐ��̃��t���N�V�������
		struct Texture_Info
		{
			std::string name;
			UINT register_number;
		};

		//�R���X�^���g�o�b�t�@�̃��t���N�V�������
		struct ConstantBuffer_Info
		{
			std::string name;
			UINT register_number = 0;
			UINT byte_size = 0;
			std::vector<Parameter_Info> parameters{};
		};

		virtual bool Initialize(const std::string& filename) { return false; }; //������
		virtual void Activate() {}; //�V�F�[�_�[���X�e�[�W����
		static HRESULT Compile(const WCHAR* filename, LPCSTR method, LPCSTR shader_model, ID3DBlob** pp_blob_out); //�R���p�C������
		void Reflect_Resource_Buffer(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector); //�V�F�[�_�[�����t���N�V�������ăR���X�^���g�o�b�t�@�����擾��r

		std::vector<ConstantBuffer_Info> constant_buffer_info;
		std::vector<Texture_Info> texture_info;

		friend class Material;
	};

	// ���_�V�F�[�_
	class Vertex_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		void Reflect_InputLayout(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector, const Microsoft::WRL::ComPtr<ID3D10Blob>& blob);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;
	};

	// �s�N�Z���V�F�[�_
	class Pixel_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr;
	};

	// �W�I���g���V�F�[�_
	class Geometry_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs = nullptr;
	};

	// �n���V�F�[�_
	class Hull_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11HullShader> hs = nullptr;
	};

	// �h���C���l�[���V�F�[�_
	class Domain_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> ds = nullptr;
	};
}