//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning( disable:4005 )

#include <Windows.h>

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>//�uD3DX�`�v�̒�`�g�p���ɕK�v.
#include <D3D10.h>

#include "DX9Mesh.h"	//Dx9���g�p����N���X.
#include "DebugText.h"	//�f�o�b�O�e�L�X�g�N���X.
#include "D3DXSKINMESH.h"	//�X�L�����b�V���N���X.

#include "DxInput.h"	//�R���g���[���N���X.
#include "Ray.h"		//���C�\���N���X.

#include <thread>

//�V�[���N���X�B.
#include "TitleScene.h"
#include "StageScene.h"
#include "OverScene.h"
#include "ClearScene.h"
#include "ResultScene.h"

//======================================
//	���C�u����.
//======================================
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )//�uD3DX�`�v�̒�`�g�p���ɕK�v.

//======================================
//	�萔.
//======================================
#define WND_TITLE	"�f�B�t�F���T�["
#define APR_NAME	"�f�B�t�F���T�["

const float WSPACE = 0.8f;	//�ǂƂ̌��E����.

//======================================
//	�\����.
//======================================
//�J�������.
struct CAMERA
{
	D3DXVECTOR3	vEye;	//�J�����ʒu.
	D3DXVECTOR3	vLook;	//�����ʒu.
	float		fYaw;	//��]�l.
	D3DXMATRIX	mRot;	//��]�s��.
};

/*************************************************
*	���C���N���X.
**/
class clsMain
{
private:
	HWND	m_hWnd;	//�E�B���h�E�n���h��.

public:
	clsMain();	//�R���X�g���N�^.
	~clsMain();	//�f�X�g���N�^.

	//�E�B���h�E�������֐�.
	HRESULT InitWindow(
		HINSTANCE hInstance,
		INT x, INT y, INT width, INT height,
		LPSTR WindowName);

	//�E�B���h�E�֐�(���b�Z�[�W���̏���)
	LRESULT MsgProc(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam);

	//���b�Z�[�W���[�v.
	void Loop();
	//�A�v���P�[�V�������C������.
	void AppMain();
	//�`��(�����_�����O)
	void Render();

private:
	//���A�v���ɂЂƂ�.
	ID3D11Device*			m_pDevice;	//�f�o�C�X�I�u�W�F�N�g.
	ID3D11DeviceContext*	m_pDeviceContext;//�f�o�C�X�R���e�L�X�g.
	IDXGISwapChain*			m_pSwapChain;//�X���b�v�`�F�[��.
	ID3D11RenderTargetView*	m_pBackBuffer_TexRTV;//�����_�[�^�[�Q�b�g�r���[.
	ID3D11Texture2D*		m_pBackBuffer_DSTex;//�o�b�N�o�b�t�@.
	ID3D11DepthStencilView*	m_pBackBuffer_DSTexDSV;//�f�v�X�X�e���V���r���[.

	D3DXVECTOR3		m_vLight;		//���C�g�̕���.

	CAMERA			m_Camera;		//�J�������.

	D3DXMATRIX		m_mView;		//�r���[(�J����)�s��.
	D3DXMATRIX		m_mProj;		//�v���W�F�N�V�����s��.

	int					m_AnimationNo;//�A�j���[�V�����ԍ�.
	double				m_AnimTime;	//�A�j���[�V��������.

	//�f�o�b�O�e�L�X�g�N���X.
	unique_ptr<clsDebugText>	m_smpText;

	//�R���g���[���N���X.
	unique_ptr<clsDxInput>	m_smpDxInput;

	//���C�\���N���X.
	unique_ptr<clsRay>			m_smpRayV;	//����.
	unique_ptr<clsRay>			m_smpRayFB;	//�O��.
	unique_ptr<clsRay>			m_smpRayH;	//���E.

	//�L�����N�^�N���X.
	int	m_iAnimModelArrayMax;

	clsResource*				m_pResource;//���\�[�X�N���X.

	//�����݂̃V�[��.
	SceneMode m_enScene;
	//�V�[�����ς�����u�Ԃ��Ƃ邽�߂̕ϐ�.
	SceneMode m_enOldScene;
	//�V�[����.
	vector<string> m_vsSceneName;

	//�V�[���N���X�B.
	unique_ptr<clsTitleScene>	m_smpTitleScene;
	unique_ptr<clsStageScene>	m_smpStageScene;
	unique_ptr<clsOverScene>	m_smpOverScene;
	unique_ptr<clsClearScene>	m_smpClearScene;
	unique_ptr<clsResultScene>	m_smpResultScene;

	unique_ptr<clsSound>	m_smpSeClick;

	//���[�h��ʗp.
	unique_ptr<clsSprite2D> m_smpLoadString;
	unique_ptr<clsSprite2D> m_smpLoadCircle;

	bool m_bLoadFlg;

	void SceneChange();
	bool m_bTitleFlg;

public:
	//DirectX������.
	HRESULT InitD3D();
	//Direct3D�I������.
	void DestroyD3D();

	//���b�V���Ǎ��֐��i�܂Ƃ߂��j
	HRESULT MeshRead();

	//���C�ƃ��b�V���̓����蔻��.
	bool Intersect(
		clsGameObject* pAttacker, clsCharacter* pTarget,
		float* pfDistance, D3DXVECTOR3* pvIntersect);
	//�����ʒu�̃|���S���̒��_��������.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pTarget, DWORD dwPolyIndex,
		D3DXVECTOR3* pVecVertices);

	//�ǂ̓����蔻��֘A.
	void WallJudge(clsGameObject* pAttacker, clsCharacter* pWall);
	void dirOverGuard(float* fYaw);//��]�l����.

	//�J�����֐�.
	void Camera();
	//�v���W�F�N�V�����֐�.
	void Proj();
};