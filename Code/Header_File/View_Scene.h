#pragma once
#include "View_Texture.h"

class View_Scene : public View_Texture
{
public:
	void Render(Matrix V, Matrix P, std::shared_ptr<Transform> trans) override;
private:

};