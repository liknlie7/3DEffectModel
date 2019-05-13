#include "pch.h"
#include "ModelObject.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// コンストラクタ
ModelObject::ModelObject()
{

}

// デストラクタ
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

	// ビュー行列はUpdateの一番最後に
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

	// コモンステートを作成する
	m_states = std::make_unique<CommonStates>(device);
	// エフェクトファクトリーを作成する
	m_fxFactory = std::make_unique<EffectFactory>(device);
	// CMOを読み込んでモデルを作成する
	m_model = Model::CreateFromCMO(device, L"cup.cmo", *m_fxFactory);
	// ワールド行列を作成する
	m_world = Matrix::Identity;

	// 画面のサイズを取得する
	RECT outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(outputSize.right - outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(outputSize.bottom - outputSize.top, 1);

	// 射影行列を作る
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 100.1f);
}

void ModelObject::Render()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_model->Draw(context, *m_states, m_world, m_view, m_proj);
}
