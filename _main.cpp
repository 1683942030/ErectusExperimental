#include "ErectusInclude.h"
#include "settings.h"

void onShutdown() {
	ErectusProcess::ResetProcessData();
	
	Renderer::Cleanup();
	ErectusImGui::ImGuiCleanup();

	Settings::WriteIniSettings();

	ErectusMain::CloseWnd();
}

int onStartup(const HINSTANCE hInstance) {
	ErectusMain::CreateWnd(hInstance);

	ShowWindow(ErectusMain::appHwnd, SW_SHOW);

	if (!Renderer::Init())
	{
		onShutdown();
		return 3;
	}

	if (!ErectusImGui::ImGuiInitialize())
	{
		onShutdown();
		return 4;
	}

	Settings::ReadIniSettings();

	ErectusProcess::SetProcessMenu();
	return 0;
}


// ReSharper disable once CppInconsistentNaming
// ReSharper disable once CppParameterMayBeConst
int WINAPI wWinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] PWSTR lpCmdLine, [[maybe_unused]] int nShowCmd)
{
	int result;

	if (((result = onStartup(hInstance))) && result != 0)
		return result;

	ErectusMain::RegisterHotkeys();
	
	MSG overlayMsg;
	while (GetMessage(&overlayMsg, nullptr, 0, 0))
	{
		TranslateMessage(&overlayMsg);
		DispatchMessage(&overlayMsg);
	}

	onShutdown();

	return result;
}
