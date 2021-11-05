#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include "Object.h"

namespace BeastEngine
{
	class Material;

	//シェーダー
	class Shader
	{
	public:
		virtual ~Shader() = default;

		//シェーダーの種類
		enum class Shader_Type
		{
			Vertex,
			Geometry,
			Pixel,
			Hull,
			Domain
		};

		//ファイルパスからシェーダーをコンパイルし生成する
		static std::shared_ptr<Shader> Create(const std::string& shader_path, const Shader_Type& shader_type);

	protected:
		//リフレクションされたコンスタントバッファの変数の型
		enum class Parameter_Type
		{
			Int,
			Float,
			Vector2,
			Vector3,
			Vector4,
			Matrix
		};

		//コンスタントバッファの変数のリフレクション情報
		struct Parameter_Info
		{
			std::string name;
			Parameter_Type type;
			std::vector<std::byte> default_value;
			UINT size;
			UINT offset;
		};

		//テクスチャ変数のリフレクション情報
		struct Texture_Info
		{
			std::string name;
			UINT register_number;
		};

		//コンスタントバッファのリフレクション情報
		struct ConstantBuffer_Info
		{
			std::string name;
			UINT register_number = 0;
			UINT byte_size = 0;
			std::vector<Parameter_Info> parameters{};
		};

		virtual bool Initialize(const std::string& filename) { return false; }; //初期化
		virtual void Activate() {}; //シェーダーをステージする
		static HRESULT Compile(const WCHAR* filename, LPCSTR method, LPCSTR shader_model, ID3DBlob** pp_blob_out); //コンパイルする
		void Reflect_Resource_Buffer(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector); //シェーダーをリフレクションしてコンスタントバッファ情報を取得すr

		std::vector<ConstantBuffer_Info> constant_buffer_info;
		std::vector<Texture_Info> texture_info;

		friend class Material;
	};

	// 頂点シェーダ
	class Vertex_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		void Reflect_InputLayout(const Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& reflector, const Microsoft::WRL::ComPtr<ID3D10Blob>& blob);

		Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;
	};

	// ピクセルシェーダ
	class Pixel_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr;
	};

	// ジオメトリシェーダ
	class Geometry_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs = nullptr;
	};

	// ハルシェーダ
	class Hull_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11HullShader> hs = nullptr;
	};

	// ドメインネームシェーダ
	class Domain_Shader final : public Shader
	{
		void Activate() override;
		bool Initialize(const std::string& filename) override;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> ds = nullptr;
	};
}