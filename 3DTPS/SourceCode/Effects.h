#ifndef _CEFFECTS_H_
#define _CEFFECTS_H_

//�x���ɂ��ẴR�[�h���͂𖳌��ɂ���B4005:�Ē�`.
#pragma warning( disable : 4005 )

//------------------------------------------------
//	��ɂ��Ă����w�b�_,���C�u�����̓Ǎ�.
//------------------------------------------------
#include <stdio.h>

#include <D3D11.h>
#include <XAudio2.h>
#pragma comment( lib, "d3d11.lib" )

//------------------------------------------------
//	��������Effekseer�֌W �w�b�_,���C�u�����̓Ǎ�.
//------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>

#if _DEBUG
#pragma comment( lib, "VS2013\\Debug\\Effekseer.lib" )
#pragma comment( lib, "VS2013\\Debug\\EffekseerRendererDX11.lib" )
#pragma comment( lib, "VS2013\\Debug\\EffekseerSoundXAudio2.lib" )
#else//#if _DEBUG
#pragma comment( lib, "VS2013\\Release\\Effekseer.lib" )
#pragma comment( lib, "VS2013\\Release\\EffekseerRendererDX11.lib" )
#pragma comment( lib, "VS2013\\Release\\EffekseerSoundXAudio2.lib" )
#endif//#if _DEBUG

//�uD3DX�`�v�g�p�ŕK�{.
//����:Effekseer�֌W����ɓǂݍ��ނ���.
#include <D3DX10.h>
#pragma comment( lib, "d3dx10.lib" )

/*************************************************
*	�t���[�\�t�g Effekseer�̃f�[�^���g�����߂̃N���X.
**/
class clsEffects
{
public:
	//�G�t�F�N�g��ޗ񋓑�.
	enum enEfcType
	{
		enEfcType_Laser = 0,
		enEfcType_MagicDark,
		enEfcType_Hihou,
		enEfcType_Max,
	};

	//�C���X�^���X�擾(�B��̃A�N�Z�X�o�H)
	static clsEffects* GetInstance()
	{
		//�B��̃C���X�^���X���쐬����.
		//(static�ō쐬���ꂽ�̂�2��ڂ͖��������)
		static clsEffects s_Instance;
		return &s_Instance;
	}
	//�f�X�g���N�^.
	~clsEffects();

	//�\�z�֐�.
	HRESULT Create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext);
	//�j���֐�.
	HRESULT Destroy();
	//�f�[�^�Ǎ��֐�.
	HRESULT LoadData();
	//�f�[�^����֐�.
	HRESULT ReleaseData();
	//�`��֐�.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		D3DXVECTOR3& vEye);
	//�r���[�s��ݒ�֐�.
	void SetViewMatrix(D3DXMATRIX& mView);
	//�v���W�F�N�V�����s��ݒ�֐�.
	void SetProjectionMatrix(D3DXMATRIX& mProj);

	//�Đ��֐�.
	::Effekseer::Handle Play(enEfcType enType, D3DXVECTOR3 vPos) {
		return m_pManager->Play(
			m_pEffect[enType], vPos.x, vPos.y, vPos.z);
	}
	//�ꎞ��~.
	void Paused(::Effekseer::Handle handle, bool bFlag) {
		m_pManager->SetPaused(handle, bFlag);//bFlag:true �ꎞ��~.
	}
	//��~.
	void Stop(::Effekseer::Handle handle) {
		m_pManager->StopEffect(handle);
	}
	//�S�Ē�~.
	void StopAll(::Effekseer::Handle handle) {
		m_pManager->StopAllEffects();
	}
	//�T�C�Y���w�肷��.
	void SetScale(::Effekseer::Handle handle, D3DXVECTOR3 vScale) {
		m_pManager->SetScale(handle, vScale.x, vScale.y, vScale.z);
	}
	//��]���w�肷��.
	void SetRotation(::Effekseer::Handle handle, D3DXVECTOR3 vRot) {
		m_pManager->SetRotation(handle, vRot.x, vRot.y, vRot.z);
	}
	void SetRotation(::Effekseer::Handle handle,
		D3DXVECTOR3 vAxis, float fAngle) {
		m_pManager->SetRotation(handle,
			::Effekseer::Vector3D(vAxis.x, vAxis.y, vAxis.z),
			fAngle);
	}
	//�ʒu���w�肷��.
	void SetLocation(::Effekseer::Handle handle, D3DXVECTOR3 vPos) {
		m_pManager->SetLocation(handle,
			::Effekseer::Vector3D(vPos.x, vPos.y, vPos.z));
	}

	//�x�N�^�[�ϊ��֐�.
	::Effekseer::Vector3D Vector3DxToEfk(
		const D3DXVECTOR3* pSrcVec3Dx);
	D3DXVECTOR3 Vector3EfkToDx(
		const ::Effekseer::Vector3D* pSrcVec3Efk);
	//�s��ϊ��֐�.
	::Effekseer::Matrix44 MatrixDxToEfk(
		const D3DXMATRIX* pSrcMatDx);
	D3DXMATRIX MatrixEfkToDx(
		const ::Effekseer::Matrix44* pSrcMatEfk);

private:
	//������R�s�[���֎~����.
	clsEffects();	//�R���X�g���N�^.
	clsEffects(const clsEffects& rhs);
	clsEffects& operator = (const clsEffects& rhs);

	//�G�t�F�N�g�𓮍삳���邽�߂ɕK�v.
	::Effekseer::Manager*			m_pManager;
	::EffekseerRenderer::Renderer*	m_pRender;
	::EffekseerSound::Sound*		m_pSound;
	//�G�t�F�N�g�f�[�^�Ɋ܂܂�鉹�Đ��ɕK�v.
	IXAudio2*						m_pXA2;
	IXAudio2MasteringVoice*			m_pXA2Master;

	//�G�t�F�N�g�̎�ނ��ƂɕK�v.
	::Effekseer::Effect*			m_pEffect[enEfcType_Max];
	//�G�t�F�N�g���ƂɕK�v.
	//�������G�t�F�N�g�𓯎��ɂR�o���Ȃ�R�K�v.
	::Effekseer::Handle				m_Handle;
};

#endif//#ifndef _CEFFECTS_H_