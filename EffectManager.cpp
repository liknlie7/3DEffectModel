#include "pch.h"
#include "EffectManager.h"

using namespace DirectX::SimpleMath;

void EffectManager::Create(DX::DeviceResources * deviceResources, const wchar_t* address, int count)
{
	auto device = deviceResources->GetD3DDevice();

	//�o�b�`�G�t�F�N�g�̍쐬
	m_batchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_batchEffect->SetReferenceAlpha(255);

	// ���̓��C�A�E�g����
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf());

	//�v���~�e�B�u�o�b�`�̍쐬
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(deviceResources->GetD3DDeviceContext());




	//�e�N�X�`���̃��[�h
	DirectX::CreateWICTextureFromFile(device, address, nullptr, m_texture.GetAddressOf());


	// ���g������
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

// �ւŔ�΂�--------------------------------------------------------------------------------
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

		// ite = MyEffect*�̃|�C���^�[ 
		(*ite)->Initialize(life, pos, vel);
		num++;
	}
}
// ------------------------------------------------------------------------------------------------

// �R�[����ɔ�΂�--------------------------------------------------------------------------------
void EffectManager::InitializeCone(float life, Vector3 pos, Vector3 dir)
{
	int range = 100;
	dir.Normalize();


	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 d = dir;
		d *= sinf(rand()) + 2.0f;

		// ���s����x�N�g���v�Z
		Vector3 cross = Vector3(d.y, -d.x, 0);

		// ���ŏ�Z���邱�ƂŐ�̕��������Ȃ�
		cross *= sinf(rand()) * 0.3f;

		d = d + cross;
		// ���x
		d *= 0.1f;

		// ite = MyEffect*�̃|�C���^�[ 
		(*ite)->Initialize(life, pos, d);
	}
}
// ------------------------------------------------------------------------------------------------

void EffectManager::Update(DX::StepTimer timer)
{
	//timer��n����m_effect�̍X�V�������s��
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*�̃|�C���^�[ 
		(*ite)->Update(timer);
	}
}

void EffectManager::Render()
{
	//m_effect��`�悷��
	// [iterator]int i�݂����Ȃ���    �^�� �ϐ��� = �񂵂�����   begin�ŏ�����next��null�̂��̂܂�
	//																	!=�łȂ��Ɠ����Ȃ�
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*�̃|�C���^�[ 
		(*ite)->Render();
	}
}

void EffectManager::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	//camera,view,proj,�̒l��m_effect�ɓn��
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		// ite = MyEffect*�̃|�C���^�[ 
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
