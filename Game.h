//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "StepTimer.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <CommonStates.h>
#include <Model.h>

//#include "ModelObject.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();


    // Rendering loop timer.
    DX::StepTimer                           m_timer;

private:
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	DirectX::SimpleMath::Matrix m_taiyou;
	DirectX::SimpleMath::Matrix m_mokusei;
	DirectX::SimpleMath::Matrix m_kinsei;
	DirectX::SimpleMath::Matrix m_tikyuu;
	DirectX::SimpleMath::Matrix m_tuki;

	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_ptaiyou;
	std::unique_ptr<DirectX::Model> m_pmokusei;
	std::unique_ptr<DirectX::Model> m_pkinsei;
	std::unique_ptr<DirectX::Model> m_ptikyuu;
	std::unique_ptr<DirectX::Model> m_ptuki;



};