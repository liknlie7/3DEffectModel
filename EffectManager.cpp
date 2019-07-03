#include "pch.h"
#include "EffectManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void EffectManager::Create(DX::DeviceResources * deviceResources, const wchar_t* path, int count)
{
	auto device = deviceResources->GetD3DDevice();

	//バッチエフェクトの作成
	m_batchEffect = std::make_unique<AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);
	// 入力レイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf());

	//プリミティブバッチの作成
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(deviceResources->GetD3DDeviceContext());
	//テクスチャのロード
	//const wchar_t* path = L"Resources/Textures/image01.png";
	CreateWICTextureFromFile(device, path, nullptr, m_texture.GetAddressOf());

	m_effectList.clear();

	//int count = 1000;
	for (int i = 0; i < count; i++) {
		MyEffect* effect = new MyEffect();
		effect->Create(deviceResources, m_texture.Get(), m_batchEffect.get(), m_batch.get(), m_inputLayout.Get());
		m_effectList.push_back(effect);
	}
}
void EffectManager::InitializeAverage(float life, Vector3 pos)
{
	int range = 100;

	const float RAD = XM_PI * 2.0f;
	int num = 0;
	//life,pos,vel の値でm_effectを初期化する
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		float size = (float)m_effectList.size();
		Vector3 vel = Vector3(cos(RAD * num / size + XM_PI / 2.0f), sin(RAD * num / size + XM_PI / 2.0f), 0);
		vel *= 0.1;
		(*ite)->Initialize(life, pos, vel);
		num++;
	}
}

void EffectManager::Initialize(float life, Vector3 pos)
{
	int range = 100;


	//life,pos,vel の値でm_effectを初期化する
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
		while (vel.Length() < 0.03f)
		{
			vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
		}

		(*ite)->Initialize(life, pos, vel);
	}
}


void EffectManager::SetGravity(bool gravity)
{
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		(*ite)->SetGravity(gravity);
	}
}

void EffectManager::InitializeCone(float life, Vector3 pos, Vector3 dir)
{
	int range = 100;
	dir.Normalize();




	//life,pos,vel の値でm_effectを初期化する
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 d = dir;
		d *= sinf((float)rand()) + 1.0f;

		Vector3 cross = Vector3(d.y, -d.x, 0);
		cross *= sinf((float)rand());
		cross *= 0.1f;

		d += cross;
		d *= 0.08f;

		(*ite)->Initialize(life, pos, d);
	}
}

void EffectManager::Update(DX::StepTimer timer)
{
	//timerを渡してm_effectの更新処理を行う
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		(*ite)->Update(timer);
	}
}

void EffectManager::Render()
{
	//m_effectを描画する
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		(*ite)->Render();
	}
}

void EffectManager::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	//camera,view,proj,の値をm_effectに渡す
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		(*ite)->SetRenderState(camera, view, proj);
	}
}
