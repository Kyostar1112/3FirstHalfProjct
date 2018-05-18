#ifndef _SPRITE_H_
#define _SPRITE_H_

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

#include "Common.h"//���ʃN���X.

#include "Resource.h"

//======================================
//	�\����.
//======================================
//�R���X�^���g�o�b�t�@�̃A�v�����̒�`(Simple.hlsl)
//���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
struct SPRITESHADER_CONSTANT_BUFFER
{
	D3DXMATRIX	mWVP;		//���[���h,�r���[,�ˉe�̍����ϊ��s��.
	D3DXVECTOR4	vColor;		//�J���[(RGBA�̌^�ɍ��킹��)
	D3DXVECTOR4	vUV;		//UV���W.
};

/*************************************************
*	Sprite�N���X.
**/
class clsSprite
	: public clsCommon	//���ʃN���X���p��.
{
private:
	//�����f���̎�ނ��Ƃɗp��.
	ID3D11VertexShader*		m_pVertexShader;//���_�V�F�[�_.
	ID3D11InputLayout*		m_pVertexLayout;//���_���C�A�E�g.
	ID3D11PixelShader*		m_pPixelShader;	//�s�N�Z���V�F�[�_.
	ID3D11Buffer*			m_pConstantBuffer;//�R���X�^���g�o�b�t�@.

	//�����f�����Ƃɗp��.
	ID3D11Buffer*			m_pVertexBuffer;//���_�o�b�t�@.

	ID3D11ShaderResourceView*	m_pTexture;//�e�N�X�`��.
	ID3D11SamplerState*			m_pSampleLinear;//�e�N�X�`���̃T���v��-:�e�N�X�`���[�Ɋe��t�B���^��������.

protected:

public:
	D3DXVECTOR3		m_vPos;	//�ʒu.
	D3DXVECTOR4 m_texUV;
	int				m_AnimCount;//UV�X�N���[���p�̃J�E���^.

	bool			m_bDispFlg;	//�\���t���O.

	clsSprite();	//�R���X�g���N�^.
	~clsSprite();	//�f�X�g���N�^.

	//������.
	HRESULT Init();

	//�V�F�[�_�쐬.
	HRESULT InitShader();
	//���f���쐬.
	HRESULT InitModel();

	//�`��(�����_�����O)
	void Render();
};

#endif//#ifndef _SPRITE_H_
