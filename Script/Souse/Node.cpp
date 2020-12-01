#include "Node.h"


bool Node::judgment()
{
	//�@����N���X�������Ă���Δ���N���X��judgment�����s�������ʂ�return����
	// ����N���X��������Ύ��s���邽�߁Atrue��return����
	if (execJudgment != nullptr)
	{
		return execJudgment->judgment();

	}
	// ����N���X��������Ύ��s����
	return true;
}

Node* Node::selectPriority(std::vector<Node*>* list)
{
	Node* selectNode = NULL;
	int maxPriority = INT_MAX;

	// ��ԗD�揇�ʂ������m�[�h��T��
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
	// ���O����v
	if (name == searchName)
	{
		return this;
	}
	else {
		// �q�m�[�h�Ō���
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

	// �q�m�[�h�Ŏ��s�\�ȃm�[�h��T��
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
			// ����N���X���Ȃ���Ζ������ɒǉ�
			list.push_back(child[i].get());
		}
	}


	// �D�揇��
	result = selectPriority(&list);



	if (result != NULL)
	{
		// ���s�N���X������ΏI��
		if (result->hasAction() == true)
		{
			return result;
		}
		else {
			// �s���f�[�^�������B
			// �܂蒆�ԃm�[�h�Ȃ̂Ŏ��̃m�[�h�𒲂ׂ�
			result = result->childNodeInference(data);
		}
	}

	return result;
}

int Node::Run()
{
	// �A�N�V�����m�[�h�������run�����s�������ʂ�Ԃ��A�Ȃ���Ύ��s�����Ƃ���FAILED��Ԃ�
	if (action != NULL)
	{
		return action->run();
	}

	return ActionSTATE::FAILED;
}