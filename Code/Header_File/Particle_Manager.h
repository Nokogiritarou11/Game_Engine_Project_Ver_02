#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "DxSystem.h"
#include "Original_Math.h"
#include <memory>
#include <vector>
#include <locale.h>

class Transform;
class Particle;

class Particle_Manager
{
public:
	Particle_Manager();
	~Particle_Manager();

	void Add(std::weak_ptr<Particle> particle);
	void Update(std::shared_ptr<Transform>& camera_trans, float FOV, float near_z, float far_z, float aspect);
	void Render();
	void Reset();

	Effekseer::Manager* manager = nullptr;
	std::unordered_map<std::string, Effekseer::Effect*> effect_cache;

private:
	std::vector<std::weak_ptr<Particle>> Particle_List;
	EffekseerRendererDX11::Renderer* renderer = nullptr;

};