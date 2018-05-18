#include "Sprite2d.h"

//============================================================
//	�萔.
//============================================================

//�V�F�[�_�t�@�C����(�f�B���N�g�����܂�)(\\).
const string SHADER_NAME = "Shader\\Sprite2D.hlsl";

clsSprite2D::clsSprite2D()
	: m_vPos(0.0f, 0.0f, 0.0f)
	, m_AnimCount(0)
	, m_bDispFlg(false)
	, m_fAlpha(1.0f)
	, m_pTexture(NULL)
	, m_pVertexShader(NULL)
	, m_pVertexLayout(NULL)
	, m_pPixelShader(NULL)
	, m_pConstantBuffer(NULL)
	, m_pVertexBuffer(NULL)
	, m_pSampleLinear(NULL)
{
	//	ZeroMemory( this, sizeof( clsSprite2D ) );
	//	m_fAlpha = 1.0f;
	//	m_fPatternNo.x = 0.0f;
	//	m_fPatternNo.y = 0.0f;

	m_SState.Base.h = 0.0f;
	m_SState.Base.w = 0.0f;
	m_SState.Disp.h = 0.0f;
	m_SState.Disp.w = 0.0f;
	m_SState.Stride.h = 0.0f;
	m_SState.Stride.w = 0.0f;

	m_fPatternNo.x = 0.0f;
	m_fPatternNo.y = 0.0f;
}

clsSprite2D::~clsSprite2D()
{
	// �����[�X�֐����Ƃ肠�����A�f�X�g���N�^�ɓ˂�����.
	Release();
}

//============================================================
//	������.
//============================================================
HRESULT clsSprite2D::Create(ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPSTR fileName)
{
	m_sFileName = fileName;
	m_pDevice11 = pDevice11;
	m_pDeviceContext11 = pContext11;

	SetPos(-WND_W, -WND_H);

	m_BlackMode = Idle;

	// ���̍\���̂̒��ɉ摜�̏�񂪓���
	D3DXIMAGE_INFO info;

	// filename�͂��̂܂�܁A�Ώۂ̉摜�t�@�C����
	D3DXGetImageInfoFromFile(m_sFileName.c_str(), &info);

	SetSs(static_cast<float>(info.Width), static_cast<float>(info.Height));

	if (FAILED(InitShader())) {
		return E_FAIL;
	}
	if (FAILED(InitModel(m_SState))) {
		return E_FAIL;
	}
	if (FAILED(CreateTexture((LPSTR)m_sFileName.c_str(), &m_pTexture))) {
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	HLSL�t�@�C����ǂݍ��݃V�F�[�_���쐬����.
//	HLSL:HIGE-LEVEL-SHADER-LANGUAGE.
//============================================================
HRESULT clsSprite2D::InitShader()
{
	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrors = NULL;

	UINT uCompileFlag = 0;

#ifdef _DEBUG
	uCompileFlag
		= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSL����o�[�e�b�N�X�V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME.c_str(),	//�V�F�[�_�t�@�C����(HLSL�t�@�C��).
			NULL,			//�}�N����`�̔z��ւ̃|�C���^(���g�p).
			NULL,			//�C���N���[�h�t�@�C���������C���^�[�t�F�[�X�ւ̃|�C���^(���g�p).
			"VS_Main",			//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"vs_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��).
			uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
			0,				//�G�t�F�N�g�R���p�C���t���O(���g�p).
			NULL,			//�X���b�h�|���v�C���^�[�t�F�[�X�ւ̃|�C���^(���g�p).
			&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,		//�G���[�ƌx���ꗗ���i�[���郁�����ւ̃|�C���^.
			NULL)))		//�߂�l�ւ̃|�C���^(���g�p).
	{
		MessageBox(NULL, "hlsl(vs)�ǂݍ��ݎ��s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�o�[�e�b�N�X�V�F�[�_�v���쐬.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pVertexShader)))//(out)�o�[�e�b�N�X�V�F�[�_.
	{
		MessageBox(NULL, "vs�쐬���s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						//�ʒu.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	//DXGI�̃t�H�[�}�b�g(32bit float�^*3).
			0,
			0,								//�f�[�^�̊J�n�ʒu.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",					//�e�N�X�`���ʒu.
			0,
			DXGI_FORMAT_R32G32_FLOAT,	//DXGI�̃t�H�[�}�b�g(32bit float�^*2).
			0,
			12,							//�e�N�X�`���f�[�^�̊J�n�ʒu(���_�f�[�^��float�^*3=12bite�Ȃ̂�+12).
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//���_�C���v�b�g���C�A�E�g�̔z��v�f�����Z�o.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);//.

	//���_�C���v�b�g���C�A�E�g�̍쐬.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))//(out)���_�C���v�b�g���C�A�E�g.
	{
		MessageBox(NULL, "���_�C���v�b�g���C�A�E�g�쐬���s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSL����s�N�Z���V�F�[�_�̃u���u���쐬.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME.c_str(),	//�V�F�[�_�t�@�C����(HLSL�t�@�C��).
			NULL,
			NULL,
			"PS_Main",			//�V�F�[�_�G���g���[�|�C���g�֐��̖��O.
			"ps_5_0",		//�V�F�[�_�̃��f�����w�肷�镶����(�v���t�@�C��).
			uCompileFlag,	//�V�F�[�_�R���p�C���t���O.
			0,
			NULL,
			&pCompiledShader,//�u���u���i�[���郁�����ւ̃|�C���^.
			&pErrors,
			NULL)))
	{
		MessageBox(NULL, "hlsl(ps)�ǂݍ��ݎ��s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//��L�ō쐬�����u���u����u�s�N�Z���V�F�[�_�v���쐬.
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pPixelShader)))//(out)�s�N�Z���V�F�[�_.
	{
		MessageBox(NULL, "ps�쐬���s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);//�u���u���.

	//�R���X�^���g�o�b�t�@(�萔)�o�b�t�@�쐬�@�V�F�[�_�ɓ���̐��l�𑗂�o�b�t�@.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;;			//�R���X�g�o�b�t�@���w��.
	cb.ByteWidth = sizeof(SPRITE2D_CONSTANT_BUFFER);//�R���X�^���g�o�b�t�@�̃T�C�Y.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				//�������݂ŃA�N�Z�X.
	cb.MiscFlags = 0;					//���̑��̃t���O(���g�p).
	cb.StructureByteStride = 0;			//�\���̂̃T�C�Y(���g�p)
	cb.Usage = D3D11_USAGE_DYNAMIC;	//�g�p���@:���ڏ�������.

	//�R���X�^���g�o�b�t�@�쐬.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			NULL,
			&m_pConstantBuffer)))
	{
		MessageBox(NULL, "�R���X�^���g�o�b�t�@�쐬���s", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	���f���쐬.
//============================================================
HRESULT clsSprite2D::InitModel(SPRITE_STATE ss)
{
	float fW = ss.Disp.w;	//�\���X�v���C�g��.
	float fH = ss.Disp.h;	//�\���X�v���C�g����.
	float fU = (ss.Base.w / ss.Stride.w) / ss.Base.w;	//��R�}������̕�.
	float fV = (ss.Base.h / ss.Stride.h) / ss.Base.h;	//��R�}������̍���.

	//�|��(�l�p�`)�̒��_���쐬.
	Sprite2DVertex vertices[] =
	{
		//���_���W(x,y,z).					//UV���W( u, v ).
		D3DXVECTOR3(0.0f, fH, 0.0f), D3DXVECTOR2(0.0f, fV),	//���_1(����).
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f),	//���_2(����).
		D3DXVECTOR3(fW, fH, 0.0f), D3DXVECTOR2(fU, fV),	//���_3(�E��).
		D3DXVECTOR3(fW, 0.0f, 0.0f), D3DXVECTOR2(fU, 0.0f)	//���_4(�E��).
	};
	//�ő�v�f�����Z�o����.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	//�o�b�t�@�\����.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;		//�g�p�@(�f�t�H���g).
	bd.ByteWidth = sizeof(Sprite2DVertex) * uVerMax;//���_�T�C�Y(���_*4).
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//���_�o�b�t�@�Ƃ��Ĉ���.
	bd.CPUAccessFlags = 0;						//CPU����̓A�N�Z�X���Ȃ�.
	bd.MiscFlags = 0;						//���̑��̃t���O(���g�p).
	bd.StructureByteStride = 0;					//�\���̃T�C�Y(���g�p).

	//�T�u���\�[�X�f�[�^�\����.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	//�|���̒��_���Z�b�g.

	//���_�o�b�t�@�̍쐬.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "���_�o�b�t�@�쐬���s", "clsSprite2D::InitModel", MB_OK);
		return E_FAIL;
	}

	//���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(Sprite2DVertex);//�f�[�^�Ԋu.
	UINT offset = 0;//�J�n�ʒu.
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1,
		&m_pVertexBuffer, &stride, &offset);

	//�e�N�X�`���p�̃T���v���[�\����.
	D3D11_SAMPLER_DESC	SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	//���j�A�t�B���^�[(���^���).
	//	POINT:���������e��.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//���b�s���O���[�h.
	//	WRAP:�J��Ԃ�.
	//	MIRROR	:���]�J��Ԃ�.
	//	CLAMP	:�[�̖͗l���������΂�.
	//	BORDER	;�ʓr���E�F�����߂�.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//�T���v���[�쐬.
	if (FAILED(
		m_pDevice11->CreateSamplerState(
			&SamDesc, &m_pSampleLinear)))//(out)�T���v���[.
	{
		MessageBox(NULL, "�T���v���쐬���s", "clsSprite2D::InitModel", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	�e�N�X�`���쐬.
//============================================================
HRESULT clsSprite2D::CreateTexture(LPSTR fileName,
	ID3D11ShaderResourceView** pTex)
{
	//�e�N�X�`���쐬.
	if (FAILED(
		D3DX11CreateShaderResourceViewFromFile(
			m_pDevice11,		//���\�[�X���g�p����f�o�C�X�ւ̃|�C���^.
			fileName,	//�t�@�C����(�p�X���K�v).
			NULL, NULL,
			pTex,	//(out)�e�N�X�`��.
			NULL)))
	{
		ERR_MSG(fileName, "clsSprite2D::CreateTexture");
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//�`��(�����_�����O)(��DX9MESH����Main����2���݂���̂Œ���).
//============================================================
void clsSprite2D::Render()
{
	//���[���h�s��.
	D3DXMATRIX	mWorld;

	//���[���h�ϊ�(�\���ʒu��ݒ肷��).
	D3DXMatrixIdentity(&mWorld);	//������:�P�ʍs��쐬.

	//	D3DXMatrixScaling();

	//���s�ړ�.
	D3DXMatrixTranslation(&mWorld,
		m_vPos.x, m_vPos.y, m_vPos.z);

	//�g�p����V�F�[�_�̓o�^.
	m_pDeviceContext11->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(m_pPixelShader, NULL, 0);

	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE pData;
	SPRITE2D_CONSTANT_BUFFER cd;	//�R���X�^���g�o�b�t�@.
	//�o�b�t�@���̃f�[�^�̏������J�n����map.
	if (SUCCEEDED(
		m_pDeviceContext11->Map(
			m_pConstantBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h�s���n��.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);	//�s���]�u����.
		//�s��̌v�Z���@��DirectX��GPU�ňقȂ邽��.
		cd.mW = m;

		//�r���[�|�[�g�̕��A������n��.
		cd.fViewPortWidth = WND_W;
		cd.fViewPortHeight = WND_H;

		//�A���t�@�l��n��.
		cd.fAlpha = m_fAlpha;

		//UV���W.
		//1�R�A������̊����ɃR�}�ԍ����|���Ĉʒu��ݒ肷��.
		WHSIZE_FLOAT wf;
		wf.w = (m_SState.Base.w / m_SState.Stride.w) / m_SState.Base.w;
		wf.h = (m_SState.Base.h / m_SState.Stride.h) / m_SState.Base.h;
		cd.vUV.x = wf.w * m_fPatternNo.x;
		cd.vUV.y = wf.h * m_fPatternNo.y;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//���̃R���X�^���g�o�b�t�@���ǂ̃V�F�[�_�Ŏg����?.
	m_pDeviceContext11->VSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);

	//���_�o�b�t�@���Z�b�g.
	UINT stride = sizeof(Sprite2DVertex);
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &stride, &offset);

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pDeviceContext11->IASetInputLayout(m_pVertexLayout);

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�e�N�X�`�����V�F�[�_�ɓn��.
	m_pDeviceContext11->PSSetSamplers(
		0, 1, &m_pSampleLinear);	//�T���v���[���Z�b�g.
	m_pDeviceContext11->PSSetShaderResources(
		0, 1, &m_pTexture);		//�e�N�X�`�����V�F�[�_�ɓn��.

	//�A���t�@�u�����h�p�u�����h�X�e�[�g�쐬&�ݒ�.
	SetBlend(true);

	//�v���~�e�B�u�������_�����O.
	m_pDeviceContext11->Draw(4, 0);

	//�A���t�@�u�����h�𖳌��ɂ���.
	SetBlend(false);
}

void clsSprite2D::UpDateSpriteSs()
{
	SPRITE_STATE ss = m_SState;
	float fW = ss.Disp.w;	//�\���X�v���C�g��.
	float fH = ss.Disp.h;	//�\���X�v���C�g����.
	float fU = (ss.Base.w / ss.Stride.w) / ss.Base.w;	//��R�}������̕�.
	float fV = (ss.Base.h / ss.Stride.h) / ss.Base.h;	//��R�}������̍���.

	//�|��(�l�p�`)�̒��_���쐬.
	Sprite2DVertex vertices[] =
	{
		//���_���W(x,y,z).					//UV���W( u, v ).
		D3DXVECTOR3(0.0f, fH, 0.0f), D3DXVECTOR2(0.0f, fV),	//���_1(����).
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f),	//���_2(����).
		D3DXVECTOR3(fW, fH, 0.0f), D3DXVECTOR2(fU, fV),	//���_3(�E��).
		D3DXVECTOR3(fW, 0.0f, 0.0f), D3DXVECTOR2(fU, 0.0f)	//���_4(�E��).
	};
	//�ő�v�f�����Z�o����.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	//�o�b�t�@�\����.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;		//�g�p�@(�f�t�H���g).
	bd.ByteWidth = sizeof(Sprite2DVertex) * uVerMax;//���_�T�C�Y(���_*4).
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//���_�o�b�t�@�Ƃ��Ĉ���.
	bd.CPUAccessFlags = 0;						//CPU����̓A�N�Z�X���Ȃ�.
	bd.MiscFlags = 0;						//���̑��̃t���O(���g�p).
	bd.StructureByteStride = 0;					//�\���̃T�C�Y(���g�p).

	//�T�u���\�[�X�f�[�^�\����.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	//�|���̒��_���Z�b�g.

	//���_�o�b�t�@�̍쐬.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "���_�o�b�t�@�쐬���s,", "clsSprite2D::InitModel", MB_OK);
	}
}

void clsSprite2D::Flashing(float ChaAmo, float AlphaMax, float AlphaMin)
{
	if (m_fAlpha > AlphaMax)
	{
		m_bFlashing = true;
	}
	if (m_fAlpha < AlphaMin)
	{
		m_bFlashing = false;
	}
	if (m_bFlashing)
	{
		m_fAlpha -= ChaAmo;
	}
	else
	{
		m_fAlpha += ChaAmo;
	}
}

bool clsSprite2D::Release()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSampleLinear);

	SAFE_RELEASE(m_pPixelShader);

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pVertexLayout);

	SAFE_RELEASE(m_pVertexBuffer);

	SAFE_RELEASE(m_pConstantBuffer);

	m_pDeviceContext11 = NULL;
	m_pDevice11 = NULL;

	return true;
}