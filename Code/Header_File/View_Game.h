#pragma once
#include "View_Texture.h"

class View_Game : public View_Texture
{
public:
	void Render(Matrix V, Matrix P, std::shared_ptr<Transform> camera_transform) override;
private:

};