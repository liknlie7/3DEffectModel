#include "pch.h"
#include "ShaderSample.h"
#include "BinaryFile.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

using namespace DX;
using namespace DirectX;
using namespace DirectX::SimpleMath;
const std::vector<D3D11_INPUT_ELEMENT_DESC> ShaderSample::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(Vector3) + sizeof(Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
ShaderSample::ShaderSample()
{
}
ShaderSample::~ShaderSample()
{
	Lost();
}
void ShaderSample::Create(DeviceResources * deviceresources)
{
	m_deviceResources = deviceresources;
	auto device = m_deviceResources->GetD3DDevice();

	// コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");

	device->CreateInputLayout(&INPUT_LAYOUT[0],
		INPUT_LAYOUT.size(),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.GetAddressOf());

	// 頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_VertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	// ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_PixelShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	const wchar_t* name = L"Resources\\Textures\\image01.png";
	DirectX::CreateWICTextureFromFile(m_deviceResources->GetD3DDevice(), name, nullptr, m_texture.GetAddressOf());

	const wchar_t* name2 = L"Resources\\Textures\\ball.png";
	DirectX::CreateWICTextureFromFile(m_deviceResources->GetD3DDevice(), name2, nullptr, m_texture2.GetAddressOf());
	// プリミティブバッチの作成
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(m_deviceResources->GetD3DDeviceContext());

	m_states = std::make_unique<CommonStates>(device);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage		=	D3D11_USAGE_DEFAULT;
	bd.ByteWidth	=	sizeof(ConstBuffer);
	bd.BindFlags	=	D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);

	m_world = Matrix::Identity;
}
void ShaderSample::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	m_camera = camera;
	m_view = view;
	m_proj = proj;
}
void ShaderSample::Initialize(float life, Vector3 pos, Vector3 velocity)
{
	m_startPosition = m_position = pos;
	m_startVelocity = m_velocity = velocity;
	m_startLife = m_life = life;
	m_gravity = Vector3(0, 0.001f, 0);
}

void ShaderSample::Update(StepTimer timer)
{
	m_timer = timer;
	float time = float(m_timer.GetElapsedSeconds());

	if (m_startVelocity.Length() > 0.02f) {
		m_velocity -= m_gravity;
	}
	m_position += m_velocity;

	m_life -= time*m_position.Length();
	if ((m_position - m_startPosition).Length() > 10)
	{
		m_life = m_startLife;
		m_position = m_startPosition;
		m_velocity = m_startVelocity;
	}
	if (m_life < 0)
	{
		m_life = m_startLife;
		m_position = m_startPosition;
		m_velocity = m_startVelocity;
	}
}

void ShaderSample::Render()
{
	m_world = Matrix::CreateBillboard(m_position, m_camera, Vector3::UnitY);
	ShaderDraw();
}

void ShaderSample::ShaderDraw()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	// 頂点情報
	VertexPositionColorTexture vertex[4] =
	{
		VertexPositionColorTexture(Vector3(0.5f, 0.5f, 0.0f),Vector4::One, Vector2(1.0f, 0.0f)),
		VertexPositionColorTexture(Vector3(-0.5f, 0.5f, 0.0f),Vector4::One, Vector2(0.0f, 0.0f)),
		VertexPositionColorTexture(Vector3(-0.5f, -0.5f, 0.0f),Vector4::One, Vector2(0.0f, 1.0f)),
		VertexPositionColorTexture(Vector3(0.5f,-0.5f, 0.0f),Vector4::One, Vector2(1.0f, 1.0f)),
	};

	RECT outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(outputSize.right - outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(outputSize.bottom - outputSize.top, 1);
	Vector3 camera = Vector3(0, 0, -3);
	Matrix view = Matrix::CreateLookAt(camera,
		Vector3::Zero, Vector3::UnitY);
	Matrix proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 1000.f);

	ConstBuffer cbuff;

	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = Vector4(1, 1, 1, 1);

	//定数バッファの内容更新
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);


	ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	ID3D11BlendState* blendstate = m_states->NonPremultiplied();
	// 透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// 深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	// カリングは左周り
	context->RSSetState(m_states->CullNone());

	context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_texture2.GetAddressOf());


	context->IASetInputLayout(m_inputLayout.Get());

	// 半透明部分を描画
	m_batch->Begin();
	m_batch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_batch->End();
}

void ShaderSample::Draw()
{
}

void ShaderSample::Lost()
{
	m_CBuffer.Reset();
	m_inputLayout.Reset();
	m_batch.reset();
	m_states.reset();
	m_texture.Reset();
	m_texture2.Reset();
	m_VertexShader.Reset();
	m_PixelShader.Reset();
}
