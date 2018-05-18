#ifndef _C_COMMON_H_
#define _C_COMMON_H_

//�C���N���[�h.
#include "Global.h"

//���_�̍\����.
struct SpriteVertex
{
	D3DXVECTOR3 vPos;	//�ʒu.
	D3DXVECTOR2 vTex;	//�e�N�X�`�����W.
};

//�������\����.
struct WHSIZE_FLOAT
{
	float w;
	float h;
};

//���ʃN���X(�e�N���X).
class clsCommon
{
public:
	clsCommon() {};
	virtual ~clsCommon() {};

	//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
	void SetBlend(bool flg);

	//���A�v���Ɉ��.
	ID3D11Device*			m_pDevice11;		//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*	m_pDeviceContext11;	//�f�o�C�X�R���e�L�X�g.

	ID3D11BlendState*		m_pBlendState;	//�u�����h�X�e�[�g.

private:
};

#endif//#define _C_COMMON_H_
