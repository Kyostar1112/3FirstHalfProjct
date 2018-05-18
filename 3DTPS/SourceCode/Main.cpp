#include "Main.h"
#include "Global.h"
#include "MyMacro.h"
#include <stdio.h>

//�O���[�o���ϐ�.

//�萔.

clsMain*	g_pClsMain = NULL;

//======================================
//	���C���֐�.
//======================================
INT WINAPI WinMain(
	HINSTANCE hInstance,	//�C���X�^���ԍ�(�E�B���h�E�̔ԍ�)
	HINSTANCE hPrevInstance,
	PSTR lpCmdLine,
	INT nCmdShow)
{
	//g_pClsMain = make_unique<clsMain>();	//������&�N���X�̐錾.
	g_pClsMain = new clsMain;	//������&�N���X�̐錾.

	//�N���X�����݂��Ă邩�`�F�b�N.
	if (g_pClsMain != NULL) {
		//�E�B���h�E�쐬����.
		if (SUCCEEDED(
			g_pClsMain->InitWindow(
				hInstance,
				0, 0,
				WND_W, WND_H,
				WND_TITLE)))
		{
			//Dx11�p�̏�����.
			if (SUCCEEDED(g_pClsMain->InitD3D()))
			{
				//���b�Z�[�W���[�v.
				g_pClsMain->Loop();
			}
		}
		//�I��.
		g_pClsMain->DestroyD3D();//Direct3D�̉��.

		SAFE_DELETE(g_pClsMain);
	}
	return 0;
}

//======================================
//	�E�B���h�E�v���V�[�W��.
//======================================
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	//�v���V�[�W��.
	return g_pClsMain->MsgProc(hWnd, uMsg, wParam, lParam);
}

/*************************************************
*	���C���N���X.
**/
//======================================
//	�R���X�g���N�^.
//======================================
clsMain::clsMain()
{
	ZeroMemory(this, sizeof(clsMain));

	m_hWnd = NULL;

	m_pDevice = NULL;	//�f�o�C�X�I�u�W�F�N�g.
	m_pDeviceContext = NULL;	//�f�o�C�X�R���e�L�X�g.
	m_pSwapChain = NULL;	//�X���b�v�`�F�[��.

	m_pBackBuffer_TexRTV = NULL;//�����_�[�^�[�Q�b�g�r���[.
	m_pBackBuffer_DSTex = NULL;//�o�b�N�o�b�t�@.
	m_pBackBuffer_DSTexDSV = NULL;//�f�v�X�X�e���V���r���[.

	//�J����(���_)�ʒu.
	m_Camera.vEye = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//�����ʒu.
	m_Camera.vLook = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//���C�g����.
	m_vLight = D3DXVECTOR3(0.0f, 0.5f, -1.0f);

	m_enScene = Title;

	m_vsSceneName.push_back("GameMode[Title]");
	m_vsSceneName.push_back("GameMode[Stage]");
	m_vsSceneName.push_back("GameMode[Over]");
	m_vsSceneName.push_back("GameMode[Clear]");
	m_vsSceneName.push_back("GameMode[Result]");
}

//======================================
//	�f�X�g���N�^.
//======================================
clsMain::~clsMain()
{
}

//======================================
//	�E�B���h�E�������֐�.
//======================================
HRESULT clsMain::InitWindow(
	HINSTANCE hInstance,	//�C���X�^���X.
	INT x, INT y,			//�E�B���h�E��x,y���W.
	INT width, INT height,	//�E�B���h�E�̕�,����.
	LPSTR WindowName)		//�E�B���h�E��.
{
	//�E�B���h�E�̒�`.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));//������(0�ݒ�)

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = APR_NAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//�E�B���h�E�N���X��Windows�ɓo�^.
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL,
			"�E�B���h�E�N���X�̓o�^�Ɏ��s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�E�B���h�E�̍쐬.
	m_hWnd = CreateWindow(
		APR_NAME,	//�A�v����.
		WindowName,	//�E�B���h�E�^�C�g��.
		WS_OVERLAPPEDWINDOW,//�E�B���h�E���(����)
		0, 0,			//�\���ʒux,y���W.
		width, height,	//�E�B���h�E�̕�,����.
		NULL,			//�e�E�B���h�E�n���h��.
		NULL,			//���j���[�ݒ�.
		hInstance,		//�C���X�^���X�ԍ�.
		NULL);			//�E�B���h�E�쐬���ɔ�������C�x���g�ɓn���f�[�^.

	if (!m_hWnd) {
		MessageBox(NULL,
			"�E�B���h�E�쐬�Ɏ��s", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�E�B���h�E�̕\��.
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return S_OK;
}

//======================================
//	�E�B���h�E�֐�(���b�Z�[�W���̏���)
//======================================
LRESULT clsMain::MsgProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY://�E�B���h�E���j�����ꂽ��.
		//�A�v���P�[�V�����̏I����Windows�ɒʒm����.
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN://�L�[�{�[�h�������ꂽ�Ƃ�.

		//�L�[�ʂ̏���.
		switch ((char)wParam) {
		case VK_ESCAPE:	//ESC�L�[.
			if (MessageBox(NULL,
				"�Q�[�����I�����܂����H", "�x��",
				MB_YESNO) == IDYES)
			{
				//�E�B���h�E��j������.
				DestroyWindow(hWnd);
			}
			break;
		}
		break;
	}

	//���C���ɕԂ����.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//================================================
//	���b�Z�[�W���[�v�ƃA�v���P�[�V���������̓����.
//================================================
void clsMain::Loop()
{
	//=== DirectInput���������� ===//
	//DxInput�I�u�W�F�N�g�̍쐬.
	m_smpDxInput = make_unique<clsDxInput>();
	m_smpDxInput->initDI(m_hWnd);

	//���b�V���Ǎ��܂Ƃ߂�����.
	MeshRead();

	//�f�o�b�O�e�L�X�g�̏�����.
	m_smpText = make_unique<clsDebugText>();
	D3DXVECTOR4 vColor(1.0f, 1.0f, 1.0f, 1.0f);
	if (FAILED(m_smpText->Init(
		m_pDeviceContext,
		WND_W, WND_H, 50.0f,
		vColor)))			//�����F.
	{
		MessageBox(NULL,
			"�f�o�b�O�e�L�X�g�쐬���s", "error", MB_OK);
	}

	//-------------------------------------
	//	�t���[�����[�g���ߏ���.
	//-------------------------------------
	float fRate = 0.0f;	//���[�g.
	float fFPS = 60.0f;//FPS�l.
	DWORD sync_old = timeGetTime();//�ߋ�����.
	DWORD sync_now;					//���ݎ���.

	//���ԏ����ׁ̈A�ŏ��P�ʂ�1�~���b�ɕύX.
	timeBeginPeriod(1);

	//���b�Z�[�W���[�v.
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		Sleep(1);
		sync_now = timeGetTime();//���ݎ��Ԃ��擾.

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		fRate = 1000.0f / fFPS;//���z���Ԃ��Z�o.
		if (sync_now - sync_old >= fRate)
		{
			sync_old = sync_now;//���ݎ��Ԃɒu������.

			//�A�v���P�[�V�����̏����͂���������.
			AppMain();
		}
	}
	//�A�v���P�[�V�����̏I��.
	timeEndPeriod(1);	//����.
}

//================================================
//	�A�v���P�[�V�������C������.
//================================================
void clsMain::AppMain()
{
	//�R���g���[�����͏��X�V.
	m_smpDxInput->UpdateInputState();
	float fBlackSpeed = 0.01f;
	if (m_smpSeClick->IsPlaying())
	{
		m_smpSeClick->SeekToStart();
	}

	if (GetAsyncKeyState('C') & 0x8000) {
		if (CameaVp >= 2 )
		{
			CameaVp = 0;
		}
		CameaVp++;
		m_pDeviceContext->RSSetViewports(1, &m_vp[CameaVp]);
		Resource->SetSpriteRender(m_hWnd,m_pDevice,m_pDeviceContext);
	}

	switch (m_enScene)
	{
	case Title:
	{
		m_smpTitleScene->UpDate();
		if (GetAsyncKeyState('I') & 0x8000) {
			m_bTitleFlg = false;
		}
		if (GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000 || m_smpDxInput->IsPressKey(enPKey_Down)) {
			m_bTitleFlg = true;
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x0001 || m_smpDxInput->IsPressKey(enPKey_00)) {
			m_smpSeClick->Play();
			m_smpTitleScene->BlackStart(fBlackSpeed);
		}
		if (m_smpTitleScene->GetBlackEnd())
		{
			if (m_bTitleFlg)
			{
				m_enScene = Stage;
			}
			else
			{
				m_enScene = Result;
			}
		}
	}
	break;
	case Stage:
	{
		m_smpStageScene->UpDate();
		if (m_smpStageScene->GetMode() != Stage) {
			m_smpStageScene->BlackStart(fBlackSpeed);
		}
		if (m_smpStageScene->GetBlackEnd())
		{
			m_enScene = m_smpStageScene->GetMode();
		}
		//�U�����{�^����������Ă��邩�ǂ���.
		bool bAttackFlg = false;
		float fMovePoint = 0.03f;
		if (GetAsyncKeyState('Z') & 0x0001 || GetAsyncKeyState(VK_SPACE) & 0x8000 || m_smpDxInput->IsPressKey(enPKey_00)) {
			m_smpStageScene->Fire();
			bAttackFlg = true;
		}
		if (bAttackFlg)
		{
			fMovePoint = 0.015f;
		}
		if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000 || m_smpDxInput->IsPressKey(enPKey_Left)) {
			m_smpStageScene->LeftRoll(fMovePoint);
		}
		if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000 || m_smpDxInput->IsPressKey(enPKey_Right)) {
			m_smpStageScene->RightRoll(fMovePoint);
		}
	}
	break;
	case Over:
	{
		m_smpOverScene->UpDate();
		if (GetAsyncKeyState(VK_SPACE) & 0x0001 || m_smpDxInput->IsPressKey(enPKey_00)) {
			m_smpSeClick->Play();
			m_smpOverScene->BlackStart(fBlackSpeed);
		}
		if (m_smpOverScene->GetBlackEnd())
		{
			m_enScene = Title;
		}
	}
	break;
	case Clear:
	{
		m_smpClearScene->UpDate();
		if (GetAsyncKeyState(VK_SPACE) & 0x0001 || m_smpDxInput->IsPressKey(enPKey_00)) {
			m_smpSeClick->Play();
			m_smpClearScene->BlackStart(fBlackSpeed);
		}
		if (m_smpClearScene->GetBlackEnd())
		{
			m_enScene = Title;
		}
	}
	break;
	case Result:
	{
		m_bTitleFlg = true;
		m_smpResultScene->UpDate();
		if (GetAsyncKeyState(VK_SPACE) & 0x0001 || m_smpDxInput->IsPressKey(enPKey_00)) {
			m_smpSeClick->Play();
			m_smpResultScene->BlackStart(fBlackSpeed);
		}
		if (m_smpResultScene->GetBlackEnd())
		{
			m_enScene = Title;
		}
	}
	break;
	}

	SceneChange();

	//�����_�����O.
	Render();
}

//================================================
//	�V�[��(���)����ʂɃ����_�����O(�`��)
//================================================
void clsMain::Render()
{
	//��ʂ̃N���A.
	float ClearColor[4] = { 0.0f, 0.0f, 0.5f, 1.0f };//�N���A�F(RGBA��)
	//�J���[�o�b�N�o�b�t�@.
	m_pDeviceContext->ClearRenderTargetView(
		m_pBackBuffer_TexRTV, ClearColor);
	//�f�v�X�X�e���V���o�b�N�o�b�t�@.
	m_pDeviceContext->ClearDepthStencilView(
		m_pBackBuffer_DSTexDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);

	//�J�����֐�.
	Camera();
	//�v���W�F�N�V�����֐�.
	Proj();

	m_pResource->SetModelRender(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_pResource->SetSpriteRender(m_hWnd, m_pDevice, m_pDeviceContext);

	switch (m_enScene)
	{
	case Title:
		m_smpTitleScene->Render();
		break;
	case Stage:
		m_smpStageScene->Render();
		break;
	case Over:
		m_smpOverScene->Render();
		break;
	case Clear:
		m_smpClearScene->Render();
		break;
	case Result:
		m_smpResultScene->Render();
		break;
	}

#ifdef _DEBUG
	//�f�o�b�O�e�L�X�g.
	if (m_smpText != NULL)//NULL�`�F�b�N.
	{
		char strDbgTxt[256];
		int iDbgY = 10;
		const int CDBGY = 30;

		sprintf_s(strDbgTxt, "Ply:x[%f],y[%f],z[%f]",
			m_smpStageScene->m_smpPlayer->GetPositionX(),
			m_smpStageScene->m_smpPlayer->GetPositionY(),
			m_smpStageScene->m_smpPlayer->GetPositionZ());
		m_smpText->Render(strDbgTxt, 0, iDbgY);
		iDbgY += CDBGY;

		sprintf_s(strDbgTxt, "PlyRot:y[%f]",
			m_smpStageScene->m_smpPlayer->GetRotationY());
		m_smpText->Render(strDbgTxt, 0, iDbgY);
		iDbgY += CDBGY;

		sprintf_s(strDbgTxt, "Enemy:x[%f],y[%f],z[%f]",
			m_smpStageScene->m_vsmpEnemy[0]->GetPositionX(),
			m_smpStageScene->m_vsmpEnemy[0]->GetPositionY(),
			m_smpStageScene->m_vsmpEnemy[0]->GetPositionZ());
		m_smpText->Render(strDbgTxt, 0, iDbgY);
		iDbgY += CDBGY;

		sprintf_s(strDbgTxt, m_vsSceneName[static_cast<int>(m_enScene)].c_str(),
			m_smpStageScene->m_smpPlayer->GetRotationY());
		m_smpText->Render(strDbgTxt, 0, iDbgY);
		iDbgY += CDBGY;

	}
#endif//#ifdef _DEBUG

	//�����_�����O���ꂽ�C���[�W��\��.
	m_pSwapChain->Present(0, 0);
}

//================================================
//	DirectX������.
//================================================
HRESULT clsMain::InitD3D()
{
	//-------------------------------------
	//	�f�o�C�X�ƃX���b�v�`�F�[���֌W.
	//-------------------------------------

	//�X���b�v�`�F�[���\����.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;	//�o�b�N�o�b�t�@�̐�.
	sd.BufferDesc.Width = WND_W;//�o�b�N�o�b�t�@�̕�.
	sd.BufferDesc.Height = WND_H;//�o�b�N�o�b�t�@�̍���.
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//�t�H�[�}�b�g(32�r�b�g�J���[)
	sd.BufferDesc.RefreshRate.Numerator = 60;
	//���t���b�V�����[�g(����) ��FPS:60
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//���t���b�V�����[�g(���q)
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//�g����(�\����)
	sd.OutputWindow = m_hWnd;	//�E�B���h�E�n���h��.
	sd.SampleDesc.Count = 1;	//�}���`�T���v���̐�.
	sd.SampleDesc.Quality = 0;	//�}���`�T���v���̃N�I���e�B.
	sd.Windowed = TRUE;	//�E�B���h�E���[�h(�t���X�N����FALSE)

	//�쐬�����݂�@�\���x���̗D����w��.
	//	(GPU���T�|�[�g����@�\�Z�b�g�̒�`)
	//	D3D_FEATURE_LEVEL�񋓑̂̔z��.
	//	D3D_FEATURE_LEVEL_10_1:Direct3D 10.1��GPU���x��.
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;//�z��̗v�f��.

	//�f�o�C�X�ƃX���b�v�`�F�[���̍쐬.
	//	�n�[�h�E�F�A(GPU)�f�o�C�X�ō쐬.
	if (FAILED(
		D3D11CreateDeviceAndSwapChain(
			NULL,			//�r�f�I�A�_�v�^�ւ̃|�C���^.
			D3D_DRIVER_TYPE_HARDWARE,//�쐬����f�o�C�X�̎��.
			NULL,			//�\�t�g�E�F�A ���X�^���C�U�[����������DLL�̃n���h��.
			0,				//�L���ɂ��郉���^�C�����C���[.
			&pFeatureLevels,//�쐬�����݂�@�\���x���̏������w�肷��z��ւ̃|�C���^.
			1,				//���̗v�f��.
			D3D11_SDK_VERSION,//SDK�̃o�[�W����.
			&sd,			//�X���b�v�`�F�[���̏������p�����[�^�̃|�C���^.
			&m_pSwapChain,	//(out)�����_�����O�Ɏg�p����X���b�v�`�F�[��.
			&m_pDevice,		//(out)�쐬���ꂽ�f�o�C�X.
			pFeatureLevel,	//�@�\���x���̔z��ɂ���ŏ��̗v�f��\���|�C���^.
			&m_pDeviceContext)))//(out)�f�o�C�X �R���e�L�X�g.
	{
		//WARP�f�o�C�X�̍쐬.
		//	D3D_FEATURE_LEVEL_9_1 �` D3D_FEATURE_LEVEL_10_1
		if (FAILED(
			D3D11CreateDeviceAndSwapChain(
				NULL, D3D_DRIVER_TYPE_WARP,
				NULL, 0, &pFeatureLevels, 1,
				D3D11_SDK_VERSION, &sd, &m_pSwapChain,
				&m_pDevice, pFeatureLevel,
				&m_pDeviceContext)))
		{
			//���t�@�����X�f�o�C�X�̍쐬.
			//	DirectX SDK���C���X�g�[������Ă��Ȃ��Ǝg���Ȃ�.
			if (FAILED(
				D3D11CreateDeviceAndSwapChain(
					NULL, D3D_DRIVER_TYPE_REFERENCE,
					NULL, 0, &pFeatureLevels, 1,
					D3D11_SDK_VERSION, &sd, &m_pSwapChain,
					&m_pDevice, pFeatureLevel,
					&m_pDeviceContext)))
			{
				MessageBox(NULL,
					"�f�o�C�X�ƃX���b�v�`�F�[���̍쐬�Ɏ��s",
					"error(main.cpp)", MB_OK);
				return E_FAIL;
			}
		}
	}

	//�e��e�N�X�`��-�ƁA����ɕt�т���e��r���[(���)���쐬.

	//-----------------------------------------------
	//	�o�b�N�o�b�t�@����:�J���[�o�b�t�@�ݒ�.
	//-----------------------------------------------

	//�o�b�N�o�b�t�@�e�N�X�`��-���擾(���ɂ���̂ō쐬�ł͂Ȃ�)
	ID3D11Texture2D *pBackBuffer_Tex;
	m_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),	//__uuidof:���Ɋ֘A�t�����ꂽGUID���擾.
		//         Texture2D�̗B��̕��Ƃ��Ĉ���.
		(LPVOID*)&pBackBuffer_Tex);	//(out)�o�b�N�o�b�t�@�e�N�X�`��-.

	//���̃e�N�X�`���ɑ΂������_�[�^�[�Q�b�g�r���[(RTV)���쐬.
	m_pDevice->CreateRenderTargetView(
		pBackBuffer_Tex,
		NULL,
		&m_pBackBuffer_TexRTV);//(out)RTV.
	//�o�b�N�o�b�t�@�e�N�X�`��-�����.
	SAFE_RELEASE(pBackBuffer_Tex);

	//-----------------------------------------------
	//	�o�b�N�o�b�t�@����:�f�v�X(�[�x)�X�e���V���֌W.
	//-----------------------------------------------

	//�f�v�X(�[��or�[�x)�X�e���V���r���[�p�̃e�N�X�`��-���쐬.
	D3D11_TEXTURE2D_DESC descDepth;

	descDepth.Width = WND_W;//��.
	descDepth.Height = WND_H;//����.
	descDepth.MipLevels = 1;//�~�b�v�}�b�v���x��:1.
	descDepth.ArraySize = 1;//�z��:1.
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;//32�r�b�g�t�H�[�}�b�g.
	descDepth.SampleDesc.Count = 1;//�}���`�T���v���̐�.
	descDepth.SampleDesc.Quality = 0;//�}���`�T���v���̃N�I���e�B.
	descDepth.Usage = D3D11_USAGE_DEFAULT;//�g�p���@:�f�t�H���g.
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;//�[�x(�X�e���V���Ƃ��Ďg�p)
	descDepth.CPUAccessFlags = 0;//CPU����̓A�N�Z�X���Ȃ�.
	descDepth.MiscFlags = 0;//���̑��̐ݒ�Ȃ�.

	m_pDevice->CreateTexture2D(
		&descDepth,
		NULL,
		&m_pBackBuffer_DSTex);//(out)�f�v�X�X�e���V���p�e�N�X�`��.

	//���̃e�N�X�`���ɑ΂��f�v�X�X�e���V���r���[(DSV)���쐬.
	m_pDevice->CreateDepthStencilView(
		m_pBackBuffer_DSTex,
		NULL,
		&m_pBackBuffer_DSTexDSV);//(out)DSV.

	//�����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[���p�C�v���C���ɃZ�b�g.
	m_pDeviceContext->OMSetRenderTargets(
		1,
		&m_pBackBuffer_TexRTV,
		m_pBackBuffer_DSTexDSV);

	//���\�[�X�N���X.
	m_pResource = clsResource::GetInstance();
	m_pResource->Init(m_hWnd, m_pDevice, m_pDeviceContext);

	Resource->SetDepth(false);

	//�r���[�|�[�g�̐ݒ�.
	m_vp.resize(static_cast<int>(ViewPort::Max));
	switch (m_enVpMode)
	{
	case ViewPort::All:
		//�S��ʗp.
		m_vp[0].Width		= WND_W;		//��.
		m_vp[0].Height		= WND_H;		//����.
		m_vp[0].MinDepth	= 0.0f;		//�ŏ��[�x(��O)
		m_vp[0].MaxDepth	= 1.0f;		//�ő�[�x(��)
		m_vp[0].TopLeftX	= 0.0f;		//����ʒux.
		m_vp[0].TopLeftY	= 0.0f;		//����ʒuy.
		break;
	case ViewPort::RightHalf:
		//�E��ʗp
		m_vp[1].Width		= WND_W/2;	//��.
		m_vp[1].Height		= WND_H;	//����.
		m_vp[1].MinDepth	= 0.0f;		//�ŏ��[�x(��O)
		m_vp[1].MaxDepth	= 1.0f;		//�ő�[�x(��)
		m_vp[1].TopLeftX	= WND_W/2;	//����ʒux.
		m_vp[1].TopLeftY	= 0.0f;		//����ʒuy.
		break;
	case ViewPort::LeftHalf:
		//����ʗp.
		m_vp[2].Width		= WND_W/2;		//��.
		m_vp[2].Height		= WND_H;		//����.
		m_vp[2].MinDepth	= 0.0f;		//�ŏ��[�x(��O)
		m_vp[2].MaxDepth	= 1.0f;		//�ő�[�x(��)
		m_vp[2].TopLeftX	= 0.0f;		//����ʒux.
		m_vp[2].TopLeftY	= 0.0f;		//����ʒuy.
		break;
	default:
		break;
	}
	m_pDeviceContext->RSSetViewports(1, &m_vp[0]);

	//���X�^���C�Y(�ʂ̓h��Ԃ���)�ݒ�.
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.FillMode = D3D11_FILL_SOLID;//�h��Ԃ�(�\���b�h)
	rdc.CullMode = D3D11_CULL_NONE;
	//D3D11_CULL_NONE :�J�����O��؂�(���w�ʂ�`�悷��)
	//D3D11_CULL_BACK :�w�ʂ�`�悵�Ȃ�.
	//D3D11_CULL_FRONT:���ʂ�`�悵�Ȃ�.
	rdc.FrontCounterClockwise = FALSE;
	//�|���S���̕\�������肷��t���O.
	//TRUE :�����Ȃ�O����.�E���Ȃ������.
	//FALSE:���̋t�ɂȂ�.
	rdc.DepthClipEnable = FALSE;	//�����ɂ��ẴN���b�s���O�L��.

	ID3D11RasterizerState* pIr = NULL;
	m_pDevice->CreateRasterizerState(&rdc, &pIr);
	m_pDeviceContext->RSSetState(pIr);
	SAFE_RELEASE(pIr);

	return S_OK;
}

//======================================
//	Direct3D�I������.
//======================================
void clsMain::DestroyD3D()
{
	//�V�[��.
	SAFE_RELEASE(m_smpTitleScene);
	SAFE_RELEASE(m_smpStageScene);
	SAFE_RELEASE(m_smpOverScene);
	SAFE_RELEASE(m_smpClearScene);

	//Direct3D�I�u�W�F�N�g�����.
	SAFE_RELEASE(m_pBackBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBackBuffer_DSTex);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}

//���b�V���Ǎ��֐��i�܂Ƃ߂��j
HRESULT clsMain::MeshRead()
{
	m_bLoadFlg = false;

	m_smpLoadString = make_unique<clsSprite2D>();
	m_smpLoadString->Create(m_pDevice, m_pDeviceContext, "Data\\Load\\LoadBack.png");
	m_smpLoadString->SetPos(D3DXVECTOR3(WND_W / 2 - m_smpLoadString->GetSs().Disp.w / 2,
		WND_H / 2 - m_smpLoadString->GetSs().Disp.h / 2,
		0.0f));
	m_smpLoadString->SetPatarnU(0.0f);
	m_smpLoadString->SetPatarnV(0.0f);
	m_smpLoadString->SetAlpha(1.0f);

	m_smpLoadCircle = make_unique<clsSprite2D>();
	m_smpLoadCircle->Create(m_pDevice, m_pDeviceContext, "Data\\Load\\LoadIcon.png");
	m_smpLoadCircle->SetPos(D3DXVECTOR3(m_smpLoadString->GetPos().x + m_smpLoadString->GetSs().Disp.w,
		m_smpLoadString->GetPos().y,
		0.0f));
	m_smpLoadCircle->SetPatarnU(0.0f);
	m_smpLoadCircle->SetPatarnV(0.0f);
	m_smpLoadCircle->SetAlpha(1.0f);

	//��ʂ̃N���A.
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };//�N���A�F(RGBA��:255�̔䗦�ŏo��)
													 //�J���[�o�b�N�o�b�t�@.
	m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);
	//�f�v�X�X�e���V���o�b�N�o�b�t�@.
	m_pDeviceContext->ClearDepthStencilView(m_pBackBuffer_DSTexDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_smpLoadString->Render();

	m_smpLoadString->Flashing(0.0005f);
	m_smpLoadCircle->Render();

	//�����_�����O���ꂽ�C���[�W��\��.
	m_pSwapChain->Present(0, 0);

	/*��������X�^�e�B�b�N���f��*/
	m_pResource->CreateStaticModel(
		"Data\\teststage\\test_stage_X.X",
		clsResource::enStaticModel_Plane);
	m_pResource->CreateStaticModel(
		"Data\\Player\\Ziki.X",
		clsResource::enStaticModel_Shot);
	m_pResource->CreateStaticModel(
		"Data\\Collision\\Sphere.X",
		clsResource::enStaticModel_Sphere);
	/*�����܂ŃX�^�e�B�b�N���f��*/

	/*��������X�L�����f��*/
	m_pResource->CreateSkinModel(
		"Data\\Player\\Bozu.x",
		clsResource::enSkinModel_Player);
	m_pResource->CreateSkinModel(
		"Data\\EXTINGER\\extinger.X",
		clsResource::enSkinModel_Boss);
	/*�����܂ŃX�L�����f��*/

	Resource->m_smpFile = make_unique<clsFile>();
	Resource->m_smpFile->Init("Data\\Txt\\ScoreRank.csv");
	Resource->m_smpFile->Read();
	for (int i = 0; i < Resource->m_smpFile->GetDataArrayNumMax(); i++)
	{
		Resource->m_viScore.push_back(Resource->m_smpFile->GetDataArray(i));
	}

	m_bTitleFlg = true;
	m_smpTitleScene = make_unique<clsTitleScene>();
	m_smpStageScene = make_unique<clsStageScene>();
	m_smpOverScene = make_unique<clsOverScene>();
	m_smpClearScene = make_unique<clsClearScene>();
	m_smpResultScene = make_unique<clsResultScene>();
	m_pResource->SetModelRender(m_mView, m_mProj, m_vLight, m_Camera.vEye);
	m_pResource->SetSpriteRender(m_hWnd, m_pDevice, m_pDeviceContext);

	m_smpTitleScene->Create();
	m_smpStageScene->Create();
	m_smpOverScene->Create();
	m_smpClearScene->Create();
	m_smpResultScene->Create();

	m_smpTitleScene->Init();
	m_smpStageScene->Init();
	m_smpOverScene->Init();
	m_smpResultScene->Init();
	m_smpClearScene->Init();

	m_smpSeClick = make_unique<clsSound>();
	m_smpSeClick->Open("Data\\Sound\\BGM\\Click.mp3", "ClickSe00", Resource->GetSpriteRenderSet().hWnd);
	m_smpSeClick->SetVolume(100);

	m_smpClearScene->MusicStop();
	m_smpTitleScene->MusicStop();
	m_smpStageScene->MusicStop();
	m_smpResultScene->MusicStop();
	m_smpOverScene->MusicStop();

	return S_OK;
}

//���C�ƃ��b�V���̓����蔻��.
bool clsMain::Intersect(
	clsGameObject* pAttacker,	//��̕���.
	clsCharacter*  pTarget,		//�Ώۂ̕���.
	float* pfDistance,			//(out)����.
	D3DXVECTOR3* pvIntersect)	//(out)�������W.
{
	D3DXMATRIXA16 matRot;	//��]�s��.

	//��]�s����v�Z.
	D3DXMatrixRotationY(&matRot, pAttacker->GetRotationY());

	//���x�N�g����p��.
	D3DXVECTOR3 vecAxisZ;
	//Z���x�N�g�����̂��̂����݂̉�]��Ԃɂ��ϊ�����.
	D3DXVec3TransformCoord(
		&vecAxisZ, &pAttacker->m_vAxis, &matRot);

	D3DXVECTOR3 vecStart, vecEnd, vecDistance;
	//���C�̊J�n�I���ʒu��Attacker�ƍ��킹��.
	vecStart = vecEnd = pAttacker->m_vRay;
	//Attacker�̍��W�ɉ�]���W����������.
	vecEnd += vecAxisZ * 1.0f;

	//�Ώۂ������Ă��镨�̂ł��A�Ώۂ�world�s��́A
	//�t�s���p����΁A���������C��������.
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(
		&matWorld,
		pTarget->GetPositionX(),
		pTarget->GetPositionY(),
		pTarget->GetPositionZ());

	//�t�s������߂�.
	D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	D3DXVec3TransformCoord(
		&vecStart, &vecStart, &matWorld);
	D3DXVec3TransformCoord(
		&vecEnd, &vecEnd, &matWorld);

	//���������߂�.
	vecDistance = vecEnd - vecStart;

	BOOL bHit = false;	//�����t���O.

	DWORD dwIndex = 0;		//�C���f�b�N�X�ԍ�.
	D3DXVECTOR3 vVertex[3];	//���_���W.
	FLOAT U = 0, V = 0;			//(out)�d�S�q�b�g���W.

	//���b�V���ƃ��C�̌����𒲂ׂ�.
	D3DXIntersect(
		pTarget->GetMesh(),	//�Ώۃ��b�V��.
		&vecStart,			//�J�n�ʒu.
		&vecDistance,		//���C�̋���.
		&bHit,				//(out)���茋��.
		&dwIndex,	//(out)bHit��True���A���C�̎n�_��.
		//�ł��߂��̖ʂ̃C���f�b�N�X�l�ւ̃|�C���^.
		&U, &V,				//(out)�d�S�q�b�g���W.
		pfDistance,			//�^�[�Q�b�g�Ƃ̋���.
		NULL, NULL);
	if (bHit) {
		//���������Ƃ�.
		FindVerticesOnPoly(
			pTarget->GetMeshForRay(), dwIndex, vVertex);
		//�d�S���W��������_���Z�o.
		//���[�J����_p�́Av0 + U*(v1-v0) + V*(v2-v0)�ŋ��܂�.
		*pvIntersect =
			vVertex[0]
			+ U * (vVertex[1] - vVertex[0])
			+ V * (vVertex[2] - vVertex[0]);

		return true;//�������Ă���.
	}
	return false;//�O��Ă���.
}

//�����ʒu�̃|���S���̒��_��������.
//�����_���W�̓��[�J�����W.
HRESULT clsMain::FindVerticesOnPoly(
	LPD3DXMESH pTarget, DWORD dwPolyIndex,
	D3DXVECTOR3* pVecVertices)
{
	//���_���Ƃ̃o�C�g�����擾.
	DWORD dwStride = pTarget->GetNumBytesPerVertex();
	//���_�����擾.
	DWORD dwVertexAmt = pTarget->GetNumVertices();
	//�ʐ����擾.
	DWORD dwPolyAmt = pTarget->GetNumFaces();

	WORD* pwPoly = NULL;

	//�C���f�b�N�X�o�b�t�@�����b�N(�Ǎ����[�h)
	pTarget->LockIndexBuffer(
		D3DLOCK_READONLY, (VOID**)&pwPoly);
	BYTE* pbVertices = NULL;//���_(�o�C�g�^)
	FLOAT* pfVertices = NULL;//���_(float�^)
	LPDIRECT3DVERTEXBUFFER9 VB = NULL;//���_�o�b�t�@.
	pTarget->GetVertexBuffer(&VB);//���_���̎擾.

	//���_�o�b�t�@�̃��b�N.
	if (SUCCEEDED(
		VB->Lock(0, 0, (VOID**)&pbVertices, 0)))
	{
		//�|���S���̒��_�̂P�ڂ��擾.
		pfVertices
			= (FLOAT*)&pbVertices[dwStride*pwPoly[dwPolyIndex * 3]];
		pVecVertices[0].x = pfVertices[0];
		pVecVertices[0].y = pfVertices[1];
		pVecVertices[0].z = pfVertices[2];

		//�|���S���̒��_�̂Q�ڂ��擾.
		pfVertices
			= (FLOAT*)&pbVertices[dwStride*pwPoly[dwPolyIndex * 3 + 1]];
		pVecVertices[1].x = pfVertices[0];
		pVecVertices[1].y = pfVertices[1];
		pVecVertices[1].z = pfVertices[2];

		//�|���S���̒��_�̂R�ڂ��擾.
		pfVertices
			= (FLOAT*)&pbVertices[dwStride*pwPoly[dwPolyIndex * 3 + 2]];
		pVecVertices[2].x = pfVertices[0];
		pVecVertices[2].y = pfVertices[1];
		pVecVertices[2].z = pfVertices[2];

		pTarget->UnlockIndexBuffer();	//���b�N����.
		VB->Unlock();	//���b�N����.
	}
	VB->Release();

	return S_OK;
}

//�ǂ̓����蔻��֘A.
void clsMain::WallJudge(clsGameObject* pAttacker, clsCharacter* pWall)
{
	FLOAT		fDistance[4];	//����.
	D3DXVECTOR3	vIntersect[4];	//��_���W.
	float		fDis, fYaw;		//�����Ɖ�].

	pAttacker->m_vRay = pAttacker->GetPosition();
	pAttacker->m_vRay.y = 1.0f;	//���C��y�ʒu.

	//���C�̌����ɂ�蓖����ǂ܂ł̋��������߂�.
	//���x�N�g���O.
	pAttacker->m_vAxis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	Intersect(pAttacker, pWall, &fDistance[0], &vIntersect[0]);
	//���x�N�g�����.
	pAttacker->m_vAxis = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	Intersect(pAttacker, pWall, &fDistance[1], &vIntersect[1]);
	//���x�N�g���E.
	pAttacker->m_vAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	Intersect(pAttacker, pWall, &fDistance[2], &vIntersect[2]);
	//���x�N�g����.
	pAttacker->m_vAxis = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	Intersect(pAttacker, pWall, &fDistance[3], &vIntersect[3]);

	fYaw = fabs(pAttacker->GetRotationY());//fabs:��Βl(float��)
	dirOverGuard(&fYaw);	//0�`2�΂̊ԂɎ��߂�.

	//�O���ǂɐڋ�.
	fDis = fDistance[0];
	if (fDis < WSPACE && fDis > 0.01f) {
		if (pAttacker->GetRotationY() < 0.0f) {
			//���v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionX(-WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
		}
		else {
			//�����v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
		}
	}

	//��낪�ǂɐڋ�.
	fDis = fDistance[1];
	if (fDis < WSPACE && fDis > 0.01f) {
		if (pAttacker->GetRotationY() < 0.0f) {
			//���v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionX(-WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
		}
		else {
			//�����v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
		}
	}

	//�E���ǂɐڋ�.
	fDis = fDistance[2];
	if (fDis < WSPACE && fDis > 0.01f) {
		if (pAttacker->GetRotationY() < 0.0f) {
			//���v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
		}
		else {
			//�����v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionX(-WSPACE - fDis);
			}
		}
	}

	//�����ǂɐڋ�.
	fDis = fDistance[3];
	if (fDis < WSPACE && fDis > 0.01f) {
		if (pAttacker->GetRotationY() < 0.0f) {
			//���v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionX(-WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
		}
		else {
			//�����v���.
			if (fYaw >= 0.785f && fYaw < 2.355f) {		//�E����.
				pAttacker->SetPositionZ(-WSPACE - fDis);
			}
			else if (fYaw >= 2.355f && fYaw < 3.925f) {	//�O����.
				pAttacker->SetPositionX(-WSPACE - fDis);
			}
			else if (fYaw >= 3.925f && fYaw < 5.495f) {	//������.
				pAttacker->SetPositionZ(WSPACE - fDis);
			}
			else {										//������.
				pAttacker->SetPositionX(WSPACE - fDis);
			}
		}
	}
}

//��]�l����.
void clsMain::dirOverGuard(float* fYaw)
{
	if (*fYaw > D3DX_PI * 2.0f) {
		//�P���ȏサ�Ă���.
		*fYaw -= static_cast<float>(D3DX_PI * 2.0f);	//2��(360��)��������.
	}

	//�ċA�֐�.
	if (*fYaw > D3DX_PI * 2.0f) {
		dirOverGuard(fYaw);
	}
}

//�J�����֐�.
void clsMain::Camera()
{
	//�J����(���_)�ʒu.
	m_Camera.vEye = D3DXVECTOR3(0.0f, 5.0f, 0.0f);

	//���x�N�g����p��.
	D3DXVECTOR3 vecAxisZ(0.0f, 0.0f, 15.0f);
	switch (m_enScene)
	{
	case Title:
		break;
	case Stage:
		//===============================================
		//�J�����Ǐ]���� ��������.
		//===============================================
		//�J�����ʒu(���@�̔w������)�̐ݒ�.
		//�����ʒu.
		m_Camera.vLook
			= m_smpStageScene->m_smpPlayer->GetPosition();//���@�̈ʒu���R�s�[.
		m_Camera.fYaw = m_smpStageScene->m_smpPlayer->GetRotationY();//��]�l���R�s�[.

		//Y����]�s��̍쐬.
		D3DXMatrixRotationY(
			&m_Camera.mRot, m_Camera.fYaw);

		//Z���x�N�g�����̂��̂���]��Ԃɂ��ϊ�����.
		D3DXVec3TransformCoord(
			&vecAxisZ, &vecAxisZ, &m_Camera.mRot);

		m_Camera.vEye -= vecAxisZ/* * 4.0f*/;	//���@�̔w����.
		m_Camera.vLook += vecAxisZ/* * 2.0f*/;	//���@�̑O��.

		//===============================================
		//�J�����Ǐ]���� �����܂�.
		//===============================================
		break;
	case Over:
		break;
	case Clear:
		break;
	}
	//�r���[(�J����)�ϊ�.
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);//����ʒu.
	D3DXMatrixLookAtLH(
		&m_mView,	//(out)�r���[�v�Z����.
		&m_Camera.vEye, &m_Camera.vLook, &vUpVec);
}

//�v���W�F�N�V�����֐�.
void clsMain::Proj()
{
	//�v���W�F�N�V����(�ˉe�s��)�ϊ�.
	D3DXMatrixPerspectiveFovLH(
		&m_mProj,	//(out)�v���W�F�N�V�����v�Z����.
		D3DX_PI / 6.0, //y�����̎���(���W�A���w��)���l��傫�������王�삪�����Ȃ�.
		(FLOAT)WND_W / (FLOAT)WND_H,//�A�X�y�N�g��(��������)
		0.1f,		//�߂��r���[���ʂ�z�l.
		100.0f);	//�����r���[���ʂ�z�l.
}
void clsMain::SceneChange()
{
	if (m_enOldScene != m_enScene)
	{
		switch (m_enScene)
		{
		case Title:
			m_smpClearScene->MusicStop();
			m_smpOverScene->MusicStop();
			m_smpResultScene->MusicStop();
			m_smpTitleScene->Init();
			m_enOldScene = m_enScene;
			break;
		case Stage:
			m_smpTitleScene->MusicStop();
			m_smpStageScene->Init();
			m_enOldScene = m_enScene;
			break;
		case Over:
			m_smpStageScene->MusicStop();
			m_smpOverScene->Init();
			m_enOldScene = m_enScene;
			break;
		case Clear:
			m_smpStageScene->MusicStop();
			m_smpOverScene->MusicStop();
			m_smpClearScene->Init();
			m_enOldScene = m_enScene;
			break;
		case Result:
			m_smpTitleScene->MusicStop();
			m_smpResultScene->Init();
			m_enOldScene = m_enScene;
			break;
		}
	}
}