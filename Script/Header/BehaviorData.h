#pragma once
#include "BehaviorTree.h"
#include <stack>
#include <map>
#include "Node.h"

class Node;

class BehaviorData
{
public:
	// �R���X�g���N�^
	BehaviorData()
	{
		init();
	}

	// �V�[�P���X�m�[�h�̃v�b�V��
	void pushSequenceNode(Node* node)
	{
		sequenceStack.push(node);
	}

	// �V�[�P���X�m�[�h�̃|�b�v
	Node* popSequenceNode();
	
	// �m�[�h�g�p����
	bool isNodeUsed(std::string name);

	// �g�p�ς݃m�[�h�ɓo�^
	void entryUsedNode(std::string name)
	{
		usedNodeMap[name] = true;
	}

	// �V�[�P���X�X�e�b�v�̃Q�b�^�[
	int getSequenceStep(std::string name);
	
	// �V�[�P���X�X�e�b�v�̃Z�b�^�[
	void setSequenceStep(std::string name, int step)
	{
		runSequenceStepMap[name] = step;
	}

	// ������
	void init();

	// �g�p�ς݃m�[�h�̃��Z�b�g
	void resetNodeUsed(std::vector<std::shared_ptr<Node>>* resetHierachy);
private:
	std::stack<Node*> sequenceStack;					// �V�[�P���X�m�[�h�X�^�b�N
	std::map<std::string, int> runSequenceStepMap;			// ���s�V�[�P���X�̃X�e�b�v�}�b�v
	std::map<std::string, bool> usedNodeMap;				// �m�[�h�̎g�p�ςݔ���}�b�v
};