#include "Sound.h"
#include <stdio.h>

//�R���X�g���N�^.
clsSound::clsSound()
{
	ZeroMemory(this, sizeof(clsSound));
	m_bSeekFlg = true;
}

//�f�X�g���N�^.
clsSound::~clsSound()
{
}

//�����t�@�C�����J��.
bool clsSound::Open(LPCTSTR sFName, char *sAlias, HWND hWnd)
{
	//�����ݒ�.
	SetInitParam(sAlias, hWnd);

	//�R�}���h.
	char command[STR_BUFF_MAX] = "open ";

	strcat_s(command, sizeof(command), sFName);//�t�@�C����������.
	strcat_s(command, sizeof(command), " alias ");
	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.

	if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//�����t�@�C�������.
bool clsSound::Close()
{
	//�R�}���h.
	char command[STR_BUFF_MAX] = "close ";

	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.

	if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//�Đ��֐�.
bool clsSound::Play(bool bNotify)
{
	//�R�}���h.
	char command[STR_BUFF_MAX] = "play ";

	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.
	if (bNotify) {
		strcat_s(command, sizeof(command), " notify");
		m_bSeekFlg = false;
		//notify:�������I�������Ƃ���hWnd��MM_MCINOTIFY���b�Z�[�W���|�X�g����.
	}
	if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//��~�֐�.
bool clsSound::Stop()
{
	//�R�}���h.
	char command[STR_BUFF_MAX] = "stop ";

	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.

	if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//��Ԃ��擾����֐�.
//	sStatus �̔z�񐔂� 256�ȉ��ɂ��邱��.
//���������A��Ԃ��擾����ꍇ�́A�Đ����Ɂunotify�v��ݒ肵�A
//  �E�B���h�E�n���h���Ƀ��b�Z�[�W�𑗂��Ă����K�v������.
bool clsSound::GetStatus(char *sStatus)
{
	//�R�}���h.
	char command[STR_BUFF_MAX] = "status ";

	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.
	strcat_s(command, sizeof(command), " mode");

	if (mciSendString(command, sStatus, STR_BUFF_MAX, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//���̒�~���m�F����֐�.
bool clsSound::IsStopped()
{
	char sStatus[STR_BUFF_MAX] = "";

	//��Ԃ̎擾.
	if (GetStatus(sStatus)) {
		//�������r.
		if (strcmp(sStatus, "stopped") == 0) {
			return true;
		}
	}
	return false;
}

//���̍Đ������m�F����֐�.
bool clsSound::IsPlaying()
{
	char sStatus[STR_BUFF_MAX] = "";

	//��Ԃ̎擾.
	if (GetStatus(sStatus)) {
		//�������r.
		if (strcmp(sStatus, "playing") == 0) {
			return true;
		}
	}
	return false;
}

//�Đ��ʒu�����߂ɖ߂��֐�.
bool clsSound::SeekToStart()
{
	if (!m_bSeekFlg)
	{
		//�R�}���h.
		char command[STR_BUFF_MAX] = "seek ";

		strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.
		strcat_s(command, sizeof(command), " to start");

		if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
			m_bSeekFlg = true;
			return true;
		}
		m_bSeekFlg = true;
	}
	return false;
}

//���ʂ�ݒ肷��֐�.
bool clsSound::SetVolume(int iVolume)
{
	//�͈͓��Ɋۂ߂�.
	if (iVolume < 0) { iVolume = 0; }
	if (iVolume > 1000) { iVolume = 1000; }

	//���ʂ𕶎���ɕϊ�.
	char sVolume[STR_BUFF_MAX] = "";
	sprintf_s(sVolume, sizeof(sVolume), "%d", iVolume);

	//�R�}���h.
	char command[STR_BUFF_MAX] = "setaudio ";

	strcat_s(command, sizeof(command), m_sAlias);//�G�C���A�X������.
	strcat_s(command, sizeof(command), " volume to ");
	strcat_s(command, sizeof(command), sVolume);//���ʂ�����.

	if (mciSendString(command, NULL, 0, m_hWnd) == 0) {
		return true;
	}
	return false;
}

//�����ݒ�.
void clsSound::SetInitParam(char *sAlias, HWND hWnd)
{
	//�E�B���h�E�n���h����n��.
	m_hWnd = hWnd;

	//�G�C���A�X��n��.
	strcpy_s(m_sAlias, sizeof(m_sAlias), sAlias);
}