#include "DebugText.h"

//�V�F�[�_�t�@�C����(�p�X���܂�).
const char SHADER_NAME[] = "Shader\\DebugText.hlsl";

//�R���X�g���N�^.
clsDebugText::clsDebugText()
{
	ZeroMemory(this, sizeof(clsDebugText));//������.

	m_fScale = 1.0f;
}

//�f�X�g���N�^.
clsDebugText::~clsDebugText()
{
}

//�������֐�.
HRESULT clsDebugText::Init(
	ID3D11DeviceContext* pContext,
	DWORD dwWidth, DWORD dwHeight,
	float fSize, D3DXVECTOR4 vColor)
{
	m_fAlpha = vColor.w;
	m_vColor = vColor;
	m_fScale = fSize / 25.0f;

	//95�������J��Ԃ�.
	for (int i = 0; i < 95; i++) {
		m_fKerning[i] = 10.0f;
	}
	//�f�o�C�X�R���e�L�X�g���R�s�[.
	m_pDeviceContext11 = pContext;
	m_pDeviceContext11->GetDevice(&m_pDevice11);

	//window�T�C�Y.
	m_dwWindowWidth = dwWidth;
	m_dwWindowHeight = dwHeight;

	//�t�H���g���ɃN�A�b�h(��`)�쐬
	float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
	int cnt = 0;
	//2�d���[�v��1�������w�肷��.
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < 10; i++)
		{
			left = k * WDIMENSION;
			top = i * DIMENSION;
			right = left + m_fKerning[cnt];
			bottom = top + DIMENSION;

			left /= TEX_DIMENSION;
			top /= TEX_DIMENSION;
			right /= TEX_DIMENSION;
			bottom /= TEX_DIMENSION;

			TextVertex vertices[] =
			{
				D3DXVECTOR3(0.0f, DIMENSION, 0.0f), D3DXVECTOR2(left, bottom),	//���_1.
				D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(left, top),		//���_2.
				D3DXVECTOR3(WDIMENSION, DIMENSION, 0.0f), D3DXVECTOR2(right, bottom),	//���_3.
				D3DXVECTOR3(WDIMENSION, 0.0, 0.0f), D3DXVECTOR2(right, top)		//���_4.
			};
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(TextVertex) * 4;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;
			if (FAILED(m_pDevice11->CreateBuffer(
				&bd, &InitData, &m_pVertexBuffer[cnt])))
			{
				MessageBox(NULL,
					"���_�o�b�t�@�쐬���s(DebugText:Init)",
					"error", MB_OK);
				return E_FAIL;
			}
			cnt++;
		}
	}

	//�e�N�X�`���p�T���v��-�쐬.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	//�e�N�X�`���t�B���^�Ɠ\��t�����̎w��.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if (FAILED(m_pDevice11->CreateSamplerState(
		&SamDesc, &m_pSampleLinear)))
	{
		MessageBox(NULL,
			"�T���v���쐬���s(DebugText:Init)",
			"", MB_OK);
		return E_FAIL;
	}

	//�t�H���g�̃e�N�X�`���쐬.
	if (FAILED(
		D3DX11CreateShaderResourceViewFromFile(
			m_pDevice11,
			"Data\\DebugText\\ascii.png",
			NULL, NULL,
			&m_pAsciiTexture, NULL)))
	{
		MessageBox(NULL,
			"�t�H���g�e�N�X�`���쐬���s(DebugText:Init)",
			"error", MB_OK);
		return E_FAIL;
	}

	//=====================================
	//	hlsl�t�@�C���Ǎ�.
	//=====================================
	ID3DBlob *pCompileShader = NULL;
	ID3DBlob *pErrors = NULL;
	if (FAILED(D3DX11CompileFromFile(
		SHADER_NAME, NULL, NULL,
		"VS", "vs_5_0", 0, 0,
		NULL, &pCompileShader,
		&pErrors, NULL)))
	{
		MessageBox(NULL,
			"hlsl�Ǎ����s(VS)",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreateVertexShader(
		pCompileShader->GetBufferPointer(),
		pCompileShader->GetBufferSize(),
		NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE(pCompileShader);
		MessageBox(NULL,
			"�o�[�e�b�N�X�V�F�[�_�쐬���s",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	//���_�C���v�b�g���C�A�E�g���`.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//���_�C���v�b�g���C�A�E�g�쐬.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout, numElements,
			pCompileShader->GetBufferPointer(),
			pCompileShader->GetBufferSize(),
			&m_pVertexLayout)))
	{
		MessageBox(NULL,
			"���_�C���v�b�g���C�A�E�g�쐬���s",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	//�s�N�Z���V�F�[�_�쐬.
	if (FAILED(D3DX11CompileFromFile(
		SHADER_NAME, NULL, NULL,
		"PS", "ps_5_0", 0, 0,
		NULL, &pCompileShader,
		&pErrors, NULL)))
	{
		MessageBox(NULL,
			"hlsl�Ǎ����s(PS)",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice11->CreatePixelShader(
		pCompileShader->GetBufferPointer(),
		pCompileShader->GetBufferSize(),
		NULL, &m_pPixelShader)))
	{
		SAFE_RELEASE(pCompileShader);
		MessageBox(NULL,
			"�s�N�Z���V�F�[�_�쐬���s",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompileShader);

	//�R���X�^���g�o�b�t�@�쐬.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(TEXT_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDevice11->CreateBuffer(
		&cb, NULL, &m_pConstantBuffer)))
	{
		MessageBox(NULL,
			"�R���X�^���g�o�b�t�@�쐬",
			"DebugText:Init", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//����(�A���t�@�u�����h)�ݒ�̐؂�ւ�.
void clsDebugText::SetBlend(bool flg)
{
	//�A���t�@�u�����h�p�u�����h�X�e�[�g�\����.
	//png�t�@�C�����ɃA���t�@��񂪂���̂ŁA
	//���߂���悤�Ƀu�����h�X�e�[�g��ݒ肷��.
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));//������.

	blendDesc.IndependentBlendEnable
		= false;//false:RenderTarget[0]�̃����o�[�݂̂��g�p����.
	//true :RenderTarget[0�`7]���g�p�ł���.
	//      (�����_�[�^�[�Q�b�g���ɓƗ������u�����h����)
	blendDesc.AlphaToCoverageEnable
		= false;//true :�A���t�@�g�D�J�o���b�W���g�p����.
	blendDesc.RenderTarget[0].BlendEnable
		= flg;	//true :�A���t�@�u�����h���g�p����.
	blendDesc.RenderTarget[0].SrcBlend	//���f�ނɑ΂���ݒ�.
		= D3D11_BLEND_SRC_ALPHA;		//	�A���t�@�u�����h���w��.
	blendDesc.RenderTarget[0].DestBlend	//�d�˂�f�ނɑ΂���ݒ�.
		= D3D11_BLEND_INV_SRC_ALPHA;	//	�A���t�@�u�����h�̔��]���w��.

	blendDesc.RenderTarget[0].BlendOp	//�u�����h�I�v�V����.
		= D3D11_BLEND_OP_ADD;			//	ADD:���Z����.

	blendDesc.RenderTarget[0].SrcBlendAlpha	//���f�ނ̃A���t�@�ɑ΂���ݒ�.
		= D3D11_BLEND_ONE;					//	���̂܂܎g�p.
	blendDesc.RenderTarget[0].DestBlendAlpha//�d�˂�f�ނ̃A���t�@�ɑ΂���ݒ�.
		= D3D11_BLEND_ZERO;					//	�������Ȃ�.

	blendDesc.RenderTarget[0].BlendOpAlpha	//�A���t�@�̃u�����h�I�v�V����.
		= D3D11_BLEND_OP_ADD;				//	ADD:���Z����.

	blendDesc.RenderTarget[0].RenderTargetWriteMask	//�s�N�Z�����̏������݃}�X�N.
		= D3D11_COLOR_WRITE_ENABLE_ALL;				//	�S�Ă̐���(RGBA)�ւ̃f�[�^�̊i�[��������.

	//�u�����h�X�e�[�g�쐬.
	if (FAILED(
		m_pDevice11->CreateBlendState(
			&blendDesc, &m_pBlendState)))
	{
		MessageBox(NULL, "�u�����h�X�e�[�g�쐬���s", "�G���[", MB_OK);
	}

	//�u�����h�X�e�[�g�̐ݒ�.
	UINT mask = 0xffffffff;	//�}�X�N�l.
	m_pDeviceContext11->OMSetBlendState(
		m_pBlendState, NULL, mask);
}

//�����_�����O�֐�.
void clsDebugText::Render(char* text, int x, int y)
{
	//�r���[�g�����X�t�H�[��.
	D3DXVECTOR3 vEye(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3 vLook(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(
		&m_mView, &vEye, &vLook, &vUp);

#if 0
	//�v���W�F�N�V�����g�����X�t�H�[��(�Œ�)
	D3DMATRIX mOtho = {
		2.0f / (float)(m_dwWindowWidth), 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / (float)(m_dwWindowHeight), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
#else
	D3DXMATRIX mOtho;
	D3DXMatrixIdentity(&mOtho);//�P�ʍs��.
	mOtho._11 = 2.0f / (float)(m_dwWindowWidth);
	mOtho._22 = -2.0f / (float)(m_dwWindowHeight);
	mOtho._41 = -1.0f;
	mOtho._42 = 1.0f;
#endif
	m_mProj = mOtho;

	//�v���~�e�B�u�E�g�|���W�[.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pDeviceContext11->IASetInputLayout(
		m_pVertexLayout);

	//�g�p����V�F�[�_�̓o�^.
	m_pDeviceContext11->VSSetShader(
		m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(
		m_pPixelShader, NULL, 0);

	//���̃R���X�^���g�o�b�t�@���g���V�F�[�_�̓o�^.
	m_pDeviceContext11->VSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);

	//�e�N�X�`�����V�F�[�_�ɓn��.
	m_pDeviceContext11->PSSetSamplers(
		0, 1, &m_pSampleLinear);
	m_pDeviceContext11->PSSetShaderResources(
		0, 1, &m_pAsciiTexture);

	SetBlend(true);

	//�����������[�v.
	for (UINT i = 0; i < strlen(text); i++)
	{
		char font = text[i];
		int index = font - 32;	//�t�H���g�C���f�b�N�X�쐬.

		//�t�H���g�����_�����O.
		RenderFont(index, x, y);

		x += static_cast<int>(m_fKerning[index]);
	}

	SetBlend(false);
}

//�t�H���g�����_�����O�֐�.
void clsDebugText::RenderFont(
	int FontIndex, int x, int y)
{
	//���[���h�ϊ�.
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(
		&mWorld, (float)x, (float)y,
		-100.0f);	//Z(-100)�Ŏ�O�\���\.

	//�V�F�[�_�̃R���X�^���g�o�b�t�@�Ɋe��f�[�^��n��.
	D3D11_MAPPED_SUBRESOURCE	pData;
	TEXT_CONSTANT_BUFFER		cb;
	if (SUCCEEDED(m_pDeviceContext11->Map(
		m_pConstantBuffer, 0,
		D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//���[���h,�r���[,�v���W�F�N�V�����̍����s���n��.
		D3DXMATRIX m = mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose(&m, &m);
		cb.mWVP = m;
		//�J���[��n��.
		cb.vColor = m_vColor;
		//�����x��n��.
		cb.fAlpha.x = m_fAlpha;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cb), sizeof(cb));
		m_pDeviceContext11->Unmap(
			m_pConstantBuffer, 0);
	}
	//�o�[�e�b�N�X�o�b�t�@���Z�b�g.
	UINT stride = sizeof(TextVertex);
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer[FontIndex],
		&stride, &offset);

	//�`��.
	UINT ColoKey = 0xffffffff;
	m_pDeviceContext11->OMSetBlendState(
		m_pBlendState, NULL, ColoKey);
	m_pDeviceContext11->Draw(4, 0);
	m_pDeviceContext11->OMSetBlendState(
		0, NULL, ColoKey);
}