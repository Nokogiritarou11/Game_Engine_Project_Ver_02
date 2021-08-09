#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Compute_Shader
	{
	public:
		static std::shared_ptr<Compute_Shader> Create(std::string shader_path);

		void Run();
		Microsoft::WRL::ComPtr <ID3D11Buffer> Get_Copy_Buffer();

		void Create_Buffer_Input(UINT size, UINT count, void* init_data);
		void Create_Buffer_Result(UINT size, UINT count, void* init_data);

	private:
		HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

		Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs = nullptr; // コンピュートシェーダ

		Microsoft::WRL::ComPtr <ID3D11Buffer> buffer_input; //計算入力用
		Microsoft::WRL::ComPtr <ID3D11Buffer> buffer_result; //結果コピー用

		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> srv_input;
		Microsoft::WRL::ComPtr <ID3D11UnorderedAccessView> uav_result;
	};
}