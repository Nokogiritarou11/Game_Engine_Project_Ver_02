#pragma once
#include "Audio.h"

namespace BeastEngine
{
	//�I�[�f�B�I�Ǘ��}�l�[�W���[
	class Audio_Manager
	{
	public:
		Audio_Manager();

		void Update() const;  //�X�V
		void Suspend() const; //�ꎞ��~����
		void Resume() const;  //�ĊJ����
		void Reset() const;   //���Z�b�g����

		std::unique_ptr<DirectX::SoundEffectInstance> Load_SoundEffect(std::string filename); //�t�@�C���p�X����I�[�f�B�I�N���b�v��ǂݍ���
		void Play_OneShot(std::string filename, float volume = 1.0f, float pitch = 0.0f); //�d���\�ȉ������Đ�����

	private:
		std::unique_ptr<DirectX::AudioEngine> engine;
		std::unordered_map<std::wstring, std::unique_ptr<DirectX::SoundEffect>> effect_map;
	};
}