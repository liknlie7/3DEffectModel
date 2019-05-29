#include "pch.h"
#include "EffectManager.h"

using namespace DirectX::SimpleMath;

//																const�t���Ă������ق����ǂ�
void EffectManager::Create(DX::DeviceResources * deviceResources, const wchar_t* address,int count)
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

void EffectManager::Initialize(float life, Vector3 pos)
{
	int range = 100;
	//life,pos,vel �̒l��m_effect������������
	for (std::list<MyEffect*>::iterator ite = m_effectList.begin(); ite != m_effectList.end(); ite++)
	{
		Vector3 vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
		while (vel.Length() < 0.03f)
		{
			vel = Vector3(((rand() % (range * 2)) - range) / (float)range * 0.1f, ((rand() % (range * 2)) - range) / (float)range * 0.1f, 0);
		}

		// ite = MyEffect*�̃|�C���^�[ 
		(*ite)->Initialize(life, pos, vel);
	}
}


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
