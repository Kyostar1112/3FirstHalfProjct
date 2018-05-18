#include"Resource.h"

clsResource::clsResource()
	: m_vsmpStaticModels(NULL)
{
	m_stSpriteRenderSet.hWnd = NULL;
	m_stSpriteRenderSet.pDevice11 = NULL;
	m_stSpriteRenderSet.pContext11 = NULL;
}

clsResource::~clsResource()
{
	//�X�^�e�B�b�N���b�V���̃|�C���^�̈���������.
	if (m_vsmpStaticModels.empty()) {
		for (size_t i = 0; i < m_vsmpStaticModels.size(); i++)
		{
			ReleaseStaticModel((enStaticModel)i);
		}
		m_vsmpStaticModels.clear();
		m_vsmpStaticModels.shrink_to_fit();
	}
	//�X�L�����b�V���̃|�C���^�̈���������.
	if (m_vsmpSkinModels.empty()) {
		for (size_t i = 0; i < m_vsmpSkinModels.size(); i++)
		{
			ReleaseSkinModel((enSkinModel)i);
		}
		m_vsmpSkinModels.clear();
		m_vsmpSkinModels.shrink_to_fit();
	}
	m_stSpriteRenderSet.hWnd = NULL;
	m_stSpriteRenderSet.pDevice11 = NULL;
	m_stSpriteRenderSet.pContext11 = NULL;
}

HRESULT clsResource::Init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_stSpriteRenderSet.hWnd = hWnd;
	m_stSpriteRenderSet.pDevice11 = pDevice;
	m_stSpriteRenderSet.pContext11 = pContext;

	m_si.hWnd = m_stSpriteRenderSet.hWnd;
	m_si.pDevice = m_stSpriteRenderSet.pDevice11;
	m_si.pDeviceContext = m_stSpriteRenderSet.pContext11;

	//�X�^�e�B�b�N���b�V���̃|�C���^�̈���m��.
	for (int i = 0; i < enStaticModel_Max; i++)
	{
		m_vsmpStaticModels.push_back(make_unique<clsDX9Mesh>());
		m_vsmpStaticModels[i] = NULL;	//NULL������.
	}

	//�X�L�����b�V���̃|�C���^�̈���m��.
	for (int i = 0; i < enSkinModel_Max; i++)
	{
		m_vsmpSkinModels.push_back(make_unique<clsD3DXSKINMESH>());
		m_vsmpStaticModels[i] = NULL;	//NULL������.
	}

	return S_OK;
}

HRESULT clsResource::CreateStaticModel(
	LPSTR FileName, enStaticModel enModel)
{
	//���f���Ǎ�.
	m_vsmpStaticModels[enModel] = make_unique<clsDX9Mesh>();
	m_vsmpStaticModels[enModel]->Init
	(m_stSpriteRenderSet.hWnd, m_stSpriteRenderSet.pDevice11, m_stSpriteRenderSet.pContext11, FileName);

	if (!IsRangeStaticModel(enModel)) {
		return E_FAIL;
	}
	//���f�������łɂȂ��H.
	if (m_vsmpStaticModels[enModel] == NULL) {
		return E_FAIL;
	}

	return S_OK;
}
HRESULT clsResource::ReleaseStaticModel(enStaticModel enModel)
{
	if (!IsRangeStaticModel(enModel)) {
		return E_FAIL;
	}
	//���f�������łɂȂ��H.
	if (m_vsmpStaticModels[enModel] == NULL) {
		return E_FAIL;
	}

	//�폜����NULL�Ŗ��߂�.
	m_vsmpStaticModels[enModel].reset();
	m_vsmpStaticModels[enModel] = NULL;

	return S_OK;
}
clsDX9Mesh* clsResource::GetStaticModels(enStaticModel enModel)
{
	//�͈͓��`�F�b�N.
	if (!IsRangeStaticModel(enModel)) {
		return NULL;
	}

	//���f�����邩�`�F�b�N.
	if (m_vsmpStaticModels[enModel] == NULL) {
		return NULL;
	}

	//���f���̃|�C���^��Ԃ�.
	return m_vsmpStaticModels[enModel].get();
}

//�͈͓����`�F�b�N����֐�.
//	�͈͓��Ɏ��܂��Ă���Ȃ�@true��Ԃ�.
bool clsResource::IsRangeStaticModel(enStaticModel enModel)
{
	//�񋓑͈̂̔͊O���H
	if (enStaticModel_Min < enModel  &&  enModel < enStaticModel_Max) {
		return true;
	}

	return false;
}

HRESULT clsResource::CreateSkinModel(
	LPSTR FileName, enSkinModel enModel)
{
	//���f���Ǎ�.
	m_vsmpSkinModels[enModel] = make_unique<clsD3DXSKINMESH>();

	m_vsmpSkinModels[enModel]->Init(&m_si);

	m_vsmpSkinModels[enModel]->CreateFromX(FileName);

	if (!m_vsmpSkinModels[enModel]) {
		return E_FAIL;
	}
	//���f�������łɂȂ��H.
	if (m_vsmpSkinModels[enModel] == NULL) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT clsResource::ReleaseSkinModel(enSkinModel enModel)
{
	if (!IsRangeSkinModel(enModel)) {
		return E_FAIL;
	}
	//���f�������łɂȂ��H.
	if (m_vsmpSkinModels[enModel] == NULL) {
		return E_FAIL;
	}

	//�폜����NULL�Ŗ��߂�.
	m_vsmpSkinModels[enModel].reset();
	m_vsmpSkinModels[enModel] = NULL;

	return S_OK;
}
clsD3DXSKINMESH* clsResource::GetSkinModels(enSkinModel enModel)
{
	//�͈͓��`�F�b�N.
	if (!IsRangeSkinModel(enModel)) {
		return NULL;
	}

	//���f�����邩�`�F�b�N.
	if (m_vsmpSkinModels[enModel] == NULL) {
		return NULL;
	}

	//���f���̃|�C���^��Ԃ�.
	return m_vsmpSkinModels[enModel].get();
}

//�͈͓����`�F�b�N����֐�.
//	�͈͓��Ɏ��܂��Ă���Ȃ�@true��Ԃ�.
bool clsResource::IsRangeSkinModel(enSkinModel enModel)
{
	//�񋓑͈̂̔͊O���H
	if (enSkinModel_Min < enModel  &&  enModel < enSkinModel_Max) {
		return true;
	}

	return false;
}

//�[�x�e�X�g(Z�e�X�g) ON/OFF�ؑ�.
void clsResource::SetDepth(bool bFlg)
{
	//�[�x�e�X�g(Z�e�X�g)��L���ɂ���.
	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc,
		sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = bFlg;

	GetSpriteRenderSet().pDevice11->CreateDepthStencilState(
		&depthStencilDesc, &m_pDepthStencilState);

	GetSpriteRenderSet().pContext11->OMSetDepthStencilState(
		m_pDepthStencilState, 1);
}

