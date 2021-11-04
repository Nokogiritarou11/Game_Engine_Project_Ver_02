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

	//マテリアル
	class Material : public Object
	{
	public:
		//レンダリングの種類
		enum class Rendering_Mode
		{
			Opaque,
			Cutout,
			Transparent
		};

		int render_queue = 2000; //レンダーキュー 値が低いほど先に描画される

		static std::shared_ptr<Material> Create(const std::string& vertex_path, const std::string& pixel_path = "", const std::string& geometry_path = ""); //新規作成
		static std::shared_ptr<Material> Load_Material(const std::string& full_path); //ファイルパスから読み込む

		void Save(const std::string& path = ""); //ファイルパスにファイル化する 引数を省略した場合上書きセーブ

		void Set_Rendering_Mode(Rendering_Mode mode); //レンダリングモードを設定する
		void Set_Shader(const std::string& path, Shader::Shader_Type shader_type, int pass = 0); //シェーダーを読み込みセットする
		void Set_Blend_State(BS_State state, int pass = 0);         //ブレンドステートを設定する
		void Set_Rasterizer_State(RS_State state, int pass = 0);    //ラスタライザーステートを設定する
		void Set_Depth_Stencil_State(DS_State state, int pass = 0); //デプスステンシルステートを設定する

		void Set_Texture(const std::string& texture_name, const std::shared_ptr<Texture>& texture); //テクスチャを設定する
		void Set_Int(const std::string& int_name, const int& value);            //コンスタントバッファのパラメータを変数名から設定する
		void Set_Float(const std::string& float_name, const float& value);      //コンスタントバッファのパラメータを変数名から設定する
		void Set_Vector2(const std::string& vector_name, const Vector2& value); //コンスタントバッファのパラメータを変数名から設定する
		void Set_Vector3(const std::string& vector_name, const Vector3& value); //コンスタントバッファのパラメータを変数名から設定する
		void Set_Vector4(const std::string& vector_name, const Vector4& value); //コンスタントバッファのパラメータを変数名から設定する
		void Set_Matrix(const std::string& matrix_name, const Matrix& value);   //コンスタントバッファのパラメータを変数名から設定する

		void Add_Pass(const std::string& vertex_path, const std::string& pixel_path = "", const std::string& geometry_path = ""); //レンダリングパスを追加する
		void Remove_Pass(); //レンダリングパスを削除する

		std::shared_ptr<Texture> Get_Texture(const std::string& texture_name); //テクスチャを変数名から取得する
		int Get_Int(const std::string& int_name);                              //コンスタントバッファのパラメータを変数名から取得する
		float Get_Float(const std::string& float_name);                        //コンスタントバッファのパラメータを変数名から取得する
		Vector2 Get_Vector2(const std::string& vector_name);                   //コンスタントバッファのパラメータを変数名から取得する
		Vector3 Get_Vector3(const std::string& vector_name);                   //コンスタントバッファのパラメータを変数名から取得する
		Vector4 Get_Vector4(const std::string& vector_name);                   //コンスタントバッファのパラメータを変数名から取得する
		Matrix Get_Matrix(const std::string& matrix_name);                     //コンスタントバッファのパラメータを変数名から取得する

	private:
		//シェーダー情報
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

		//コンスタントバッファ
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

		//コンスタントバッファ内のパラメータ
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

		//テクスチャ情報
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

		//レンダリングパス
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

			void Reflect_Shader();                          //コンスタントバッファ情報をリフレクションする
			void Reflect_Shader(Shader::Shader_Type type);  //コンスタントバッファ情報をリフレクションする
			void Reflect_Texture();                         //シェーダー内テクスチャ情報をリフレクションする
			void Reflect_Texture(Shader::Shader_Type type); //シェーダー内テクスチャ情報をリフレクションする

			static void Create_ConstantBuffer(ConstantBuffer_Info& info, const UINT& size);
			void Initialize_Texture();   //テクスチャ情報の初期化
			void Initialize_Shader();    //シェーダー情報の初期化
			void Activate();             //レンダリングパスをアクティブにする
			void Activate_Buffer();      //コンスタントバッファをセット
			void Activate_Texture();     //テクスチャをセット
			void Activate_Shader();      //シェーダーをステージする
			void Activate_State() const; //各ステートをセットする

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

		void Set_Parameter(const std::string& parameter_name, const void* value, const Shader::Parameter_Type& type); //コンスタントバッファのパラメータを変数名から設定する
		void Activate(int pass = 0);         //指定したレンダリングパスをアクティブに
		void Activate_Buffer(int pass = 0);  //指定したレンダリングパスのコンスタントバッファをセットする
		void Activate_Texture(int pass = 0); //指定したレンダリングパスのテクスチャをセットする
		void Activate_Shader(int pass = 0);  //指定したレンダリングパスのシェーダーをステージする
		void Activate_State(int pass = 0);   //指定したレンダリングパスの各ステートをセットする
		void Draw_ImGui(); //ImGui描画

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