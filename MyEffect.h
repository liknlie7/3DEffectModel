//----------------------------------------
// MyEffect.h
//----------------------------------------
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
	//マネージャ管理のため共通項目を外部から貰えるように変更
	void Create(DX::DeviceResources* deviceResources, ID3D11ShaderResourceView* texture,DirectX::AlphaTestEffect* batchEffect, DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>* batch,ID3D11InputLayout* inputLayout);

	void Initialize(float life = 100.0f, DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3 velocity = DirectX::SimpleMath::Vector3::Zero);
	void Update(DX::StepTimer timer);
	void Render();

	void SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
protected:

	void Draw();

	DX::StepTimer                           m_timer;
	DX::DeviceResources*			m_deviceResources;
	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;
	// エフェクト
	DirectX::AlphaTestEffect* m_batchEffect;
	// プリミティブバッチ
	DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>* m_batch;
	// 入力レイアウト
	ID3D11InputLayout* m_inputLayout;

	std::unique_ptr<DirectX::CommonStates> m_states;
	// テクスチャハンドル
	ID3D11ShaderResourceView* m_texture;
	DirectX::SimpleMath::Vector3		m_camera;

	DirectX::SimpleMath::Vector3		m_position;
	DirectX::SimpleMath::Vector3		m_velocity;
	float								m_life;



};