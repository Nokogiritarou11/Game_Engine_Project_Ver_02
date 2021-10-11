#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Particle;
	class Transform;

	class Particle_Manager
	{
	public:
		Particle_Manager();
		~Particle_Manager();

		void Add(std::weak_ptr<Particle> particle);
		void Camera_Update(const std::shared_ptr<Transform>& camera_trans, float fov, float near_z, float far_z, float aspect) const;
		void Update();
		void Render() const;
		void Reset();

		Effekseer::ManagerRef manager = nullptr;
		std::unordered_map<std::string, Effekseer::EffectRef> effect_cache;

	private:
		std::vector<std::weak_ptr<Particle>> particle_list;
		EffekseerRendererDX11::RendererRef renderer = nullptr;
		Effekseer::Matrix44 reverse;
	};
}