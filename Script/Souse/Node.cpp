#include "Node.h"


bool Node::judgment()
{
	//　判定クラスを持っていれば判定クラスのjudgmentを実行した結果をreturnせよ
	// 判定クラスが無ければ実行するため、trueをreturnせよ
	if (execJudgment != nullptr)
	{
		return execJudgment->judgment();

	}
	// 判定クラスが無ければ実行扱い
	return true;
}

Node* Node::selectPriority(std::vector<Node*>* list)
{
	Node* selectNode = NULL;
	int maxPriority = INT_MAX;

	// 一番優先順位が高いノードを探す
	for (auto& node : *list)
	{
		if (node->priority < maxPriority)
		{
			selectNode = node;
			maxPriority = node->priority;
		}
	}

	return selectNode;
}

Node* Node::searchNode(std::string searchName)
{
	// 名前が一致
	if (name == searchName)
	{
		return this;
	}
	else {
		// 子ノードで検索
		for (auto itr = child.begin(); itr != child.end(); itr++)
		{
			Node* ret = (*itr)->searchNode(searchName);

			if (ret != nullptr)
			{
				return ret;
			}
		}
	}

	return NULL;
}

Node* Node::childNodeInference(BehaviorData* data)
{
	std::vector<Node*> list;
	Node* result = NULL;

	// 子ノードで実行可能なノードを探す
	for (int i = 0; i < (int)child.size(); i++)
	{
		if (child[i]->execJudgment != NULL)
		{

			if (child[i]->execJudgment->judgment() == true)
			{
				list.push_back(child[i].get());
			}
		}
		else {
			// 判定クラスがなければ無条件に追加
			list.push_back(child[i].get());
		}
	}


	// 優先順位
	result = selectPriority(&list);



	if (result != NULL)
	{
		// 実行クラスがあれば終了
		if (result->hasAction() == true)
		{
			return result;
		}
		else {
			// 行動データが無い。
			// つまり中間ノードなので次のノードを調べる
			result = result->childNodeInference(data);
		}
	}

	return result;
}

int Node::Run()
{
	// アクションノードがあればrunを実行した結果を返す、なければ失敗扱いとしてFAILEDを返す
	if (action != NULL)
	{
		return action->run();
	}

	return ActionSTATE::FAILED;
}