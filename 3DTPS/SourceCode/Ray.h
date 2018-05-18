#ifndef _CRAY_H_
#define _CRAY_H_

#include "Common.h"	//���ʃN���X.

//���C�N���X(�\���݂̂��s��)
class clsRay
	: public clsCommon	//���ʃN���X���p��.
{
public:
	//======================================
	//	�\����.
	//======================================
	//�R���X�^���g�o�b�t�@�̃A�v�����̒�`(Ray.hlsl)
	//���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;		//���[���h,�r���[,�ˉe�̍����ϊ��s��.
		D3DXVECTOR4	vColor;		//�J���[(RGBA�̌^�ɍ��킹��)
	};
	//���_�\����.
	struct MODEL_VERTEX
	{
		D3DXVECTOR3	vPos;	//�ʒu.
	};
	//���C�\����.
	struct RAY
	{
		D3DXVECTOR3 vPoint[2];	//�n�_�A�I�_.
		D3DXVECTOR3	vDirection;	//����.
		D3DXVECTOR3	vPos;		//�ʒu.
		FLOAT		fYaw;		//Y����]�l.
		RAY()
		{
			ZeroMemory(this, sizeof(RAY));
		}
	};

	RAY		m_Ray;	//���C�\����.

	clsRay();	//�R���X�g���N�^.
	~clsRay();	//�f�X�g���N�^.

	//������.
	HRESULT Init(ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11);
	//�`��.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj);

private:
	//�V�F�[�_������.
	HRESULT InitShader();
	//���C������.
	HRESULT InitLine();

	//�����f���̎�ނ��Ƃɗp��.
	ID3D11VertexShader*		m_pVertexShader;//���_�V�F�[�_.
	ID3D11InputLayout*		m_pVertexLayout;//���_���C�A�E�g.
	ID3D11PixelShader*		m_pPixelShader;	//�s�N�Z���V�F�[�_.
	ID3D11Buffer*			m_pConstantBuffer;//�R���X�^���g�o�b�t�@.

	//�����f�����Ƃɗp��.
	ID3D11Buffer*			m_pVertexBuffer;//���_�o�b�t�@.
};

#endif//#ifndef _CRAY_H_