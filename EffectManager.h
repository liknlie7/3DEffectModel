//-----------------------------------
//	エフェクト管理クラス	
//	EffectManager.h
//-----------------------------------
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include <SimpleMath.h>
#include <list>
#include <WICTextureLoader.h>
#include <random>
#include <ctime>

#include "Myeffect.h"

class EffectManager
{
public:
	void Create(DX::DeviceResources* deviceResources, const wchar_t* address1, int count = 1);
	//Create()で全部処理は長いので分割
	void InitializeSnowS(float life);
	void InitializeSnowL(float life);
	void InitializeFog(float life);
	void InitializeAverage(float life, DirectX::SimpleMath::Vector3 pos);
	void InitializeCone(float life, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 dir);


	void Update(DX::StepTimer timer);
	void Render();

	void SetRenderState(DirectX::SimpleMath::Vector3 camera, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	void SetGravity(bool gravity);

private:
	//バッチエフェクト
	std::unique_ptr<DirectX::AlphaTestEffect> m_batchEffect;
	//プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_batch;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;


	//エフェクトのリスト
	std::list<MyEffect*>	m_effectList;

	//テクスチャデータ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_texture;



};