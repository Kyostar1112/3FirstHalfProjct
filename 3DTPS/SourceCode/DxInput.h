//========================================================================================
//
//	��[dinput.h]��Ǎ��ލۂɂ��Ȃ���΂����Ȃ����ƁB
//
//		[�c�[��]��[�I�v�V����]��[�v���W�F�N�g����у\�����[�V����]��
//			[�S��]��[VC++ �f�B���N�g��]��[�f�B���N�g����\������v���W�F�N�g]��
//				[�C���N���[�h �t�@�C��]
//					C:\Program Files\Microsoft DirectX SDK (June 2010)\Include
//				[���C�u���� �t�@�C��]
//					C:\Program Files\Microsoft DirectX SDK (June 2010)\Lib\x86
//
//		[�v���W�F�N�g]��[�����̃v���p�e�B]��[�\���v���p�e�B]��
//			[�����J]��[����]��[�ǉ��̈ˑ��t�@�C��]
//					dxguid.lib dinput8.lib			��DirectInput �Ŏg�p����K�v�Œ���B
//
//		�ǉ��ˑ��t�@�C���́A���L�� #pragma comment( lib, "����.lib") �ő�p�\�B
//		���̃v���W�F�N�g�ɃR�[�h���ڐA���邱�Ƃ��l����Ƃ�����̕����y�ł悢�B
//
//========================================================================================

#ifndef _DX_INPUT_H_
#define _DX_INPUT_H_

#include <dinput.h>	// DirectInput�p.

// DirectX �֌W.
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "dinput8.lib" )	// �R���g���[������Ŏg�p.

//�񋓑̐錾.
//�L�[���.
enum enPKey {
	enPKey_Up,
	enPKey_Down,
	enPKey_Left,
	enPKey_Right,
	enPKey_00,
	enPKey_01,
	enPKey_02,
	enPKey_03,
	enPKey_04,
	enPKey_05,
	enPKey_06,
	enPKey_07,
	enPKey_08,
	enPKey_09,
	enPKey_10,
	enPKey_11,

	enPKey_Max,	//�L�[�ő吔.
};

class clsDxInput
{
public:
	clsDxInput();
	~clsDxInput();

	//DirectInput�����ݒ�.
	bool initDI(HWND hWnd);

	//���͏��X�V�֐�.
	HRESULT UpdateInputState();

	//���͏���ǉ�����֐�.
	void AddInputState(enPKey enKey);

	//���͏�������������֐�.
	void InitInputState();

	//���̓`�F�b�N����֐�.
	bool IsPressKey(enPKey enKey);

private:
	LPDIRECTINPUT8			m_pDI;//DxInput�I�u�W�F�N�g.
	LPDIRECTINPUTDEVICE8	m_pPad;//�f�o�C�X(�R���g���[��)�I�u�W�F�N�g.

	unsigned int			m_uInputState;//���͏��.
};

#endif//#ifndef _DX_INPUT_H_
