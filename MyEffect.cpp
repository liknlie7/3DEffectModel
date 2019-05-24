#include "pch.h"
#include "MyEffect.h"

#include "DeviceResources.h"
#include "StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

void MyEffect::Create(DX::DeviceResources* deviceResources, float life, Vector3 pos, Vector3 velocity)
{
	m_deviceResources = deviceResources;
	auto device = m_deviceResources->GetD3DDevice();

	// エフェクトの作成
	m_batchEffect = std::make_unique<AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	// 入力レイアウト作成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf());

	// プリミティブバッチの作成
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(m_deviceResources->GetD3DDeviceContext());

	m_states = std::make_unique<CommonStates>(device);

	// テクスチャのロード
	CreateWICTextureFromFile(device, L"Resources/Textures/image01.png", nullptr, m_texture.GetAddressOf());
	m_position = pos;
	m_velocity = velocity;
	m_life = life;
}

void MyEffect::Update(DX::StepTimer timer)
{
	float time = (m_timer.GetElapsedSeconds());
	m_timer = timer;
	m_position += m_velocity;

	m_life -= time;
}

void MyEffect::Render()
{
	if (m_life < 0) return;
	m_world = Matrix::CreateBillboard(m_position, m_camera, Vector3::UnitY);
	Draw(m_world, m_view, m_proj);
}

void MyEffect::SetRenderState(DirectX::SimpleMath::Vector3 camera,
	DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	m_camera = camera;
	m_view = view;
	m_proj = proj;
}

void MyEffect::Draw(DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj)
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	// TODO: Add your rendering code here. 
	context;


	// 頂点情報
	VertexPositionTexture vertex[4] =
	{
		VertexPositionTexture(Vector3(0.5f,0.5f,0.0f),Vector2(0.0f,0.0f)),
		VertexPositionTexture(Vector3(-0.5f,0.5f,0.0f),Vector2(1.0f,0.0f)),
		VertexPositionTexture(Vector3(-0.5f,-0.5f,0.0f),Vector2(1.0f,1.0f)),
		VertexPositionTexture(Vector3(0.5f,-0.5f,0.0f),Vector2(0.0f,1.0f)),
	};
	
	// テクスチャサンプラーの設定（クランプテクスチャアドレッシングモード）
	ID3D11SamplerState* samplers[1] = { m_states->LinearClamp() };
	context->PSSetSamplers(0, 1, samplers);
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	CD3D11_DEFAULT default_state;

	CD3D11_BLEND_DESC bddesc(default_state);

	blendstate->GetDesc(&bddesc);
	bddesc.AlphaToCoverageEnable = TRUE;
	m_deviceResources->GetD3DDevice()->CreateBlendState(&bddesc, &blendstate);

	// 透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	// 深度バッファに書き込み参照する 
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	// カリングは左周り 
	context->RSSetState(m_states->CullCounterClockwise());

	// 不透明のみ描画する設定 
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);
	m_batchEffect->SetWorld(m_world);
	m_batchEffect->SetView(m_view);
	m_batchEffect->SetProjection(m_proj);
	m_batchEffect->SetTexture(m_texture.Get());
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout.Get());

	// 半透明部分を描画 
	m_batch->Begin();
	m_batch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_batch->End();
}