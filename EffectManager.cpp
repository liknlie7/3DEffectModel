#include "pch.h"
#include "EffectManager.h"

using namespace DirectX::SimpleMath;

void EffectManager::Create(DX::DeviceResources * deviceResources)
{
	auto device = deviceResources->GetD3DDevice();

	//バッチエフェクトの作成
	m_batchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);
	// 入力レイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,shaderByteCode,byteCodeLength,m_inputLayout.GetAddressOf());

	//プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(deviceResources->GetD3DDeviceContext());
	
	//テクスチャのロード
	DirectX::CreateWICTextureFromFile(device,L"Resources/Textures/image01.png",nullptr,m_texture.GetAddressOf());
	

	m_effect = new MyEffect();
	m_effect->Create(deviceResources,m_texture.Get(),m_batchEffect.get(),m_batch.get(),m_inputLayout.Get());
		
}

void EffectManager::Initialize(float life,Vector3 pos)
{
	int range = 100;
	Vector3 vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
	while (vel.Length() < 0.03f)
	{
		vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
	}

	//life,pos,vel の値でm_effectを初期化する
	m_effect->Initialize(life, pos, vel);
}


void EffectManager::Update(DX::StepTimer timer)
{
	//timerを渡してm_effectの更新処理を行う
	m_effect->Update(timer);
}

void EffectManager::Render()
{
	//m_effectを描画する
	m_effect->Render();
}

void EffectManager::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	//camera,view,proj,の値をm_effectに渡す
	m_effect->SetRenderState(camera, view, proj);
}
