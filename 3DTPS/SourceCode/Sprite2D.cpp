#include "Sprite2d.h"

//============================================================
//	定数.
//============================================================

//シェーダファイル名(ディレクトリも含む)(\\).
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
	// リリース関数をとりあえず、デストラクタに突っ込む.
	Release();
}

//============================================================
//	初期化.
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

	// この構造体の中に画像の情報が入る
	D3DXIMAGE_INFO info;

	// filenameはそのまんま、対象の画像ファイル名
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
//	HLSLファイルを読み込みシェーダを作成する.
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

	//HLSLからバーテックスシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME.c_str(),	//シェーダファイル名(HLSLファイル).
			NULL,			//マクロ定義の配列へのポインタ(未使用).
			NULL,			//インクルードファイルを扱うインターフェースへのポインタ(未使用).
			"VS_Main",			//シェーダエントリーポイント関数の名前.
			"vs_5_0",		//シェーダのモデルを指定する文字列(プロファイル).
			uCompileFlag,	//シェーダコンパイルフラグ.
			0,				//エフェクトコンパイルフラグ(未使用).
			NULL,			//スレッドポンプインターフェースへのポインタ(未使用).
			&pCompiledShader,//ブロブを格納するメモリへのポインタ.
			&pErrors,		//エラーと警告一覧を格納するメモリへのポインタ.
			NULL)))		//戻り値へのポインタ(未使用).
	{
		MessageBox(NULL, "hlsl(vs)読み込み失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「バーテックスシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pVertexShader)))//(out)バーテックスシェーダ.
	{
		MessageBox(NULL, "vs作成失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						//位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	//DXGIのフォーマット(32bit float型*3).
			0,
			0,								//データの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",					//テクスチャ位置.
			0,
			DXGI_FORMAT_R32G32_FLOAT,	//DXGIのフォーマット(32bit float型*2).
			0,
			12,							//テクスチャデータの開始位置(頂点データがfloat型*3=12biteなので+12).
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//頂点インプットレイアウトの配列要素数を算出.
	UINT numElements = sizeof(layout) / sizeof(layout[0]);//.

	//頂点インプットレイアウトの作成.
	if (FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout)))//(out)頂点インプットレイアウト.
	{
		MessageBox(NULL, "頂点インプットレイアウト作成失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);

	//HLSLからピクセルシェーダのブロブを作成.
	if (FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME.c_str(),	//シェーダファイル名(HLSLファイル).
			NULL,
			NULL,
			"PS_Main",			//シェーダエントリーポイント関数の名前.
			"ps_5_0",		//シェーダのモデルを指定する文字列(プロファイル).
			uCompileFlag,	//シェーダコンパイルフラグ.
			0,
			NULL,
			&pCompiledShader,//ブロブを格納するメモリへのポインタ.
			&pErrors,
			NULL)))
	{
		MessageBox(NULL, "hlsl(ps)読み込み失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	//上記で作成したブロブから「ピクセルシェーダ」を作成.
	if (FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL,
			&m_pPixelShader)))//(out)ピクセルシェーダ.
	{
		MessageBox(NULL, "ps作成失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);//ブロブ解放.

	//コンスタントバッファ(定数)バッファ作成　シェーダに特定の数値を送るバッファ.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;;			//コンストバッファを指定.
	cb.ByteWidth = sizeof(SPRITE2D_CONSTANT_BUFFER);//コンスタントバッファのサイズ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				//書き込みでアクセス.
	cb.MiscFlags = 0;					//その他のフラグ(未使用).
	cb.StructureByteStride = 0;			//構造体のサイズ(未使用)
	cb.Usage = D3D11_USAGE_DYNAMIC;	//使用方法:直接書き込み.

	//コンスタントバッファ作成.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			NULL,
			&m_pConstantBuffer)))
	{
		MessageBox(NULL, "コンスタントバッファ作成失敗", "clsSprite2D::InitShader", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	モデル作成.
//============================================================
HRESULT clsSprite2D::InitModel(SPRITE_STATE ss)
{
	float fW = ss.Disp.w;	//表示スプライト幅.
	float fH = ss.Disp.h;	//表示スプライト高さ.
	float fU = (ss.Base.w / ss.Stride.w) / ss.Base.w;	//一コマあたりの幅.
	float fV = (ss.Base.h / ss.Stride.h) / ss.Base.h;	//一コマあたりの高さ.

	//板ポリ(四角形)の頂点を作成.
	Sprite2DVertex vertices[] =
	{
		//頂点座標(x,y,z).					//UV座標( u, v ).
		D3DXVECTOR3(0.0f, fH, 0.0f), D3DXVECTOR2(0.0f, fV),	//頂点1(左下).
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f),	//頂点2(左上).
		D3DXVECTOR3(fW, fH, 0.0f), D3DXVECTOR2(fU, fV),	//頂点3(右下).
		D3DXVECTOR3(fW, 0.0f, 0.0f), D3DXVECTOR2(fU, 0.0f)	//頂点4(右上).
	};
	//最大要素数を算出する.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	//バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;		//使用法(デフォルト).
	bd.ByteWidth = sizeof(Sprite2DVertex) * uVerMax;//頂点サイズ(頂点*4).
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//頂点バッファとして扱う.
	bd.CPUAccessFlags = 0;						//CPUからはアクセスしない.
	bd.MiscFlags = 0;						//その他のフラグ(未使用).
	bd.StructureByteStride = 0;					//構造体サイズ(未使用).

	//サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	//板ポリの頂点をセット.

	//頂点バッファの作成.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "頂点バッファ作成失敗", "clsSprite2D::InitModel", MB_OK);
		return E_FAIL;
	}

	//頂点バッファをセット.
	UINT stride = sizeof(Sprite2DVertex);//データ間隔.
	UINT offset = 0;//開始位置.
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1,
		&m_pVertexBuffer, &stride, &offset);

	//テクスチャ用のサンプラー構造体.
	D3D11_SAMPLER_DESC	SamDesc;
	ZeroMemory(&SamDesc, sizeof(SamDesc));
	//リニアフィルター(線型補間).
	//	POINT:高速だが粗い.
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//ラッピングモード.
	//	WRAP:繰り返し.
	//	MIRROR	:反転繰り返し.
	//	CLAMP	:端の模様を引き延ばす.
	//	BORDER	;別途境界色を決める.
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//サンプラー作成.
	if (FAILED(
		m_pDevice11->CreateSamplerState(
			&SamDesc, &m_pSampleLinear)))//(out)サンプラー.
	{
		MessageBox(NULL, "サンプラ作成失敗", "clsSprite2D::InitModel", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//	テクスチャ作成.
//============================================================
HRESULT clsSprite2D::CreateTexture(LPSTR fileName,
	ID3D11ShaderResourceView** pTex)
{
	//テクスチャ作成.
	if (FAILED(
		D3DX11CreateShaderResourceViewFromFile(
			m_pDevice11,		//リソースを使用するデバイスへのポインタ.
			fileName,	//ファイル名(パスも必要).
			NULL, NULL,
			pTex,	//(out)テクスチャ.
			NULL)))
	{
		ERR_MSG(fileName, "clsSprite2D::CreateTexture");
		return E_FAIL;
	}

	return S_OK;
}

//============================================================
//描画(レンダリング)(※DX9MESH内とMain内で2つ存在するので注意).
//============================================================
void clsSprite2D::Render()
{
	//ワールド行列.
	D3DXMATRIX	mWorld;

	//ワールド変換(表示位置を設定する).
	D3DXMatrixIdentity(&mWorld);	//初期化:単位行列作成.

	//	D3DXMatrixScaling();

	//平行移動.
	D3DXMatrixTranslation(&mWorld,
		m_vPos.x, m_vPos.y, m_vPos.z);

	//使用するシェーダの登録.
	m_pDeviceContext11->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext11->PSSetShader(m_pPixelShader, NULL, 0);

	//シェーダのコンスタントバッファに各種データを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	SPRITE2D_CONSTANT_BUFFER cd;	//コンスタントバッファ.
	//バッファ内のデータの書き方開始時にmap.
	if (SUCCEEDED(
		m_pDeviceContext11->Map(
			m_pConstantBuffer, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド行列を渡す.
		D3DXMATRIX m = mWorld;
		D3DXMatrixTranspose(&m, &m);	//行列を転置する.
		//行列の計算方法がDirectXとGPUで異なるため.
		cd.mW = m;

		//ビューポートの幅、高さを渡す.
		cd.fViewPortWidth = WND_W;
		cd.fViewPortHeight = WND_H;

		//アルファ値を渡す.
		cd.fAlpha = m_fAlpha;

		//UV座標.
		//1コア当たりの割合にコマ番号を掛けて位置を設定する.
		WHSIZE_FLOAT wf;
		wf.w = (m_SState.Base.w / m_SState.Stride.w) / m_SState.Base.w;
		wf.h = (m_SState.Base.h / m_SState.Stride.h) / m_SState.Base.h;
		cd.vUV.x = wf.w * m_fPatternNo.x;
		cd.vUV.y = wf.h * m_fPatternNo.y;

		memcpy_s(pData.pData, pData.RowPitch,
			(void*)(&cd), sizeof(cd));

		m_pDeviceContext11->Unmap(m_pConstantBuffer, 0);
	}

	//このコンスタントバッファをどのシェーダで使うか?.
	m_pDeviceContext11->VSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);
	m_pDeviceContext11->PSSetConstantBuffers(
		0, 1, &m_pConstantBuffer);

	//頂点バッファをセット.
	UINT stride = sizeof(Sprite2DVertex);
	UINT offset = 0;
	m_pDeviceContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &stride, &offset);

	//頂点インプットレイアウトをセット.
	m_pDeviceContext11->IASetInputLayout(m_pVertexLayout);

	//プリミティブ・トポロジーをセット.
	m_pDeviceContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャをシェーダに渡す.
	m_pDeviceContext11->PSSetSamplers(
		0, 1, &m_pSampleLinear);	//サンプラーをセット.
	m_pDeviceContext11->PSSetShaderResources(
		0, 1, &m_pTexture);		//テクスチャをシェーダに渡す.

	//アルファブレンド用ブレンドステート作成&設定.
	SetBlend(true);

	//プリミティブをレンダリング.
	m_pDeviceContext11->Draw(4, 0);

	//アルファブレンドを無効にする.
	SetBlend(false);
}

void clsSprite2D::UpDateSpriteSs()
{
	SPRITE_STATE ss = m_SState;
	float fW = ss.Disp.w;	//表示スプライト幅.
	float fH = ss.Disp.h;	//表示スプライト高さ.
	float fU = (ss.Base.w / ss.Stride.w) / ss.Base.w;	//一コマあたりの幅.
	float fV = (ss.Base.h / ss.Stride.h) / ss.Base.h;	//一コマあたりの高さ.

	//板ポリ(四角形)の頂点を作成.
	Sprite2DVertex vertices[] =
	{
		//頂点座標(x,y,z).					//UV座標( u, v ).
		D3DXVECTOR3(0.0f, fH, 0.0f), D3DXVECTOR2(0.0f, fV),	//頂点1(左下).
		D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f),	//頂点2(左上).
		D3DXVECTOR3(fW, fH, 0.0f), D3DXVECTOR2(fU, fV),	//頂点3(右下).
		D3DXVECTOR3(fW, 0.0f, 0.0f), D3DXVECTOR2(fU, 0.0f)	//頂点4(右上).
	};
	//最大要素数を算出する.
	UINT uVerMax = sizeof(vertices) / sizeof(vertices[0]);

	//バッファ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;		//使用法(デフォルト).
	bd.ByteWidth = sizeof(Sprite2DVertex) * uVerMax;//頂点サイズ(頂点*4).
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//頂点バッファとして扱う.
	bd.CPUAccessFlags = 0;						//CPUからはアクセスしない.
	bd.MiscFlags = 0;						//その他のフラグ(未使用).
	bd.StructureByteStride = 0;					//構造体サイズ(未使用).

	//サブリソースデータ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	//板ポリの頂点をセット.

	//頂点バッファの作成.
	if (FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_pVertexBuffer)))
	{
		MessageBox(NULL, "頂点バッファ作成失敗,", "clsSprite2D::InitModel", MB_OK);
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