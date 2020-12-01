#pragma once
#include "BehaviorTree.h"
#include <stack>
#include <map>
#include "Node.h"

class Node;

class BehaviorData
{
public:
	// コンストラクタ
	BehaviorData()
	{
		init();
	}

	// シーケンスノードのプッシュ
	void pushSequenceNode(Node* node)
	{
		sequenceStack.push(node);
	}

	// シーケンスノードのポップ
	Node* popSequenceNode();
	
	// ノード使用判定
	bool isNodeUsed(std::string name);

	// 使用済みノードに登録
	void entryUsedNode(std::string name)
	{
		usedNodeMap[name] = true;
	}

	// シーケンスステップのゲッター
	int getSequenceStep(std::string name);
	
	// シーケンスステップのセッター
	void setSequenceStep(std::string name, int step)
	{
		runSequenceStepMap[name] = step;
	}

	// 初期化
	void init();

	// 使用済みノードのリセット
	void resetNodeUsed(std::vector<std::shared_ptr<Node>>* resetHierachy);
private:
	std::stack<Node*> sequenceStack;					// シーケンスノードスタック
	std::map<std::string, int> runSequenceStepMap;			// 実行シーケンスのステップマップ
	std::map<std::string, bool> usedNodeMap;				// ノードの使用済み判定マップ
};