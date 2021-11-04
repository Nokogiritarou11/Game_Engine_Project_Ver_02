#pragma once
#include "Shader.h"

namespace BeastEngine
{
	//コンピュートシェーダー
	class Compute_Shader
	{
	public:
		static std::shared_ptr<Compute_Shader> Create(std::string shader_path); //ファイルパスからコンピュートシェーダーを作成する

		void Run(); //コンピュートシェーダーをセットし実行する
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Buffer> Get_Copy_Buffer() const;        //計算結果をバッファからコピーし取得する
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_SRV() { return srv_result; } //計算結果のShaderResourceViewを取得する

		void Create_Buffer_Input(UINT size, UINT count, void* init_data);  //入力用のバッファを作成する
		void Create_Buffer_Result(UINT size, UINT count, void* init_data); //結果出力用のバッファを作成する

	private:
		static HRESULT Compile(const WCHAR* filename, LPCSTR method, LPCSTR shader_model, ID3DBlob** pp_blob_out);

		Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs = nullptr; // コンピュートシェーダ

		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_input; //計算入力用
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_result; //結果コピー用

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_input;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_result;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav_result;

		UINT contents_count = 0;
	};
}