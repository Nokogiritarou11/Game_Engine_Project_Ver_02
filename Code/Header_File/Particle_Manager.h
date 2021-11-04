#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Particle;
	class Transform;

	//Effekseerの歪み描画用コールバック
	class Distorting_Callback final : public EffekseerRenderer::DistortingCallback
	{
	public:
		bool OnDistorting(EffekseerRenderer::Renderer* renderer) override; //歪みエフェクトが描画される際に呼ばれる
	};

	//パーティクルマネージャー(Effekseerライブラリのラッパー)
	class Particle_Manager
	{
	public:
		Particle_Manager();
		~Particle_Manager();

		void Add(std::weak_ptr<Particle> particle); //パーティクルコンポーネントを登録する
		void Camera_Update(const std::shared_ptr<Transform>& camera_trans, float fov, float near_z, float far_z, float aspect) const; //Effekseer内のカメラを更新する
		void Update(); //更新
		void Render() const; //描画実行
		void Reset(); //クリア

		EffekseerRendererDX11::RendererRef renderer = nullptr;
		Effekseer::ManagerRef manager = nullptr;
		std::unordered_map<std::string, Effekseer::EffectRef> effect_cache;
		Distorting_Callback* distorting_callback;

	private:
		std::vector<std::weak_ptr<Particle>> particle_list;
		Effekseer::Matrix44 reverse;
	};

}