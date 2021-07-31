#pragma once
#include "View_Texture.h"

namespace BeastEngine
{
	class Transform;

	class View_Scene : public View_Texture
	{
	public:
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P, std::shared_ptr<BeastEngine::Transform>& camera_transform) override;
	private:

	};
}