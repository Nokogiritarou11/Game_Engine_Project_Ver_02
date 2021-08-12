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
	class Material;

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

		static std::shared_ptr<BeastEngine::Shader> Create(const std::string& shader_path, const Shader_Type& shader_type);

	protected:
		enum class Parameter_Type
		{
			INT,
			FLOAT,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			MATRIX
		};

		struct Parameter_Info
		{
			std::string name;
			Parameter_Type type;
			UINT size;
			UINT offset;
		};

		struct Texture_Info
		{
			std::string name;
			UINT register_number;
		};

		struct ConstantBuffer_Info
		{
			std::string name;
			UINT register_number;
			UINT byte_size;
			std::vector<Parameter_Info> parameters;
		};

		virtual bool Initialize(const std::string& filename) { return false; };
		virtual void Activate() {};
		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
		void Reflect_Resource_Buffer(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector);

		std::vector<ConstantBuffer_Info> constant_buffer_info;
		std::vector<Texture_Info> texture_info;

		friend class BeastEngine::Material;
	};

	class Vertex_Shader : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		void Reflect_InputLayout(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector, Microsoft::WRL::ComPtr<ID3DBlob>& blob);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr; // 頂点シェーダ
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;
	};

	class Pixel_Shader : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr; // ピクセルシェーダ
	};

	class Geometry_Shader : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs = nullptr; // ジオメトリシェーダ
	};

	class Hull_Shader : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11HullShader> hs = nullptr; // ハルシェーダ
	};

	class Domain_Shader : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> ds = nullptr; // ドメインネームシェーダ
	};
}