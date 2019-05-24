#pragma once

#include "DeviceResources.h"

#include "StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>

class MyEffect
{
public:
	void Create(DX::DeviceResources* deviceResources, float life = 100.0f,
		DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3::Zero,
		DirectX::SimpleMath::Vector3 velocity = DirectX::SimpleMath::Vector3::Zero);

	void Update(DX::StepTimer timer);
	void Render();

	void SetRenderState(DirectX::SimpleMath::Vector3 camera,
		DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

private:

	void Draw(DirectX::SimpleMath::Matrix world, DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj);

	DX::StepTimer					m_timer;
	DX::DeviceResources*			m_deviceResources;
	DirectX::SimpleMath::Matrix		m_world;
	DirectX::SimpleMath::Matrix		m_view;
	DirectX::SimpleMath::Matrix		m_proj;

	// エフェクト
	std::unique_ptr<DirectX::AlphaTestEffect>									m_batchEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>	m_batch;

	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>									m_inputLayout;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates>										m_states;

	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>							m_texture;

	DirectX::SimpleMath::Vector3												m_position;
	DirectX::SimpleMath::Vector3												m_velocity;
	DirectX::SimpleMath::Vector3												m_camera;
	float																		m_life;
};