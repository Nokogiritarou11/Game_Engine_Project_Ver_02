#include "BehaviorData.h"

void BehaviorData::init()
{
	runSequenceStepMap.clear();
	while (sequenceStack.size() > 0)
	{
		sequenceStack.pop();
	}
}

void BehaviorData::resetNodeUsed(std::vector<std::shared_ptr<Node>>* resetHierachy)
{
	for (auto itr = resetHierachy->begin(); itr != resetHierachy->end(); itr++)
	{
		usedNodeMap[(*itr)->getName()] = false;
	}
}

Node* BehaviorData::popSequenceNode()
{
	// ��Ȃ�NULL
	if (sequenceStack.empty() != 0)
	{
		return NULL;
	}

	Node* node = sequenceStack.top();

	if (node != NULL)
	{
		// ���o�����f�[�^���폜
		sequenceStack.pop();
	}

	return node;
}

bool BehaviorData::isNodeUsed(std::string name)
{
	// �g�p���ĂȂ�������false
	if (usedNodeMap.count(name) == 0)
	{
		return false;
	}

	return usedNodeMap[name];
}

int BehaviorData::getSequenceStep(std::string name)
{
	if (runSequenceStepMap.count(name) == 0)
	{
		runSequenceStepMap[name] = 0;
	}

	return runSequenceStepMap[name];
}