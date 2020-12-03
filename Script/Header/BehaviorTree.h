#pragma once
#include <unordered_map>

#include "ExecJudgment.h"
#include "Node.h"
#include "BehaviorData.h"

class Action;
class ExecJudgment;
class Node;
class BehaviorData;
enum SELECT_RULE;


class BehaviorTree
{
public:


	BehaviorTree() :root(nullptr)
	{
	}

	// 実行ノードを推論する
	Node* activeNodeInference(BehaviorData* data);
	
	Node* sequenceBack(Node* sequence_node, BehaviorData* data);


	void addNode(std::string parentName, std::string entryName, int priority,SELECT_RULE _select_Rule, ExecJudgment* judgment, Action* action);
	

	Node* run(Node* actionNode, BehaviorData* data);

	std::unique_ptr<Node> root;


};



