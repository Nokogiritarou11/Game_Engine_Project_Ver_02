#include "Doragon.h"
#include "ExecJudgment.h"
#include "Actions.h"
#include "BehaviorTree.h"
#include "Dragon_HP.h"
using namespace std;

void Doragon::Awake()
{

}

void Doragon::Start()
{
	timer = 0.0f;
	anime = gameObject->GetComponent<Animator>();
	state = 0;
	is_Howling = false;
	count_Bless = 0;
	count_Stomp = 0;
	length = 0.0f;

	aiTree = std::make_shared<BehaviorTree>();
	aiData = std::make_shared<BehaviorData>();

	aiTree->addNode("", "root", 0, SELECT_RULE::PRIORITY, NULL, NULL);
	{	// rootノードの子
		// 上から順に条件の確認
		aiTree->addNode("root", "Attack", 1, SELECT_RULE::RANDOM, AttackJudgment::getInstance(this), NULL);
		{	// Attackノードの子
			aiTree->addNode("Attack", "HowlingAction", 0, SELECT_RULE::NONE,
				HowlingJudgment::getInstance(this), HowlingAction::getInstance(this));

			aiTree->addNode("Attack", "Physics", 1, SELECT_RULE::PRIORITY, PhysicsJudgment::getInstance(this), nullptr);
			{
				aiTree->addNode("Physics", "MaulAction", 2, SELECT_RULE::NONE, MaulJudgment::getInstance(this), MaulAction::getInstance(this));
				aiTree->addNode("Physics", "StompAction", 1, SELECT_RULE::NONE, StompJudgment::getInstance(this), StompAction::getInstance(this));
				//aiTree->addNode("Physics", "WalkMaulAction", 3, SELECT_RULE::NONE, WalkMaulJudgment::getInstance(this), WalkMaulAction::getInstance(this));
			}
			aiTree->addNode("Attack", "Magic", 1, SELECT_RULE::PRIORITY, MagicJudgment::getInstance(this), nullptr);
			{
				aiTree->addNode("Magic", "FireballAction", 1, SELECT_RULE::NONE, FireballJudgment::getInstance(this), FireballAction::getInstance(this));
				aiTree->addNode("Magic", "BlessAction", 2, SELECT_RULE::NONE, BlessJudgment::getInstance(this), BlessAction::getInstance(this));
			}
		}
		aiTree->addNode("root", "Walk", 2, SELECT_RULE::NONE, WalkJudgment::getInstance(this), WalkAction::getInstance(this));
	}
}

void Doragon::Update()
{
	HP = GetComponent<Dragon_HP>()->HP;
	
	if (timer >= 60)
	{
		if (activeNode == NULL && !this->anime.lock()->IsPlayAnimation())
		{
			anime.lock()->Stop();
			activeNode = aiTree->activeNodeInference(aiData.get());
		}

		if (activeNode != NULL)
		{
			activeNode = aiTree->run(activeNode, aiData.get());
		}
	}
	else
	{
		timer += 1;
	}


}

bool Doragon::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Doragon"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Doragon_sub"))
		{

			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
			
				Object::Destroy(dynamic_pointer_cast<Doragon>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}
	}
	return true;
}