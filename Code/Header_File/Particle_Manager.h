#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <memory>
#include <vector>
#include <locale.h>

namespace BeastEngine
{
	class Particle;
	class Transform;

	class Particle_Manager
	{
	public:
		Particle_Manager();
		~Particle_Manager();

		void Add(std::weak_ptr<BeastEngine::Particle> particle);
		void Camera_Update(std::shared_ptr<BeastEngine::Transform>& camera_trans, float FOV, float near_z, float far_z, float aspect);
		void Update();
		void Render();
		void Reset();

		Effekseer::ManagerRef manager = nullptr;
		std::unordered_map<std::string, Effekseer::EffectRef> effect_cache;

	private:
		std::vector<std::weak_ptr<BeastEngine::Particle>> Particle_List;
		EffekseerRendererDX11::RendererRef renderer = nullptr;

	};
}