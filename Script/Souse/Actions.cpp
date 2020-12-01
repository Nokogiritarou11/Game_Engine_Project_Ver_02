#include "Actions.h"
#include "GameObject.h"
// TODO ˆÚ“®ˆ—
bool Action::is_anime = false;
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
	if (!is_anime)
	{
		is_anime = true;
		doragon->anime.lock()->Play(2);
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
		doragon->is_Bless = true;
		doragon->anime.lock()->Play(4);
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
	static bool is_anime = false;
	if (!is_anime)
	{
		is_anime = true;
		doragon->anime.lock()->Play(1);
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
