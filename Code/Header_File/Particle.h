#pragma once
#include "Component.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "DxSystem.h"
#include "Original_Math.h"
#include <memory>
#include <vector>
#include <locale.h>

class Particle : public Component
{
public:
	~Particle();

	void Set_Particle(const char* filepath, const char* filename);

	void Play();
	void Pause();
	void Stop();

	bool Play_On_Awake = true;
	float Play_Speed = 1.0f;

private:
	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;
	void SetActive(bool value) override;

	bool IsCalled = false;
	bool Disable_flg = false;

	Effekseer::Effect* effect = nullptr;
	Effekseer::Handle handle = -1;

	std::string file_name;
	std::string file_path;

	friend class Particle_Manager;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this), Play_On_Awake, Play_Speed, file_name, file_path);
	}
};

CEREAL_REGISTER_TYPE(Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Particle)