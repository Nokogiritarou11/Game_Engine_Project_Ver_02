#pragma once
#include "SkinMesh_Renderer.h"

class Animator : public Behavior
{
public:
	Animator();
	~Animator();

	void Initialize(std::shared_ptr<GameObject> obj);

	bool IsPlayAnimation() const { return currentAnimation >= 0; }
	void Play(int animationIndex);
	void Stop();
	void Pause();
	void Update();

	bool Draw_ImGui();
private:

	void Set_Skin_Renderer(std::shared_ptr<SkinMesh_Renderer> render);
	void Set_Mesh(std::shared_ptr<Mesh> mesh);

	std::weak_ptr<SkinMesh_Renderer> skin_renderer;
	std::shared_ptr<Mesh> mesh_data;

	float       animation_speed  = 1;
	int			currentAnimation = -1;
	float		currentSeconds   = 0.0f;
	bool		loopAnimation    = false;
	bool		endAnimation     = false;
	bool        Playing          = false;
};