#pragma once
#include "Shader.h"

namespace BeastEngine
{
	class Compute_Shader
	{
	public:
		static std::shared_ptr<Compute_Shader> Create(std::string shader_path);

		void Run();
		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Buffer> Get_Copy_Buffer() const;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& Get_SRV() { return srv_result; }

		void Create_Buffer_Input(UINT size, UINT count, void* init_data);
		void Create_Buffer_Result(UINT size, UINT count, void* init_data);

	private:
		static HRESULT Compile(const WCHAR* filename, LPCSTR method, LPCSTR shader_model, ID3DBlob** pp_blob_out);

		Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs = nullptr; // �R���s���[�g�V�F�[�_

		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_input; //�v�Z���͗p
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_result; //���ʃR�s�[�p

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_input;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_result;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav_result;

		UINT contents_count = 0;
	};
}