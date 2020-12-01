#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Actions.h"
#include "ExecJudgment.h"

class ExecJudgment;
class BehaviorData;
class Action;

class Node
{
protected:
	std::string name; // 名前
	const int priority; // 中間層での優先順位
	Action* action; // 行動
	Node* parent;  // 親ノード　
	std::vector<std::shared_ptr<Node>> child;// 子ノード
	int hierarchyNo; // 階層番号

public:

	Node(std::string name,
		Node* parent,
		int priority,
		ExecJudgment* execJudgment,
		Action* action, int hierarchyNo) :
		name(name),
		parent(parent),
		priority(priority),
		execJudgment(execJudgment),
		action(action),
		hierarchyNo(hierarchyNo),
		child(NULL)
	{
	}

	ExecJudgment* execJudgment;

	// 名前ゲッター
	std::string getName() { return name; }

	// 親ノードゲッター
	Node* getParent() { return parent; }
	
	// 子ノードゲッター
	Node* getChild(int index) { return child[index].get(); }

	// 子ノードゲッター(末尾)
	Node* getLastChild() { return child[child.size() - 1].get(); }

	// 子ノードゲッター(先頭)
	Node* getTopChild() { return child[0].get(); }

	// 階層番号ゲッター
	int getHirerchyNo() { return hierarchyNo; }

	// 優先順位ゲッター
	int getPriority() { return priority; }

	// 親ノードセッター
	void setParent(Node* newParent) { parent = newParent; }

	// 子ノード追加
	void addChild(std::shared_ptr<Node> newChild) { child.push_back(newChild); }

	// 行動データを持っているか
	bool hasAction() { if (action) { return true; } return false; }

	// 実行可否判定
	bool judgment();

	// 優先順位選択
	Node* selectPriority(std::vector<Node*>* list);

	// ノード検索
	Node* searchNode(std::string searchName);

	// ノード推論
	Node* childNodeInference(BehaviorData* data);
	 
	int Run();

};