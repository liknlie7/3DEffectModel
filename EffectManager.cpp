#include "pch.h"
#include "EffectManager.h"

using namespace DirectX::SimpleMath;

void EffectManager::Create(DX::DeviceResources * deviceResources, const wchar_t* address, int count)
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
		DirectX::VertexPositionTexture::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf());

	//プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(deviceResources->GetD3DDeviceContext());




	//テクスチャのロード
	DirectX::CreateWICTextureFromFile(device, address, nullptr, m_texture.GetAddressOf());


	// 中身を消す
	m_effectList.clear();

	for (int i = 0; i < count; i++)
	{
		MyEffect* effect = new MyEffect();

		effect->Create(deviceResources, m_texture.Get(), m_batchEffect.get(), m_batch.get(), m_inputLayout.Get());

		m_effectList.push_back(effect);

	}
}

void EffectManager::InitializeSnowS(float life)
{
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 pos = Vector3(100, 100, 0);
		Vector3 vel = Vector3(0, 0, 0);

		(*ite)->Initialize(life, pos, vel);
	}
}
void EffectManager::InitializeSnowL(float life)
{
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
	MyEffect* myEffect;
	myEffect = new MyEffect();

		srand((unsigned int)time(NULL));

		Vector3 pos = Vector3((rand() % 4) + 1, (rand() % 4) + 6, (rand() % 4) - 2);

		Vector3 vel = Vector3(0, 0, 0);

		myEffect->Initialize(life, pos, vel);
		(*ite)->Initialize(life, pos, vel);
	}
}

// 輪で飛ばす--------------------------------------------------------------------------------
void EffectManager::InitializeAverage(float life, Vector3 pos)
{
	int range = 100;

	const float RAD = DirectX::XM_PI * 2;

	int num = 0;

	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		float size = m_effectList.size();
		Vector3 vel = Vector3(cos(RAD*num / size + DirectX::XM_PI / 2.0f), sin(RAD*num / size + DirectX::XM_PI / 2.0f), 0);
		vel.Normalize();
		vel *= 0.1f;

		// ite = MyEffect*のポインター 
		(*ite)->Initialize(life, pos, vel);
		num++;
	}
}
// ------------------------------------------------------------------------------------------------

// コーン状に飛ばす--------------------------------------------------------------------------------
void EffectManager::InitializeCone(float life, Vector3 pos, Vector3 dir)
{
	int range = 100;
	dir.Normalize();


	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 d = dir;
		d *= sinf(rand()) + 2.0f;

		// 直行するベクトル計算
		Vector3 cross = Vector3(d.y, -d.x, 0);

		// 後ろで乗算することで扇の幅が狭くなる
		cross *= sinf(rand()) * 0.3f;

		d = d + cross;
		// 速度
		d *= 0.1f;

		// ite = MyEffect*のポインター 
		(*ite)->Initialize(life, pos, d);
	}
}
// ------------------------------------------------------------------------------------------------

void EffectManager::Update(DX::StepTimer timer)
{
	//timerを渡してm_effectの更新処理を行う
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*のポインター 
		(*ite)->Update(timer);
	}
}

void EffectManager::Render()
{
	//m_effectを描画する
	// [iterator]int iみたいなもの    型名 変数名 = 回したい物   begin最初からnextがnullのものまで
	//																	!=でないと動かない
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*のポインター 
		(*ite)->Render();
	}
}

void EffectManager::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	//camera,view,proj,の値をm_effectに渡す
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*のポインター 
		(*ite)->SetRenderState(camera, view, proj);
	}
}

void EffectManager::SetGravity(bool gravity)
{

	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		(*ite)->SetGravity(gravity);
	}

}
