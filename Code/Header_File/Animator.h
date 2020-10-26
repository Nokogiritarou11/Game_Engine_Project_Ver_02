#pragma once
#include "SkinMesh_Renderer.h"

class Animator : public Behaviour
{
public:

	bool IsPlayAnimation() const { return currentAnimation >= 0; }
	void Play(int animationIndex);
	void Stop();
	void Pause();
	void Update();

	float       animation_speed = 1;
	bool		loopAnimation = false;

private:
	void Initialize(std::shared_ptr<GameObject> obj);
	bool Draw_ImGui();

	void Set_Skin_Renderer(std::shared_ptr<SkinMesh_Renderer> render);
	void Set_Mesh(std::shared_ptr<Mesh> mesh);

	std::weak_ptr<SkinMesh_Renderer> skin_renderer;
	std::shared_ptr<Mesh> mesh_data;

	int			currentAnimation = -1;
	float		currentSeconds   = 0.0f;
	bool		endAnimation     = false;
	bool        Playing          = false;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, Animator)