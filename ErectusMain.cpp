#include "ErectusInclude.h"
#include "resource.h"

// ReSharper disable once CppInconsistentNaming
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// ReSharper disable once CppParameterMayBeConst
// ReSharper disable once CppParameterMayBeConst
LRESULT CALLBACK ErectusMain::WndCallback(HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return 1;

	switch (uMsg)
	{
	case WM_HOTKEY:
		OnHotkey(wParam);
	case WM_KEYDOWN:
		return 0;
	case WM_PAINT:
		Render();
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void ErectusMain::OnHotkey(const WPARAM hotkeyId)
{
	switch (hotkeyId)
	{
	case (static_cast<int>(HotKeys::ContainerLooterToggle)):
		ErectusIni::containerLooterSettings.entityLooterEnabled = !ErectusIni::containerLooterSettings.entityLooterEnabled;
		break;
	case (static_cast<int>(HotKeys::NpcLooterToggle)):
		ErectusIni::npcLooterSettings.entityLooterEnabled = !ErectusIni::npcLooterSettings.entityLooterEnabled;
		break;
	case (static_cast<int>(HotKeys::HarvesterToggle)):
		ErectusIni::customHarvesterSettings.harvesterEnabled = !ErectusIni::customHarvesterSettings.harvesterEnabled;
		break;
	case (static_cast<int>(HotKeys::PositionSpoofingToggle)):
		if (ErectusIni::customLocalPlayerSettings.positionSpoofingEnabled)
			ErectusThread::positionSpoofingToggle = !ErectusThread::positionSpoofingToggle;
		break;
	case (static_cast<int>(HotKeys::NoclipToggle)):
		if (ErectusIni::customLocalPlayerSettings.noclipEnabled)
			ErectusThread::noclipToggle = !ErectusThread::noclipToggle;
		break;
	case (static_cast<int>(HotKeys::OpkPlayersToggle)):
		if (ErectusIni::customOpkSettings.playersEnabled)
			ErectusThread::opkPlayersToggle = !ErectusThread::opkPlayersToggle;
		break;
	case (static_cast<int>(HotKeys::OpkNpcsToggle)):
		if (ErectusIni::customOpkSettings.npcsEnabled)
			ErectusThread::opkNpcsToggle = !ErectusThread::opkNpcsToggle;
		break;
	case (static_cast<int>(HotKeys::LootItems)):
		if (ErectusIni::customItemLooterSettings.itemKeybindEnabled)
			ErectusThread::RequestLootItems();
		break;
	case (static_cast<int>(HotKeys::LootScrap)):
		if (ErectusIni::customScrapLooterSettings.scrapKeybindEnabled)
			ErectusThread::RequestLootScrap();
		break;
	case (static_cast<int>(HotKeys::ToggleOverlay)):
		ToggleOverlay();
		break;
	default:
		break;
	}
}

void ErectusMain::RegisterHotkeys()
{
	for (auto item : HOTKEYS)
	{
		::RegisterHotKey(appHwnd, static_cast<int>(item.first), item.second.modifiers, item.second.vk);
	}
}

void ErectusMain::ToggleOverlay()
{
	if (!ErectusProcess::processSelected)
		return;

	if (overlayMenuActive)
		SetOverlayPosition(false, true);
	else
		SetOverlayMenu();
}
void ErectusMain::Render() {
	switch (Renderer::BeginScene()) {
	case 0: //OK
		break;
	case 1: //TRY LATER
		return;
	case 99: //FATAL ERROR
	default:
		CloseWnd();
	}

	//render
	RenderProcessMenu();
	RenderOverlayMenu();
	RenderOverlay();

	Renderer::EndScene();

	if (ErectusProcess::processSelected)
	{
		ErectusProcess::processValidCounter++;
		if (ErectusProcess::processValidCounter > 20)
		{
			ErectusProcess::processValidCounter = 0;
			if (WaitForSingleObject(ErectusProcess::handle, 0) != WAIT_TIMEOUT)
			{
				ErectusProcess::ResetProcessData();
			}

			if (overlayActive)
			{
				if (ErectusProcess::hWnd == GetForegroundWindow())
				{
					overlayForeground = true;
					if (!SetOverlayPosition(true, true))
						ErectusProcess::SetProcessMenu();
				}
				else
				{
					overlayForeground = false;
					if (!SetOverlayPosition(false, false))
						ErectusProcess::SetProcessMenu();
				}
			}
		}

		if (!ErectusThread::threadCreationState)
			ErectusThread::threadCreationState = ErectusThread::CreateProcessThreads();
	}

	if (ErectusThread::threadDestructionQueued)
	{
		if (!ErectusThread::ThreadDestruction())
		{
			ErectusThread::threadDestructionCounter++;
			if (ErectusThread::threadDestructionCounter > 900)
			{
				ErectusThread::threadDestructionCounter = 0;
				CloseWnd();
			}
		}
	}

	//ghetto run once per frame
	std::this_thread::sleep_for(std::chrono::milliseconds(16));
}

void ErectusMain::RenderProcessMenu() {
	if (!ErectusProcess::processMenuActive)
		return;

	ErectusImGui::ProcessMenu();
}

void ErectusMain::RenderOverlayMenu() {
	if (!overlayMenuActive)
		return;

	ErectusImGui::OverlayMenu();
}

void ErectusMain::RenderOverlay()
{
	if (!overlayActive)
		return;

	Renderer::d3DxSprite->Begin(D3DXSPRITE_ALPHABLEND);

	ErectusMemory::targetLockingValid = false;
	ErectusMemory::targetLockingClosestDegrees = ErectusIni::customTargetSettings.lockingFov;
	ErectusMemory::targetLockingClosestPtr = 0;

	ErectusMemory::RenderCustomEntityList();
	ErectusMemory::RenderCustomNpcList();
	ErectusMemory::RenderCustomPlayerList();

	if (!ErectusMemory::targetLockingValid)
	{
		if (ErectusMemory::targetLockingPtr)
		{
			ErectusMemory::targetLockingCooldown = ErectusIni::customTargetSettings.retargeting ? ErectusIni::customTargetSettings.cooldown : -1;
			ErectusMemory::targetLockingPtr = 0;
		}
		else if (ErectusMemory::targetLockingClosestDegrees < ErectusIni::customTargetSettings.lockingFov)
		{
			ErectusMemory::targetLockingCooldown = 0;
			ErectusMemory::targetLockingPtr = ErectusMemory::targetLockingClosestPtr;
		}
	}
	ErectusMemory::RenderData();

	Renderer::d3DxSprite->End();
}

void ErectusMain::SetOverlayMenu()
{
	if (windowSize[0] != 480 || windowSize[1] != 480)
	{
		windowSize[0] = 480;
		windowSize[1] = 720;

		Renderer::deviceResetQueued = true;
		SetWindowPos(appHwnd, HWND_NOTOPMOST, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);
	}

	int bufferPosition[2];
	bufferPosition[0] = GetSystemMetrics(SM_CXSCREEN) / 2 - windowSize[0] / 2;
	bufferPosition[1] = GetSystemMetrics(SM_CYSCREEN) / 2 - windowSize[1] / 2;

	if (windowPosition[0] != bufferPosition[0] || windowPosition[1] != bufferPosition[1])
	{
		windowPosition[0] = bufferPosition[0];
		windowPosition[1] = bufferPosition[1];

		MoveWindow(appHwnd, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], FALSE);
		if (!Renderer::deviceResetQueued)
			SetWindowPos(appHwnd, HWND_NOTOPMOST, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);
	}

	auto style = GetWindowLongPtr(appHwnd, GWL_EXSTYLE);

	if (style & WS_EX_LAYERED)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLongPtr(appHwnd, GWL_EXSTYLE, style);
	}

	if (style & WS_EX_TOPMOST)
		SetWindowPos(appHwnd, HWND_NOTOPMOST, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);

	ErectusProcess::processMenuActive = false;
	overlayMenuActive = true;
	overlayActive = false;
}

int ErectusMain::CreateWnd(const HINSTANCE hInstance)
{
	mHInstance = hInstance;

	WNDCLASSEX wndClass{
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_VREDRAW | CS_HREDRAW,
		.lpfnWndProc = WndCallback,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = mHInstance,
		.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)),
		.hCursor = nullptr,
		.hbrBackground = CreateSolidBrush(RGB(0x00, 0x00, 0x00)),
		.lpszMenuName = nullptr,
		.lpszClassName = OVERLAY_WINDOW_CLASS,
		.hIconSm = nullptr
	};

	if (!RegisterClassEx(&wndClass))
	{
		return 1;
	}

	windowSize[0] = 384;
	windowSize[1] = 224;
	windowPosition[0] = GetSystemMetrics(SM_CXSCREEN) / 2 - windowSize[0] / 2;
	windowPosition[1] = GetSystemMetrics(SM_CYSCREEN) / 2 - windowSize[1] / 2;
	appHwnd = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, wndClass.lpszClassName, OVERLAY_WINDOW_NAME, WS_POPUP, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], nullptr, nullptr, wndClass.hInstance, nullptr);

	if (appHwnd == nullptr)
	{
		UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
		return 2;
	}

	MARGINS overlayMargins = { -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea(appHwnd, &overlayMargins);
	SetLayeredWindowAttributes(appHwnd, RGB(0x00, 0x00, 0x00), 0xFF, LWA_ALPHA);
	SetWindowLongPtr(appHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT);

	return 0;
}

void ErectusMain::CloseWnd() {
	if (appHwnd != nullptr)
	{
		SendMessage(appHwnd, WM_CLOSE, 0, 0);
	}
	UnregisterClass(OVERLAY_WINDOW_CLASS, mHInstance);
}

bool ErectusMain::SetOverlayPosition(const bool topmost, const bool layered)
{
	RECT windowRect;
	RECT clientRect;

	if (!ErectusProcess::HwndValid(ErectusProcess::pid) || !GetWindowRect(ErectusProcess::hWnd, &windowRect) || !GetClientRect(ErectusProcess::hWnd, &clientRect))
	{
		overlayActive = false;
		return false;
	}

	unsigned int size[2];
	size[0] = clientRect.right;
	size[1] = clientRect.bottom;

	int position[2];
	position[0] = windowRect.left - (clientRect.right + windowRect.left - windowRect.right) / 2;
	position[1] = windowRect.top - (clientRect.bottom + windowRect.top - windowRect.bottom) / 2;

	if (GetWindowLongPtr(ErectusProcess::hWnd, GWL_STYLE) & WS_BORDER)
	{
		auto buffer = GetSystemMetrics(SM_CYCAPTION) / 2;
		buffer += buffer & 1;
		position[1] += buffer;
	}

	if (GetMenu(ErectusProcess::hWnd) != nullptr)
	{
		auto buffer = GetSystemMetrics(SM_CYMENU) / 2;
		buffer += buffer & 1;
		position[1] += buffer;
	}

	if (position[0] != windowPosition[0] || position[1] != windowPosition[1])
	{
		windowPosition[0] = position[0];
		windowPosition[1] = position[1];
		MoveWindow(appHwnd, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);
	}

	if (size[0] != windowSize[0] || size[1] != windowSize[1])
	{
		windowSize[0] = size[0];
		windowSize[1] = size[1];
		Renderer::deviceResetQueued = true;
	}

	if (topmost || layered)
	{
		auto style = GetWindowLongPtr(appHwnd, GWL_EXSTYLE);

		if (topmost && !(style & WS_EX_TOPMOST))
		{
			SetWindowPos(appHwnd, HWND_TOPMOST, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);
			windowTopmostCounter++;
			if (windowTopmostCounter > 3)
			{
				windowTopmostCounter = 0;
				ErectusProcess::SetProcessError(0, "Process State: Overlay not topmost");
				overlayActive = false;
				return false;
			}
		}
		else if (!topmost && style & WS_EX_TOPMOST)
			SetWindowPos(appHwnd, HWND_NOTOPMOST, windowPosition[0], windowPosition[1], windowSize[0], windowSize[1], 0);
		else
			windowTopmostCounter = 0;

		if (layered && !(style & WS_EX_LAYERED))
		{
			style |= WS_EX_LAYERED;
			SetWindowLongPtr(appHwnd, GWL_EXSTYLE, style);
			if (experimentalOverlayFix)
				SetLayeredWindowAttributes(appHwnd, RGB(0x00, 0x00, 0x00), 0xFF, LWA_ALPHA | LWA_COLORKEY);
			else
				SetLayeredWindowAttributes(appHwnd, RGB(0x00, 0x00, 0x00), 0xFF, LWA_ALPHA);
		}
		else if (!layered && style & WS_EX_LAYERED)
		{
			style &= ~WS_EX_LAYERED;
			SetWindowLongPtr(appHwnd, GWL_EXSTYLE, style);
		}
	}

	ErectusProcess::processMenuActive = false;
	overlayMenuActive = false;
	overlayActive = true;
	return true;
}
