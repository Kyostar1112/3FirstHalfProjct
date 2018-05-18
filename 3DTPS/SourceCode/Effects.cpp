#include "Effects.h"
#include "Global.h"

//�`��p�C���X�^���X�ő吔.
const int g_RenderSpriteMax = 2000;
//�G�t�F�N�g�Ǘ��p�C���X�^���X�ő吔.
const int g_EffectInstanceMax = 1000;

//�G�t�F�N�g�t�@�C���̃��X�g.
const wchar_t g_strFileNameList
[clsEffects::enEfcType_Max][128] =
{
	L"Data\\Effekseer\\Laser03.efk",
	L"Data\\Effekseer\\MagicDark.efk",
	L"Data\\Effekseer\\miko_hihou_effect.efk"
};

//�R���X�g���N�^.
clsEffects::clsEffects()
{
	ZeroMemory(this, sizeof(clsEffects));
}

//�f�X�g���N�^.
clsEffects::~clsEffects()
{
}

//�\�z�֐�.
HRESULT clsEffects::Create(
	ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext)
{
	//XAudio2�̏��������s��.
	if (FAILED(
		XAudio2Create(&m_pXA2)))
	{
		ERR_MSG("XAudio2�쐬���s", "CEffects::Create()");
		return E_FAIL;
	}
	if (FAILED(
		m_pXA2->CreateMasteringVoice(&m_pXA2Master)))
	{
		ERR_MSG("MasteringVoice�쐬���s", "CEffects::Create()");
		return E_FAIL;
	}

	//�`��p�C���X�^���X�̐���.
	m_pRender
		= ::EffekseerRendererDX11::Renderer::Create(
			pDevice, pContext, g_RenderSpriteMax);

	//�G�t�F�N�g�Ǘ��p�C���X�^���X�̐���.
	m_pManager
		= ::Effekseer::Manager::Create(g_EffectInstanceMax);

	//�`��p�C���X�^���X����`��@�\��ݒ�.
	m_pManager->SetSpriteRenderer(m_pRender->CreateSpriteRenderer());
	m_pManager->SetRibbonRenderer(m_pRender->CreateRibbonRenderer());
	m_pManager->SetRingRenderer(m_pRender->CreateRingRenderer());
	m_pManager->SetModelRenderer(m_pRender->CreateModelRenderer());

	//�`��C���X�^���X����e�N�X�`���̓Ǎ��@�\��ݒ�.
	//�Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���.
	m_pManager->SetTextureLoader(m_pRender->CreateTextureLoader());
	m_pManager->SetModelLoader(m_pRender->CreateModelLoader());

	//���Đ��p�C���X�^���X�̐���.
	m_pSound = ::EffekseerSound::Sound::Create(m_pXA2, 16, 16);

	//���Đ��p�C���X�^���X����Đ��@�\��ݒ�.
	m_pManager->SetSoundPlayer(m_pSound->CreateSoundPlayer());

	//���Đ��p�C���X�^���X����T�E���h�f�[�^�̓Ǎ��@�\��ݒ�.
	//�Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���.
	m_pManager->SetSoundLoader(m_pSound->CreateSoundLoader());

	return S_OK;
}

//�j���֐�.
HRESULT clsEffects::Destroy()
{
	//�G�t�F�N�g�f�[�^�����.
	ReleaseData();

	//��ɃG�t�F�N�g�Ǘ��p�C���X�^���X��j��.
	m_pManager->Destroy();

	//���ɉ��Đ��p�C���X�^���X��j��.
	m_pSound->Destory();

	//���ɕ`��p�C���X�^���X��j��.
	m_pRender->Destory();

	//XAudio2�̉��.
	if (m_pXA2Master != NULL) {
		m_pXA2Master->DestroyVoice();
		m_pXA2Master = NULL;
	}
	ES_SAFE_RELEASE(m_pXA2);

	return S_OK;
}

//�f�[�^�Ǎ��֐�.
HRESULT clsEffects::LoadData()
{
	////�G�t�F�N�g�̓Ǎ�.
	//m_pEffect
	//	= ::Effekseer::Effect::Create(
	//			m_pManager,
	//			(const EFK_CHAR*)L"Data\\Effekseer\\Laser03.efk" );
	//if( m_pEffect == NULL ){
	//	ERR_MSG( "�G�t�F�N�g�t�@�C���Ǎ����s", "clsEffects::LoadData()" );
	//	return E_FAIL;
	//}

	//�G�t�F�N�g�̓Ǎ�.
	for (int i = 0; i < enEfcType_Max; i++) {
		m_pEffect[i]
			= ::Effekseer::Effect::Create(
				m_pManager,
				(const EFK_CHAR*)g_strFileNameList[i]);
		if (m_pEffect[i] == NULL) {
			char strMsg[128];
			wsprintf(strMsg, "clsEffects::LoadData()\n%ls",
				g_strFileNameList[i]);
			ERR_MSG(strMsg, "�G�t�F�N�g�t�@�C���Ǎ����s");
			return E_FAIL;
		}
	}

	return S_OK;
}

//�f�[�^����֐�.
HRESULT clsEffects::ReleaseData()
{
	//�G�t�F�N�g�̔j��.
	for (int i = 0; i < enEfcType_Max; i++) {
		ES_SAFE_RELEASE(m_pEffect[i]);
	}

	return S_OK;
}

//�`��֐�.
void clsEffects::Render(
	D3DXMATRIX& mView, D3DXMATRIX& mProj, D3DXVECTOR3& vEye)
{
	//�r���[�s���ݒ�.
	SetViewMatrix(mView);

	//�v���W�F�N�V�����s���ݒ�.
	SetProjectionMatrix(mProj);

	//�G�t�F�N�g�̍X�V����.
	m_pManager->Update();

	//-------------------------------------
	//	Effekseer�����_�����O.
	//-------------------------------------
	//�G�t�F�N�g�̕`��J�n�������s��.
	m_pRender->BeginRendering();

	//�G�t�F�N�g�̕`����s��.
	m_pManager->Draw();

	//�G�t�F�N�g�̕`��I���������s��.
	m_pRender->EndRendering();
}

//�r���[�s��ݒ�֐�.
void clsEffects::SetViewMatrix(D3DXMATRIX& mView)
{
	//�r���[(�J����)�s��.
	::Effekseer::Matrix44 tmpEsCamMat;

	//DirectX Matrix �� Effekseer Matrix �ɕϊ�.
	tmpEsCamMat = MatrixDxToEfk(&mView);

	//�r���[�s���ݒ�.
	m_pRender->SetCameraMatrix(tmpEsCamMat);
}

//�v���W�F�N�V�����s��ݒ�֐�.
void clsEffects::SetProjectionMatrix(D3DXMATRIX& mProj)
{
	//�v���W�F�N�V�����s��.
	::Effekseer::Matrix44 tmpEsPrjMat;

	//DirectX Matrix �� Effekseer Matrix �ɕϊ�.
	tmpEsPrjMat = MatrixDxToEfk(&mProj);

	//�v���W�F�N�V�����s���ݒ�.
	m_pRender->SetProjectionMatrix(tmpEsPrjMat);
}

// DirectX Vector3 �� Effekseer Vector3 �ɕϊ�����.
::Effekseer::Vector3D clsEffects::Vector3DxToEfk(
	const D3DXVECTOR3* pSrcVec3Dx)
{
	return ::Effekseer::Vector3D(
		pSrcVec3Dx->x,
		pSrcVec3Dx->y,
		pSrcVec3Dx->z);
}

// Effekseer Vector3 �� DirectX Vector3 �ɕϊ�����.
D3DXVECTOR3 clsEffects::Vector3EfkToDx(
	const ::Effekseer::Vector3D* pSrcVec3Efk)
{
	return D3DXVECTOR3(
		pSrcVec3Efk->X,
		pSrcVec3Efk->Y,
		pSrcVec3Efk->Z);
}

// DirectX Matrix �� Effekseer Matrix �ɕϊ�����.
::Effekseer::Matrix44 clsEffects::MatrixDxToEfk(
	const D3DXMATRIX* pSrcMatDx)
{
	::Effekseer::Matrix44 OutMatEfk;

#if 1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatEfk.Values[i][j] = pSrcMatDx->m[i][j];
		}
	}
#else
	OutMatEfk.Values[0][0] = pSrcMatDx->_11;
	OutMatEfk.Values[0][1] = pSrcMatDx->_12;
	OutMatEfk.Values[0][2] = pSrcMatDx->_13;
	OutMatEfk.Values[0][3] = pSrcMatDx->_14;

	OutMatEfk.Values[1][0] = pSrcMatDx->_21;
	OutMatEfk.Values[1][1] = pSrcMatDx->_22;
	OutMatEfk.Values[1][2] = pSrcMatDx->_23;
	OutMatEfk.Values[1][3] = pSrcMatDx->_24;

	OutMatEfk.Values[2][0] = pSrcMatDx->_31;
	OutMatEfk.Values[2][1] = pSrcMatDx->_32;
	OutMatEfk.Values[2][2] = pSrcMatDx->_33;
	OutMatEfk.Values[2][3] = pSrcMatDx->_34;

	OutMatEfk.Values[3][0] = pSrcMatDx->_41;
	OutMatEfk.Values[3][1] = pSrcMatDx->_42;
	OutMatEfk.Values[3][2] = pSrcMatDx->_43;
	OutMatEfk.Values[3][3] = pSrcMatDx->_44;
#endif

	return OutMatEfk;
}

// Effekseer Matrix �� DirectX Matrix �ɕϊ�����.
D3DXMATRIX clsEffects::MatrixEfkToDx(
	const ::Effekseer::Matrix44* pSrcMatEfk)
{
	D3DXMATRIX OutMatDx;

#if 1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatDx.m[i][j] = pSrcMatEfk->Values[i][j];
		}
	}
#else
	OutMatDx._11 = pSrcMatEfk->Values[0][0];
	OutMatDx._12 = pSrcMatEfk->Values[0][1];
	OutMatDx._13 = pSrcMatEfk->Values[0][2];
	OutMatDx._14 = pSrcMatEfk->Values[0][3];

	OutMatDx._21 = pSrcMatEfk->Values[1][0];
	OutMatDx._22 = pSrcMatEfk->Values[1][1];
	OutMatDx._23 = pSrcMatEfk->Values[1][2];
	OutMatDx._24 = pSrcMatEfk->Values[1][3];

	OutMatDx._31 = pSrcMatEfk->Values[2][0];
	OutMatDx._32 = pSrcMatEfk->Values[2][1];
	OutMatDx._33 = pSrcMatEfk->Values[2][2];
	OutMatDx._34 = pSrcMatEfk->Values[2][3];

	OutMatDx._41 = pSrcMatEfk->Values[3][0];
	OutMatDx._42 = pSrcMatEfk->Values[3][1];
	OutMatDx._43 = pSrcMatEfk->Values[3][2];
	OutMatDx._44 = pSrcMatEfk->Values[3][3];
#endif

	return OutMatDx;
}