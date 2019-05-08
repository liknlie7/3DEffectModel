#pragma once

#include "DeviceResources.h"

#include <SimpleMath.h>
#include <Effects.h>
#include <CommonStates.h>
#include <Model.h>

class ModelObject
{
public:
	ModelObject();
	~ModelObject();

	void Update();  
	void Lost();  
	void Create(DX::DeviceResources* deviceResources);
    void Render();

private:
	DX::DeviceResources* m_deviceResources;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_model;

};