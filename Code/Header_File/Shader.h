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
		enum Render_Type
		{
			Opaque,
			CutOut,
			Fade,
			Transparent,
			Overlay
		};

		static std::shared_ptr<BeastEngine::Shader> Create(std::string VSName, std::string PSName);

		void Activate();
		void Activate_VS();
		void Activate_PS();
		void Activate_GS();
		void Activate_HS();
		void Activate_DS();

		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader>		vs = nullptr; // 頂点シェーダ
		Microsoft::WRL::ComPtr<ID3D11PixelShader>		ps = nullptr; // ピクセルシェーダ
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>	gs = nullptr; // ジオメトリシェーダ
		Microsoft::WRL::ComPtr<ID3D11HullShader>		hs = nullptr; // ハルシェーダ
		Microsoft::WRL::ComPtr<ID3D11DomainShader>		ds = nullptr; // ドメインネームシェーダ

		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

		bool Create_VS(std::string filename);
		bool Create_PS(std::string filename);
		//bool Create_GS(std::string filename);
		//bool Create_HS(std::string filename);
		//bool Create_DS(std::string filename);

	};
}