#include "Actions.h"
#include "GameObject.h"
// TODO ˆÚ“®ˆ—
bool Action::is_anime = false;

bool judg(Doragon* doragon,GameObject* player)
{
	Vector2 v1 = { player->transform->Get_position().x - doragon->transform->Get_position().x,
									player->transform->Get_position().z - doragon->transform->Get_position().z };
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


	float rangeCos = cosf(DirectX::XMConvertToRadians(15.0f / 2.0f));

	if (rangeCos > dot)
	{
		return false;
	}

	return true;
}

bool LRjudg(Doragon* doragon, GameObject* player)
{
	Vector2 v1 = { player->transform->Get_position().x - doragon->transform->Get_position().x,
									player->transform->Get_position().z - doragon->transform->Get_position().z };
	doragon->length = std::sqrtf(v1.x * v1.x + v1.y * v1.y);

	float angle = doragon->transform->Get_eulerAngles().y + 182.5f;

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


	float rangeCos = cosf(DirectX::XMConvertToRadians(175.0f / 2.0f));

	if (rangeCos > dot)
	{
		float angle = doragon->transform->Get_eulerAngles().y + 7.5f;

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


		float rangeCos = cosf(DirectX::XMConvertToRadians(175.0f / 2.0f));
		return false;
	}

	return true;
}

ActionSTATE HowlingAction::run()
{

	doragon->is_Howling = true;

	
	if (!is_anime)
	{
		is_anime = true;
		doragon->anime.lock()->Play(0);
	}


	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		is_anime = false;
		return ActionSTATE::FAILED;

	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

}


ActionSTATE MaulAction::run()
{
	static Vector3 v;
	if (!is_anime)
	{
		is_anime = true;
		v = doragon->transform->Get_forward() * -3;
		doragon->anime.lock()->Play(2);
		doragon->count_Stomp++;
	}
	
	Vector3 v1 = doragon->transform->Get_position();
	v1 += v * Time::deltaTime;
	doragon->transform->Set_position(v1);


	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		is_anime = false;
		
		return ActionSTATE::END;

	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

	return ActionSTATE::END;

}

ActionSTATE FireballAction::run()
{

	if (!is_anime)
	{
		is_anime = true;
		doragon->anime.lock()->Play(3);
	}


	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		is_anime = false;
		Vector3 v = doragon->transform->Get_forward();
		v *= 50;
		Vector3 v1 = doragon->transform->Get_position();
		v1 += v * Time::deltaTime;
		doragon->transform->Set_position(v1);
		doragon->count_Bless++;
		return ActionSTATE::END;
	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

	return ActionSTATE::END;

}

ActionSTATE BlessAction::run()
{

	if (!is_anime)
	{
		is_anime = true;
		doragon->count_Bless = 0;
		doragon->anime.lock()->Play(4);
	}	


	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		is_anime = false;
		Vector3 v = doragon->transform->Get_forward() * 100;
		Vector3 v1 = doragon->transform->Get_position();
		v1 += v * Time::deltaTime;
		doragon->transform->Set_position(v1);


		return ActionSTATE::END;
	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

	return ActionSTATE::END;

}

ActionSTATE StompAction::run()
{

	if (!is_anime)
	{
		is_anime = true;
		doragon->anime.lock()->Play(5);
	}
	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		is_anime = false;

		return ActionSTATE::END;
	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

	return ActionSTATE::END;

}


ActionSTATE WalkAction::run()
{
	float step = 1 * Time::deltaTime;

	static float angle;
	if (!doragon->anime.lock()->IsPlayAnimation())
	{
		if (LRjudg(doragon, player.lock().get()))
		{
			doragon->anime.lock()->Play(7);
			doragon->anime.lock()->animation_speed = 2.0f;
			angle = doragon->transform->Get_eulerAngles().y + 135.5f;
		}
		else
		{
			doragon->anime.lock()->Play(6);
			doragon->anime.lock()->animation_speed = 2.0f;
			angle = doragon->transform->Get_eulerAngles().y - 135.5;
		}
		if (angle < -180.0f)
		{
			angle += 360.0f;
		}
		else if (angle > 180.0f)
		{
			angle -= 360.0f;
		}
	}
	if (count > 60.0)
	{
		Vector3 q = { 0,angle,0 };
		doragon->transform->Set_rotation(Quaternion::Slerp(doragon->transform->Get_rotation(),
			Quaternion::Euler(q), step));
	}
	count++;
	if (judg(doragon ,player.lock().get()))
	{
		count = 0;
		doragon->anime.lock()->animation_speed = 1.0f;
		return ActionSTATE::END;
	}
	else
	{
		return ActionSTATE::SUCCESS;
	}

		return ActionSTATE::SUCCESS;

}
