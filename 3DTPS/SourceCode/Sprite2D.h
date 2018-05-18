#ifndef _SPRITE_2D_H_
#define _SPRITE_2D_H_

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

using namespace std;

//============================================================
//	�C���N���[�h.
//============================================================
#include "Common.h"//���ʃN���X.
#include <memory>
#include <string>
#include <d3dx9tex.h>
#define ALIGN16 _declspec( align( 16 ) )

//============================================================
//	�\����.
//============================================================
//�R���X�^���g�o�b�t�@�̃A�v�����̒�`(Sprite2D.hlsl).
//�V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
struct SPRITE2D_CONSTANT_BUFFER
{
	ALIGN16 D3DXMATRIX	mW;				//���[���h�s��.
	ALIGN16 float		fViewPortWidth;	//�r���[�|�[�g��.
	ALIGN16 float		fViewPortHeight;//�r���[�|�[�g����.
	ALIGN16 float		fAlpha;			//�A���t�@�l(����).
	ALIGN16 D3DXVECTOR2 vUV;			//UV���W.
};
//���_�̍\����.
struct Sprite2DVertex
{
	D3DXVECTOR3	vPos;	//�ʒu.
	D3DXVECTOR2	vTex;	//�e�N�X�`�����W.
};
//////�X�v���C�g�\����.
//struct SPRITE_STATE
//{
//	WHSIZE_FLOAT	Disp;	//�\��������.
//	WHSIZE_FLOAT	Base;	//���摜����.
//	WHSIZE_FLOAT	Stride;	//��R�}������̍���.
//};//m_SState.
//�X�v���C�g�\����.
struct SPRITE_STATE
{
	WHSIZE_FLOAT	Disp;	//�\��������.
	WHSIZE_FLOAT	Base;	//���摜����.
	//WHSIZE_FLOAT	Stride;	//��R�}������̍���.
	WHSIZE_FLOAT	Stride;	//�R�}�̐�.
};//m_SState.
enum en_BlackMode
{
	Idle = 0,
	In,
	Change,
	Out,
};

//============================================================
//	�X�v���C�g2D�N���X.
//============================================================
class clsSprite2D
	: public clsCommon
{
public:
	clsSprite2D();	//�R���X�g���N�^.
	virtual ~clsSprite2D();	//�f�X�g���N�^.

	//������.
	HRESULT Create(ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		LPSTR fileName);

	////�X�v���C�g����ʊO��.
	//void InitSetPos()
	//{
	//	m_vPos = D3DXVECTOR3( WND_W, WND_H, 0.0f );
	//}

	void SetPos(D3DXVECTOR3 Pos)
	{
		m_vPos = Pos;
	}

	void AddPosX(float Add)
	{
		m_vPos.x += Add;
	}

	void AddPosY(float Add)
	{
		m_vPos.y += Add;
	}

	void AddPosZ(float Add)
	{
		m_vPos.z += Add;
	}

	void SetPosX(float PosX)
	{
		m_vPos.x = PosX;
	}

	void SetPosY(float PosY)
	{
		m_vPos.y = PosY;
	}

	void SetPosZ(float PosZ)
	{
		m_vPos.z = PosZ;
	}

	void SetPos(float x, float y, float z = 0.0f)
	{
		m_vPos.x = x;
		m_vPos.y = y;
		m_vPos.z = z;
	}

	void SetAlpha(float Alpha)
	{
		m_fAlpha = Alpha;
	}

	void AddAlpha(float Alpha)
	{
		m_fAlpha += Alpha;
	}
	void SetStrideX(float StrideW)
	{
		m_SState.Stride.w = StrideW;		//���R�}��.
	}
	void SetStrideY(float StrideH)
	{
		m_SState.Stride.h = StrideH;		//�c�R�}��.
	}

	//���̃p�^�[��.
	void SetPatarnU(float Patarn)
	{
		m_fPatternNo.x = Patarn;
	}

	//�c�̃p�^�[��.
	void SetPatarnV(float Patarn)
	{
		m_fPatternNo.y = Patarn;
	}

	void SetSs(SPRITE_STATE ss)
	{
		m_SState = ss;
	}

	//SPRITE_STATE�ɃZ�b�g.
	void SetSs(
		float BaseW,			//���摜����.
		float BaseH,		    //���摜��.
		float DispW = -1,	    //�\������.//��������ĂȂ�������Base�ɍ��킹��.
		float DispH = -1,       //�\����.//��������ĂȂ�������Base�ɍ��킹��.
		float StrideW = 1,	    //�c�R�}��.
		float StrideH = 1)	    //���R�}��.
	{
		m_SState.Base.h = BaseH;		//���摜����.
		m_SState.Base.w = BaseW;		//���摜��.
		if (DispH < 0)
		{
			m_SState.Disp.h = m_SState.Base.h;
		}
		else
		{
			m_SState.Disp.h = DispH;		//�\������.
		}
		if (DispW < 0)
		{
			m_SState.Disp.w = m_SState.Base.w;	//�\����.
		}
		else
		{
			m_SState.Disp.w = DispW;		//�\����.
		}
		m_SState.Stride.h = StrideH;		//�c�R�}��.
		m_SState.Stride.w = StrideW;		//���R�}��.
	}

	void SetDispH(float DispH)
	{
		m_SState.Disp.h = DispH;
	}

	void SetDispW(float DispW)
	{
		m_SState.Disp.w = DispW;
	}

	void AddDispH(float DispH)
	{
		m_SState.Disp.h += DispH;
	}

	void AddDispW(float DispW)
	{
		m_SState.Disp.w += DispW;
	}

	void MulDisp(float Disp)
	{
		m_SState.Disp.w = Disp * (m_SState.Base.w / m_SState.Stride.w);
		m_SState.Disp.h = Disp * (m_SState.Base.h / m_SState.Stride.h);
	}

	float GetAlpha()
	{
		return m_fAlpha;
	}

	SPRITE_STATE GetSs()
	{
		return m_SState;
	}

	WHSIZE_FLOAT GetCenterDisp()
	{
		WHSIZE_FLOAT tmp;
		tmp.w = m_SState.Disp.w / 2;
		tmp.h = m_SState.Disp.h / 2;
		return tmp;
	}

	D3DXVECTOR3 GetPos()
	{
		return m_vPos;
	}

	//SPRITE_STATE�ύX��K�p���邽�߂ɕK�v.
	virtual void UpDateSpriteSs();

	//�_�ŗp.
	void Flashing(float ChaAmo, float AlphaMax = 1.0f, float AlphaMin = 0.0f);

	//�`��(�����_�����O)(��DX9MESH����Main����2���݂���̂Œ���).
	virtual void Render();
	en_BlackMode m_BlackMode;

	bool Release();

protected:

	//�V�F�[�_�쐬.
	HRESULT InitShader();
	////���f���쐬.
	virtual HRESULT InitModel(SPRITE_STATE ss);
	//�e�N�X�`���쐬.
	HRESULT CreateTexture(LPSTR fileName,
		ID3D11ShaderResourceView** pTex);

	bool			m_bFlashing;

	float			MulPoint = 1.0f;//�|���Z�p�̕ϐ�.

	string			m_sFileName;

	D3DXVECTOR3		m_vPos;	//�ʒu.

	int				m_AnimCount;//UV�X�N���[��.

	bool			m_bDispFlg;//�\���t���O.

	float			m_fAlpha;	//�A���t�@�l.

	SPRITE_STATE	m_SState;	//�X�v���C�g���.
	POINTFLOAT		m_fPatternNo;//�}�X�ڔԍ�.

	//�����f���̎�ނ��Ƃɗp��.
	ID3D11VertexShader*		m_pVertexShader;	//���_�V�F�[�_.
	ID3D11InputLayout*		m_pVertexLayout;	//���_���C�A�E�g.
	ID3D11PixelShader*		m_pPixelShader;		//�s�N�Z���V�F�[�_.
	ID3D11Buffer*			m_pConstantBuffer;	//�R���X�^���g�o�b�t�@.
	ID3D11ShaderResourceView*	m_pTexture;		//�e�N�X�`��.

	//�����f�����Ƃɗp��.
	ID3D11Buffer*			m_pVertexBuffer;	//���_�o�b�t�@.

	ID3D11SamplerState*		m_pSampleLinear;//�e�N�X�`���̃T���v���[:/�e�N�X�`���Ɋe��t�B���^��������.
};

#endif//#define _SPRITE_2D_H_
