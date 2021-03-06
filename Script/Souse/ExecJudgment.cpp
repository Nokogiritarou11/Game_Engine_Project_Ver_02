#include "ExecJudgment.h"
#include "GameObject.h"
//TODO ������ 
bool AttackJudgment::judgment()
{
	{
		if (doragon->count_Stomp >= 5)
		{
			return true;
		}
		Vector2 v1 = { player.lock()->transform->Get_position().x - doragon->transform->Get_position().x,
								player.lock()->transform->Get_position().z - doragon->transform->Get_position().z };
		doragon->length = std::sqrtf(v1.x * v1.x + v1.y * v1.y);

		if (doragon->length > 500)
		{
			return false;
		}
		float angle = doragon->transform->Get_eulerAngles().y + 90.0f;

		float rad = DirectX::XMConvertToRadians(-angle);
		Vector2 arc_dir = { 1.0f,0.0 };
		Vector2 rotate_arc_dir = {
			((arc_dir.x * cosf(rad)) + (arc_dir.y * -sinf(rad))),
			((arc_dir.x * sinf(rad)) + (arc_dir.y * cosf(rad)))
		};
		Vector2 normal = {
			v1.x / doragon->length,
			v1.y / doragon->length
		};

		float dot = normal.x * rotate_arc_dir.x + normal.y * rotate_arc_dir.y;


		float rangeCos = cosf(DirectX::XMConvertToRadians(90.0f / 2.0f));

		if (rangeCos > dot)
		{
			return false;
		}

		return true;
	}

}

bool PhysicsJudgment::judgment()
{
	if (!doragon->is_Howling)
	{
		return false;
	}

	return true;
}

bool MagicJudgment::judgment()
{
	if (!doragon->is_Howling)
	{
		return false;
	}


	if (doragon->count_Stomp >= 5)
	{
		return false;
	}
	return true;
}

bool HowlingJudgment::judgment()
{

	if (!doragon->is_Howling)
	{
		return true;
	}
	

	return false;

}

bool MaulJudgment::judgment()
{
	if (doragon->length > 200.0f)
	{
		return false;
	}

	
	return true;

}

bool WalkMaulJudgment::judgment()
{
	if (doragon->length < 185.0f && doragon->length > 300.0f )
	{
		return false;
	}
	return true;
}

bool FireballJudgment::judgment()
{
	
	if (doragon->count_Bless > 6)
	{
		return false;
	}

	return true;

}

bool BlessJudgment::judgment()
{
	if (doragon->count_Bless < 6)
	{
		return false;
	}

	return true;

}

bool StompJudgment::judgment()
{
	if (doragon->count_Stomp >= 5)
	{
		doragon->count_Stomp = 0;
		return true;
	}
	if(doragon->length < 100)
	{
		return true;
		
	}
	return false;

}

bool WalkJudgment::judgment()
{
	doragon->count_Stomp++;
	
	return true;

}