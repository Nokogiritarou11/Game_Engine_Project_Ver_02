#pragma once
#include "View_Texture.h"

class View_Game : public View_Texture
{
public:
	void Render(Matrix V, Matrix P, Vector3 pos) override;
private:

};