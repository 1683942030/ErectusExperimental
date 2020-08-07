#include "app.h"
#include "gui.h"
#include "renderer.h"
#include "common.h"
#include "settings.h"
#include "threads.h"

#include <thread>

#include "ErectusProcess.h"
#include "Looter.h"
#include "Window.hpp"


App::App(const HINSTANCE hInstance, const LPCSTR windowTitle) : appInstance(hInstance)
{
	Init(windowTitle);
}

App::~App()
{
	Detach();
	Renderer::Shutdown();
	Settings::Write();
}

void App::Init(LPCSTR windowTitle)
{
	appWindow = std::make_unique<Window>(this, windowTitle);

	if (!Renderer::Init(appWindow->GetHwnd()))
		return;

	Settings::Read();
}

void App::Shutdown()
{
	continueRunning = false;

	//everything should be  handled by destructors...
}

void App::Run()
{
	started = true;

	MSG msg;

	while (continueRunning)
	{
		Update();
		Render();

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			switch (msg.message)
			{
			case WM_QUIT:
				continueRunning = false;
				break;
			case WM_HOTKEY:
				OnHotkey(msg.wParam);
				break;
			default:
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}

void App::OnWindowChanged() const
{
	if (!started)
		return;

	RECT rect;
	GetClientRect(appWindow->GetHwnd(), &rect);
	Renderer::Resize(rect.right - rect.left, rect.bottom - rect.top);
}

void App::SetMode(const Mode newMode)
{
	switch (newMode)
	{
	case Mode::Standalone:
		UnRegisterHotkeys();
		appWindow->SetStyle(Window::Styles::Standalone);
		mode = Mode::Standalone;
		break;
	case Mode::Overlay:
		RegisterHotkeys();
		appWindow->SetStyle(Window::Styles::Overlay);
		SetForegroundWindow(ErectusProcess::hWnd);
		mode = Mode::Overlay;
		break;
	case Mode::Attached:
		if (!SnapToWindow(ErectusProcess::hWnd))
		{
			Detach();
			return;
		}
		RegisterHotkeys();
		appWindow->SetStyle(Window::Styles::Attached);
		SetForegroundWindow(appWindow->GetHwnd());
		mode = Mode::Attached;
		break;
	}
}

void App::Attach(const DWORD pid)
{
	if (!ErectusProcess::AttachToProcess(pid))
	{
		Detach();
		return;
	}
	SetMode(Mode::Attached);
}

void App::Detach()
{
	while (!Threads::ThreadDestruction() && Threads::threadDestructionCounter < 900)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	ErectusProcess::ResetProcessData();

	SetMode(Mode::Standalone);
}

void App::OnHotkey(const WPARAM hotkeyId)
{
	switch (hotkeyId)
	{
	case (static_cast<int>(HotKeys::PositionSpoofingToggle)):
		if (Settings::localPlayer.positionSpoofingEnabled)
			Threads::positionSpoofingToggle = !Threads::positionSpoofingToggle;
		break;
	case (static_cast<int>(HotKeys::NoclipToggle)):
		if (Settings::localPlayer.noclipEnabled)
			Threads::noclipToggle = !Threads::noclipToggle;
		break;
	case (static_cast<int>(HotKeys::OpkNpcsToggle)):
		if (Settings::opk.enabled)
			Threads::opkNpcsToggle = !Threads::opkNpcsToggle;
		break;
	case (static_cast<int>(HotKeys::Loot)):
		if (Settings::looter.mode == LooterSettings::Mode::Keybind)
			Looter::RequestLootItems();
		break;
	case (static_cast<int>(HotKeys::ToggleOverlay)):
		ToggleOverlay();
		break;
	default:
		break;
	}
}

void App::UnRegisterHotkeys()
{
	if (!gApp || !gApp->appWindow)
		return;

	for (const auto& [hotkeyId, hotkey] : HOTKEYS)
	{
		UnregisterHotKey(gApp->appWindow->GetHwnd(), static_cast<int>(hotkeyId));
	}
}
void App::RegisterHotkeys()
{
	for (const auto& [hotkeyId, hotkey] : HOTKEYS)
	{
		RegisterHotKey(gApp->appWindow->GetHwnd(), static_cast<int>(hotkeyId), hotkey.modifiers, hotkey.vk);
	}
}

void App::ToggleOverlay()
{
	switch (mode)
	{
	case Mode::Attached:
		SetMode(Mode::Overlay);
		break;
	case Mode::Overlay:
		SetMode(Mode::Attached);
		break;
	default:
		break;
	}
}
void App::Update()
{
	if (mode != Mode::Standalone && !IsWindow(ErectusProcess::hWnd))
	{
		Detach();
		return;
	}

	if (mode != Mode::Standalone)
		Threads::CreateProcessThreads();

	if (Threads::threadDestructionQueued)
	{
		if (!Threads::ThreadDestruction())
		{
			Threads::threadDestructionCounter++;
			if (Threads::threadDestructionCounter > 900)
			{
				Threads::threadDestructionCounter = 0;
				gApp->Shutdown();
			}
		}
	}
}

void App::Render() const
{
	Renderer::BeginFrame();
	if (mode == Mode::Attached)
		Renderer::d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(128, 0, 0, 0), 1.0f, 0);

	if (mode == Mode::Overlay)
		Renderer::d3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	Gui::Render();
	Renderer::EndFrame();
}

bool App::SnapToWindow(const HWND hwnd) const
{
	RECT targetClientRect;
	if (!GetClientRect(hwnd, &targetClientRect) || IsRectEmpty(&targetClientRect))
		return false;

	appWindow->SetSize(targetClientRect.right - targetClientRect.left, targetClientRect.bottom - targetClientRect.top);

	ClientToScreen(hwnd, reinterpret_cast<POINT*>(&targetClientRect));
	appWindow->SetPosition(targetClientRect.left, targetClientRect.top);

	return true;
}
