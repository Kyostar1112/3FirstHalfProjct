#ifndef _DX9MESH_H_
#define _DX9MESH_H_
//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

#include <Windows.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#include <D3D10.h>
#include <d3dx9.h>
#include "MyMacro.h"

#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
//======================================
//	�\����.
//======================================
//�R���X�^���g�o�b�t�@�̃A�v�����̒�`(Mesh.hlsl)
//���V�F�[�_���̃R���X�^���g�o�b�t�@�ƈ�v���Ă���K�v����.
struct MESHSHADER_CONSTANT_BUFFER_ZERO
{
	D3DXMATRIX	mW;			//���[���h(�ʒu)���W�s��.
	D3DXMATRIX	mWVP;		//���[���h,�r���[,�ˉe�̍����ϊ��s��.
	D3DXVECTOR4 vLightDir;	//���C�g����.
	D3DXVECTOR4	vEye;		//�J�����ʒu(���_�ʒu).
	D3DXVECTOR4 vColor;		//�J���[.
};
struct MESHSHADER_CONSTANT_BUFFER1
{
	D3DXVECTOR4	vAmbient;	//�A���r�G���g.
	D3DXVECTOR4	vDiffuse;	//�f�B�t���[�Y.
	D3DXVECTOR4	vSpecular;	//�X�y�L����.
};

//���_�̍\����.
struct MeshVertex
{
	D3DXVECTOR3	vPos;	//�ʒu.
	D3DXVECTOR3	vNormal;//�@��(�A�e�v�Z�ɕK�{)
	D3DXVECTOR2	vTex;	//�e�N�X�`�����W.
};
//�}�e���A���\����.
struct MY_MATERIAL
{
	D3DXVECTOR4	Ambient;	//�A���r�G���g.
	D3DXVECTOR4	Diffuse;	//�f�B�t���[�Y.
	D3DXVECTOR4	Specular;	//�X�y�L����.
	CHAR szTextureName[128];//�e�N�X�`���t�@�C����.
	ID3D11ShaderResourceView* pTexture;//�e�N�X�`��.
	DWORD dwNumFace;	//���̃}�e���A���̃|���S����.
	//�R���X�g���N�^.
	MY_MATERIAL() {
		ZeroMemory(this, sizeof(MY_MATERIAL));
	}
	//�f�X�g���N�^.
	~MY_MATERIAL() {
		SAFE_RELEASE(pTexture);
	}
};

//B�{�b�N�X�\����.
struct BBOX
{
	D3DXVECTOR3 vPosMax;	//�ő咸�_.
	D3DXVECTOR3	vPosMin;	//�ŏ����_.
	D3DXVECTOR3	vAxisX;		//X��.
	D3DXVECTOR3 vAxisY;		//Y��.
	D3DXVECTOR3 vAxisZ;		//Z��.
	FLOAT		fLengthX;	//X����.
	FLOAT		fLengthY;	//Y����.
	FLOAT		fLengthZ;	//Z����.
	//������.
	BBOX() {
		ZeroMemory(this, sizeof(BBOX));
		vAxisX = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		vAxisY = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		vAxisZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
};

//���b�V���f�[�^���t�@�C��������o���ׂ����ɁADirectX9���g�p����.
//�������_�����O(�\��)�́ADirectX11�ōs��.
class clsDX9Mesh
{
public:
	HWND				m_hWnd;	//�E�B���h�E�n���h��.

	//Dx9.
	LPDIRECT3D9			m_pD3d;	//Dx9�I�u�W�F�N�g.
	LPDIRECT3DDEVICE9	m_pDevice9;//Dx9�f�o�C�X�I�u�W�F�N�g.

	//	D3DMATERIAL9*		m_pMaterials;	//�}�e���A�����.
	//	LPDIRECT3DTEXTURE9*	m_pTextures;	//�e�N�X�`�����.
	//	char		m_TexFileName[8][256];//�e�N�X�`���t�@�C����(8���܂�)
	DWORD		m_dwNumMaterials;//�}�e���A����.
	LPD3DXMESH	m_pMesh;	//���b�V���I�u�W�F�N�g.

	//Dx11.
	ID3D11Device*			m_pDevice11;//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*	m_pDeviceContext11;//�f�o�C�X�R���e�L�X�g.
	ID3D11VertexShader*		m_pVertexShader;//���_�V�F�[�_.
	ID3D11InputLayout*		m_pVertexLayout;//���_���C�A�E�g.
	ID3D11PixelShader*		m_pPixelShader;	//�s�N�Z���V�F�[�_.
	ID3D11Buffer*			m_pConstantBuffer0;//�R���X�^���g�o�b�t�@.
	ID3D11Buffer*			m_pConstantBuffer1;//�R���X�^���g�o�b�t�@.

	ID3D11Buffer*	m_pVertexBuffer;//���_(�o�[�e�b�N�X)�o�b�t�@.
	ID3D11Buffer**	m_ppIndexBuffer;//�C���f�b�N�X�o�b�t�@.
	//	ID3D11ShaderResourceView*	m_pTexture;//�e�N�X�`��.
	ID3D11SamplerState*			m_pSampleLinear;//�e�N�X�`���̃T���v��-:�e�N�X�`���[�Ɋe��t�B���^��������.

	MY_MATERIAL*	m_pMaterials;//�}�e���A���\����.
	DWORD			m_NumAttr;//������.
	DWORD			m_AttrID[300];//����ID ��300�����܂�.

	bool			m_bTexture;	//�e�N�X�`���̗L��.

	float			m_fScale;	//�g�k.
	float			m_fYaw;		//��](Y��)
	float			m_fPitch;	//��](X��)
	float			m_fRoll;	//��](Z��)
	D3DXVECTOR3		m_vPos;		//�ʒu(X,Y,Z)

	enDirection		m_enDir;	//����.

	bool			m_bShotFlg;	//�V���b�g�t���O.
	bool			m_bDeadFlg;	//���S�t���O.

	BBOX			m_BBox;		//B�{�b�N�X�\����.
	SPHERE			m_Sphere;	//�X�t�B�A�\����.

	D3DXVECTOR3		m_vRay;		//���C�̈ʒu.
	D3DXVECTOR3		m_vAxis;	//��]��.
	LPD3DXMESH		m_pMeshForRay;	//���C�ƃ��b�V���p.

	D3DXMATRIX m_mYaw;//���W�R������p.

	ID3D11BlendState*	m_pBlendState;//�u�����h�X�e�[�g.

	clsDX9Mesh();	//�R���X�g���N�^.
	~clsDX9Mesh();	//�f�X�g���N�^.

	HRESULT Init(HWND hWnd, ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, LPSTR fileName);
	//Dx9������.
	HRESULT InitDx9(HWND hWnd);
	//���b�V���Ǎ�.
	HRESULT LoadXMesh(LPSTR fileName);

	//�V�F�[�_�쐬.
	HRESULT InitShader();
	//�����_�����O�p(��DX9MESH����Main���łQ���݂���̂Œ���)
	void Render(D3DXMATRIX &mView, D3DXMATRIX &mProj,
		D3DXVECTOR3 &vLight, D3DXVECTOR3 &vEye,
		D3DXVECTOR4 vColor, bool alphaFlg);

	//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
	void SetBlend(bool flg);

	//�ʒu�X�V�֐�.
	D3DXVECTOR3 UpdatePos(D3DXVECTOR3 oldPos);
	//�ʒu�X�V�֐��̏�����.
	void InitPos();
	bool m_bInitPosFlg;
	D3DXVECTOR3 m_vecAxisZ;
	//����֐�.
	HRESULT Release();
};

#endif//#ifndef _DX9MESH_H_