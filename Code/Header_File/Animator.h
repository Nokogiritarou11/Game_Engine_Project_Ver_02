#pragma once
#include "SkinMesh_Renderer.h"

class Animator : public Behavior
{
public:
	Animator();
	~Animator();

	int   Animation_Index = 0;
	float Animation_Time  = 0;
	float Animation_Rate  = 0;
	bool  Animation_Loop  = false;
	bool  Animation_End   = false;
	float Animation_Speed = 1.0f;

	void Initialize(std::shared_ptr<GameObject> obj);
	void Set_Skin_Renderer(std::shared_ptr<SkinMesh_Renderer> render);
	void Update();
private:

	std::weak_ptr<SkinMesh_Renderer> skin_renderer;

	int  Animation_Index_Old = 0;
};