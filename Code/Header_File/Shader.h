#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl.h>
#include <utility>
#include "Object.h"
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

namespace BeastEngine
{
	class Shader
	{
	public:
		enum class Shader_Type
		{
			Vertex,
			Geometry,
			Pixel,
			Hull,
			Domain
		};

		static std::shared_ptr<BeastEngine::Shader> Create(std::string shader_path, Shader_Type shader_type);
		virtual void Activate() {};

	protected:
		virtual bool Initialize(std::string filename) { return false; };
		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
	};

	class Vertex_Shader : public Shader
	{
	public:
		void Activate() override;

	private:
		bool Initialize(std::string filename) override;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr; // ���_�V�F�[�_
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;
	};

	class Pixel_Shader : public Shader
	{
	public:
		void Activate() override;

	private:
		bool Initialize(std::string filename) override;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr; // �s�N�Z���V�F�[�_
	};

	class Geometry_Shader : public Shader
	{
	public:
		void Activate() override;

	private:
		bool Initialize(std::string filename) override;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs = nullptr; // �W�I���g���V�F�[�_
	};

	class Hull_Shader : public Shader
	{
	public:
		void Activate() override;

	private:
		bool Initialize(std::string filename) override;
		Microsoft::WRL::ComPtr<ID3D11HullShader> hs = nullptr; // �n���V�F�[�_
	};

	class Domain_Shader : public Shader
	{
	public:
		void Activate() override;

	private:
		bool Initialize(std::string filename) override;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> ds = nullptr; // �h���C���l�[���V�F�[�_
	};
}