#include "BehaviorTree.h"

Node* BehaviorTree::activeNodeInference(BehaviorData* data)
{
	// データをリセットして開始
	data->init();
	return root->childNodeInference(data);
}

void BehaviorTree::addNode(std::string parentName, std::string entryName, int priority, ExecJudgment* judgment, Action* action)
{
	// ルートノードが空でない場合
	if (parentName != "")
	{
		// ルートノードから繋がっているノードを検索
		Node* search_node = root->searchNode(parentName);
		if (search_node != NULL)
		{
			// 追加したいノード作成
			std::shared_ptr<Node> add_node = std::make_shared<Node>(entryName, search_node, priority, judgment, action, search_node->getHirerchyNo() + 1);
			// ノード登録
			search_node->addChild(add_node);
		}
	}
	else {
		// ルートノードが空の場合
		if (root == NULL)
		{
			// ルートノードにエントリーノードを登録
			root = std::make_unique<Node>(entryName, nullptr, priority, judgment, action, 1);
		}
	}
}
// シーケンスノードからの推論開始
Node* BehaviorTree::sequenceBack(Node* sequence_node, BehaviorData* data)
{
	return sequence_node->childNodeInference(data);
}
Node* BehaviorTree::run(Node* actionNode, BehaviorData* data)
{
	// ノード実行し、戻り値からノードの完了、継続、
	int state = actionNode->Run();

	// 正常終了
	if (state == ActionSTATE::END)
	{
		bool loop = false;
		do {
			//シーケンスノードを取り出す
			Node* sequenceNode = data->popSequenceNode();
			loop = false;

			// 途中じゃないなら終了
			// シーケンスノードがNULLであれば全て完了のため、return NULL
			if (sequenceNode == NULL)
			{
				return NULL;
			}
			else {
				// NULLでなければsequenceBack()を行い次のシーケンスノードを実行した結果をreturnしなさい
				Node* node = sequenceBack(sequenceNode, data);
				if (node != NULL)
				{
					// 実行した結果をreturnさせる
					return node;
				}
				// nodeがNULLの場合、スタックのサイズを調べて終了しているか判断
				// 0の場合、終了しているため、NULLをリターン
//				if (data->sequenceStack.size() == 0) {
//					return NULL;
//				}
				// ループ継続させるため、変数loopにtrueを代入
				loop = true;
			}
		} while (loop);
	}
	else if (state == ActionSTATE::FAILED)
	{
		return NULL;
	}
	// 現状維持
	return actionNode;
}