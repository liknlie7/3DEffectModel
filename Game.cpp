//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

// インクルードを追加
#include <SimpleMath.h>
#include <Effects.h>
#include <CommonStates.h>
#include <Model.h>
#include "EffectManager.h"
extern void ExitGame();

// namespaceを追加
using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
	elapsedTime;

	float time = float(timer.GetTotalSeconds());

	m_effectManager->Update(timer);

	Vector3 camPos = Vector3(0.0f, 0.0f, 5.0f);

	m_effectManager->SetRenderState(camPos,m_view,m_proj);


	// ビュー行列はUpdateの一番最後に
	m_view = Matrix::CreateLookAt(camPos,
		Vector3::Zero, Vector3::UnitY);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();


	// TODO: Add your rendering code here.
	context;
	
	m_effectManager->Render();

	m_deviceResources->PIXEndEvent();
	// Show the new frame.
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
	// TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
	// TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
	// TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

	// TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();

	// TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	// TODO: Change to desired default window size (note minimum size is 320x200).
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto device = m_deviceResources->GetD3DDevice();

	// TODO: Initialize device dependent objects here (independent of window size).
	device;


	// 画面のサイズを取得する
	RECT outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(outputSize.right - outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(outputSize.bottom - outputSize.top, 1);

	m_effectManager = new EffectManager;
	m_effectManager->Create(m_deviceResources.get());
	m_effectManager->Initialize(3.0f,Vector3::Zero);

	// 射影行列を作る
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 100.1f);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	// TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
	// TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_fxFactory.reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion
