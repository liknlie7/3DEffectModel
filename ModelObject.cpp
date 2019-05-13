#include "pch.h"
#include "ModelObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// �R���X�g���N�^
ModelObject::ModelObject()
{

}

// �f�X�g���N�^
ModelObject::~ModelObject()
{

}

void ModelObject::Update(float timer)
{
	Vector3 camPos = Vector3(0.f, 0.f, 3.f);

	Matrix rotX = Matrix::CreateRotationX(timer);
	Matrix rotY = Matrix::CreateRotationY(timer);
	Matrix rotZ = Matrix::CreateRotationZ(timer);
	Matrix trans = Matrix::CreateTranslation(Vector3(2.f, 0.f, 0.f));
	Matrix scale = Matrix::CreateScale(Vector3(5.f, 5.f, 5.f));

	/*m_world = rotX * trans * rotY;*/

	// �r���[�s���Update�̈�ԍŌ��
	m_view = Matrix::CreateLookAt(camPos,
		Vector3::Zero, Vector3::UnitY);
}

void ModelObject::Lost()
{
	m_states.reset();
	m_fxFactory.reset();
	m_model.reset();
}

void ModelObject::Create(DX::DeviceResources* deviceResources)
{
	m_deviceResources = deviceResources;
	auto device = m_deviceResources->GetD3DDevice();

	// TODO: Initialize device dependent objects here (independent of window size).
	device;

	// �R�����X�e�[�g���쐬����
	m_states = std::make_unique<CommonStates>(device);
	// �G�t�F�N�g�t�@�N�g���[���쐬����
	m_fxFactory = std::make_unique<EffectFactory>(device);
	// CMO��ǂݍ���Ń��f�����쐬����
	m_model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	// ���[���h�s����쐬����
	m_world = Matrix::Identity;

	// ��ʂ̃T�C�Y���擾����
	RECT outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(outputSize.right - outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(outputSize.bottom - outputSize.top, 1);

	// �ˉe�s������
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 100.1f);
}

void ModelObject::Render()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_model->Draw(context, *m_states, m_world, m_view, m_proj);
}
