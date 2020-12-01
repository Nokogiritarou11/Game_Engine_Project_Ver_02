#pragma once
#include <unordered_map>
#include "ExecJudgment.h"

#include "Node.h"
#include "BehaviorData.h"

class Action;
class ExecJudgment;
class Node;
class BehaviorData;


class BehaviorTree
{
public:
	BehaviorTree() :root(nullptr)
	{
	}

	// 実行ノードを推論する
	Node* activeNodeInference(BehaviorData* data);
	
	Node* sequenceBack(Node* sequence_node, BehaviorData* data);


	void addNode(std::string parentName, std::string entryName, int priority, ExecJudgment* judgment, Action* action);
	

	Node* run(Node* actionNode, BehaviorData* data);

	std::unique_ptr<Node> root;


};



