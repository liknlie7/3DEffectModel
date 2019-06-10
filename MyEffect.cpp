//----------------------------------------
// MyEffect.cpp
//----------------------------------------

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


void MyEffect::Create(DX::DeviceResources* deviceResources, ID3D11ShaderResourceView* texture, AlphaTestEffect* batchEffect, DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>* batch, ID3D11InputLayout* inputLayout)
{
	m_deviceResources = deviceResources;
	auto device = m_deviceResources->GetD3DDevice();

	// �G�t�F�N�g�̎擾
	m_batchEffect = batchEffect;
	// ���̓��C�A�E�g�擾
	m_inputLayout = inputLayout;

	// �v���~�e�B�u�o�b�`�̎擾
	m_batch = batch;

	m_states = std::make_unique<CommonStates>(device);
	// �e�N�X�`���̎擾
	m_texture = texture;

	// �d�͂̉e�����󂯂Ȃ�
	m_isGravity = false;
}

void MyEffect::Initialize(float life, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 velocity)
{
	/*m_position = */m_startPosition = pos;
	//velocity.Normalize();
	//velocity *= 0.05f;
	/*m_velocity = */m_startVelocity = velocity;
	/*m_life = */m_startLife = life;
}

void MyEffect::Update(DX::StepTimer timer)
{
	m_timer = timer;
	float time = float(m_timer.GetElapsedSeconds());

	// �O���O�����(�P��)
	//float total = float(m_timer.GetTotalSeconds());
	//m_position += Vector3((cos(total), sin(total), 0)*0.01f);

	// �����O�ŃO���O�����
	float total = float(m_timer.GetTotalSeconds());
	float rad = atan2(m_velocity.y, m_velocity.x);
	m_position += Vector3(cos(total + rad), sin(total + rad), 0)*0.01f;

	// ����	(�E�ӂ�Velocity�ɂ��Ă��܂��ƌ�������������ԂŎ~�܂�
	//m_velocity -= m_startVelocity * 0.06f;
	//m_position += m_velocity;

	// �d��
	//Vector3 g = Vector3(0, 0.001f, 0);
	//if (m_isGravity)
	//	m_velocity -= g;

	//m_position += m_velocity;

	m_life -= time;

	if (m_life < 0)
	{
		Restart();
		return;
	}

	Vector3 length;
	length = m_position - m_startPosition;
	// �������擾����(float)
	if (length.Length() > 3)
	{
		Restart();
	}
}

void MyEffect::Restart()
{
	m_position = m_startPosition;
	m_velocity = m_startVelocity;
	m_life = m_startLife;
}

void MyEffect::Render()
{
	if (m_life < 0) return;
	m_world = Matrix::CreateBillboard(m_position, m_camera, Vector3::UnitY);
	Draw();

}

void MyEffect::SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	m_camera = camera;
	m_view = view;
	m_proj = proj;
}

void MyEffect::Draw()
{
	auto context = m_deviceResources->GetD3DDeviceContext();

	// TODO: Add your rendering code here.
	context;


	// ���_���
	VertexPositionTexture vertex[4] =
	{
		VertexPositionTexture(Vector3(0.5f, 0.5f, 0.0f), Vector2(0.0f, 0.0f)),
		VertexPositionTexture(Vector3(-0.5f, 0.5f, 0.0f), Vector2(1.0f, 0.0f)),
		VertexPositionTexture(Vector3(-0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f)),
		VertexPositionTexture(Vector3(0.5f,-0.5f, 0.0f), Vector2(0.0f, 1.0f)),
	};
	// �e�N�X�`���T���v���[�̐ݒ�i�N�����v�e�N�X�`���A�h���b�V���O���[�h�j
	ID3D11SamplerState* samplers[1] = { m_states->LinearClamp() };
	context->PSSetSamplers(0, 1, samplers);
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	CD3D11_DEFAULT default_state;

	CD3D11_BLEND_DESC bddesc(default_state);

	blendstate->GetDesc(&bddesc);
	bddesc.AlphaToCoverageEnable = TRUE;
	m_deviceResources->GetD3DDevice()->CreateBlendState(&bddesc, &blendstate);

	// �������菈��
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);
	// �[�x�o�b�t�@�ɏ������ݎQ�Ƃ���
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	// �J�����O�͍�����
	context->RSSetState(m_states->CullCounterClockwise());
	// �s�����̂ݕ`�悷��ݒ�
	m_batchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_batchEffect->SetReferenceAlpha(0);
	m_batchEffect->SetWorld(m_world);
	m_batchEffect->SetView(m_view);
	m_batchEffect->SetProjection(m_proj);
	m_batchEffect->SetTexture(m_texture);
	m_batchEffect->Apply(context);
	context->IASetInputLayout(m_inputLayout);

	// ������������`��
	m_batch->Begin();
	m_batch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_batch->End();
}

