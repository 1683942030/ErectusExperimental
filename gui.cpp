#include "gui.h"

#include "app.h"
#include "settings.h"
#include "utils.h"
#include "renderer.h"

#include "ErectusProcess.h"
#include "ErectusMemory.h"

#include "fmt/format.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"




void Gui::ProcessMenu()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(App::windowSize[0]), static_cast<float>(App::windowSize[1])));
	ImGui::SetNextWindowCollapsed(false);

	if (ImGui::Begin("Erectus - Process Menu", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Exit"))
				App::CloseWnd();
			if (ImGui::MenuItem("Overlay Menu"))
				App::SetOverlayMenu();
			if (!ErectusProcess::pid)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);

				ImGui::MenuItem("Overlay");

				ImGui::PopStyleVar();
				ImGui::PopItemFlag();
			}
			else
			{
				if (ImGui::MenuItem("Overlay Menu"))
					App::SetOverlayMenu();
				if (ImGui::MenuItem("Overlay"))
					App::SetOverlayPosition(false, true);
			}
			ImGui::EndMenuBar();
		}

		ImGui::SetNextItemWidth(-16.f);

		auto processText = ErectusProcess::pid ? "Fallout76.exe - " + std::to_string(ErectusProcess::pid) : "No  process selected.";
		if (ImGui::BeginCombo("###ProcessList", processText.c_str()))
		{
			for (auto item : ErectusProcess::GetProcesses())
			{
				processText = item ? "Fallout76.exe - " + std::to_string(item) : "NONE";
				if (ImGui::Selectable(processText.c_str()))
					ErectusProcess::AttachToProcess(item);
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();
		switch (ErectusProcess::processErrorId)
		{
		case 0:
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
			break;
		case 1:
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
			break;
		case 2:
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ErectusProcess::processError.c_str());
			break;
		default:
			ImGui::Text(ErectusProcess::processError.c_str());
			break;
		}

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::Columns(2);
		ImGui::Separator();
		ImGui::Text("Overlay Menu Keybind");
		ImGui::NextColumn();
		ImGui::Text("CTRL+ENTER");
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("ImGui (D3D9) FPS");
		ImGui::NextColumn();
		ImGui::Text("%.1f", ImGui::GetIO().Framerate);
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("PID (Process Id)");
		ImGui::NextColumn();
		ImGui::Text("%lu", ErectusProcess::pid);
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("HWND (Window)");
		ImGui::NextColumn();
		ImGui::Text("%p", ErectusProcess::hWnd);
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("Base Address");
		ImGui::NextColumn();
		ImGui::Text("%016llX", ErectusProcess::exe);
		ImGui::NextColumn();
		ImGui::Separator();
		ImGui::Text("HANDLE");
		ImGui::NextColumn();
		ImGui::Text("%p", ErectusProcess::handle);
		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopItemFlag();
	}
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Gui::ButtonToggle(const char* label, bool* state)
{
	if (*state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(224.0f, 0.0f)))
			*state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(224.0f, 0.0f)))
			*state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::LargeButtonToggle(const char* label, bool* state)
{
	if (*state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(451.0f, 0.0f)))
			*state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(451.0f, 0.0f)))
			*state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::SmallButtonToggle(const char* label, bool* state)
{
	if (*state)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(110.0f, 0.0f)))
			*state = false;
		ImGui::PopStyleColor(3);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
		if (ImGui::Button(label, ImVec2(110.0f, 0.0f)))
			*state = true;
		ImGui::PopStyleColor(3);
	}
}

void Gui::OverlayMenuTabEsp()
{
	if (ImGui::BeginTabItem("ESP###ESPTab"))
	{
		if (ImGui::CollapsingHeader("Player ESP Settings"))
		{
			ButtonToggle("Player ESP Enabled", &Settings::playerSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###PlayerSettingsEnabledDistance", &Settings::playerSettings.enabledDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Draw Living Players", &Settings::playerSettings.drawAlive);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###PlayerSettingsAliveColor", Settings::playerSettings.aliveColor);
			Utils::ValidateRgb(Settings::playerSettings.aliveColor);

			ButtonToggle("Draw Downed Players", &Settings::playerSettings.drawDowned);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###PlayerSettingsDownedColor", Settings::playerSettings.downedColor);
			Utils::ValidateRgb(Settings::playerSettings.downedColor);

			ButtonToggle("Draw Dead Players", &Settings::playerSettings.drawDead);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###PlayerSettingsDeadColor", Settings::playerSettings.deadColor);
			Utils::ValidateRgb(Settings::playerSettings.deadColor);

			ButtonToggle("Draw Unknown Players", &Settings::playerSettings.drawUnknown);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###PlayerSettingsUnknownColor", Settings::playerSettings.unknownColor);
			Utils::ValidateRgb(Settings::playerSettings.unknownColor);

			ButtonToggle("Draw Enabled Players", &Settings::playerSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###PlayerSettingsEnabledAlpha", &Settings::playerSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Players", &Settings::playerSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###PlayerSettingsDisabledAlpha", &Settings::playerSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Players", &Settings::playerSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Players", &Settings::playerSettings.drawUnnamed);

			ButtonToggle("Show Player Name", &Settings::playerSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Player Distance", &Settings::playerSettings.showDistance);

			ButtonToggle("Show Player Health", &Settings::playerSettings.showHealth);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Dead Player Health", &Settings::playerSettings.showDeadHealth);

			ButtonToggle("Player Text Shadowed", &Settings::playerSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Player Text Centered", &Settings::playerSettings.textCentered);
		}

		if (ImGui::CollapsingHeader("NPC ESP Settings"))
		{
			ButtonToggle("NPC ESP Enabled", &Settings::npcSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###NpcSettingsEnabledDistance", &Settings::npcSettings.enabledDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Draw Living NPCs", &Settings::npcSettings.drawAlive);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###NpcSettingsAliveColor", Settings::npcSettings.aliveColor);
			Utils::ValidateRgb(Settings::npcSettings.aliveColor);

			ButtonToggle("Draw Downed NPCs", &Settings::npcSettings.drawDowned);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###NpcSettingsDownedColor", Settings::npcSettings.downedColor);
			Utils::ValidateRgb(Settings::npcSettings.downedColor);

			ButtonToggle("Draw Dead NPCs", &Settings::npcSettings.drawDead);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###NpcSettingsDeadColor", Settings::npcSettings.deadColor);
			Utils::ValidateRgb(Settings::npcSettings.deadColor);

			ButtonToggle("Draw Unknown NPCs", &Settings::npcSettings.drawUnknown);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###NpcSettingsUnknownColor", Settings::npcSettings.unknownColor);
			Utils::ValidateRgb(Settings::npcSettings.unknownColor);

			ButtonToggle("Draw Enabled NPCs", &Settings::npcSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###NpcSettingsEnabledAlpha", &Settings::npcSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled NPCs", &Settings::npcSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###NpcSettingsDisabledAlpha", &Settings::npcSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named NPCs", &Settings::npcSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed NPCs", &Settings::npcSettings.drawUnnamed);

			ButtonToggle("Show NPC Name", &Settings::npcSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show NPC Distance", &Settings::npcSettings.showDistance);

			ButtonToggle("Show NPC Health", &Settings::npcSettings.showHealth);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Dead NPC Health", &Settings::npcSettings.showDeadHealth);

			ButtonToggle("NPC Text Shadowed", &Settings::npcSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("NPC Text Centered", &Settings::npcSettings.textCentered);

			ButtonToggle("Always Draw Living 1* NPCs", &Settings::customLegendarySettings.overrideLivingOneStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###LivingOneStarColor", Settings::customLegendarySettings.livingOneStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.livingOneStarColor);

			ButtonToggle("Always Draw Dead 1* NPCs", &Settings::customLegendarySettings.overrideDeadOneStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###DeadOneStarColor", Settings::customLegendarySettings.deadOneStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.deadOneStarColor);

			ButtonToggle("Always Draw Living 2* NPCs", &Settings::customLegendarySettings.overrideLivingTwoStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###LivingTwoStarColor", Settings::customLegendarySettings.livingTwoStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.livingTwoStarColor);

			ButtonToggle("Always Draw Dead 2* NPCs", &Settings::customLegendarySettings.overrideDeadTwoStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###DeadTwoStarColor", Settings::customLegendarySettings.deadTwoStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.deadTwoStarColor);

			ButtonToggle("Always Draw Living 3* NPCs", &Settings::customLegendarySettings.overrideLivingThreeStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###LivingThreeStarColor", Settings::customLegendarySettings.livingThreeStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.livingThreeStarColor);

			ButtonToggle("Always Draw Dead 3* NPCs", &Settings::customLegendarySettings.overrideDeadThreeStar);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###DeadThreeStarColor", Settings::customLegendarySettings.deadThreeStarColor);
			Utils::ValidateRgb(Settings::customLegendarySettings.deadThreeStarColor);

			LargeButtonToggle("Hide NPCs in the Settler Faction", &Settings::customExtraNpcSettings.hideSettlerFaction);
			LargeButtonToggle("Hide NPCs in the Crater Raider Faction", &Settings::customExtraNpcSettings.hideCraterRaiderFaction);
			LargeButtonToggle("Hide NPCs in the Diehards Faction", &Settings::customExtraNpcSettings.hideDieHardFaction);
			LargeButtonToggle("Hide NPCs in the Secret Service Faction", &Settings::customExtraNpcSettings.hideSecretServiceFaction);

			LargeButtonToggle("NPC Blacklist Enabled", &Settings::customExtraNpcSettings.useNpcBlacklist);
			if (ImGui::CollapsingHeader("NPC Blacklist"))
			{
				for (auto i = 0; i < 64; i++)
				{
					auto toggleLabel = fmt::format("NPC Blacklist: {:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::customExtraNpcSettings.npcBlacklistEnabled[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###NPCBlacklist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::customExtraNpcSettings.npcBlacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Container ESP Settings"))
		{
			ButtonToggle("Container ESP Enabled", &Settings::containerSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ContainerSettingsEnabledDistance", &Settings::containerSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###ContainerSettingsColor", Settings::containerSettings.color);
			Utils::ValidateRgb(Settings::containerSettings.color);

			ButtonToggle("Draw Enabled Containers", &Settings::containerSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###ContainerSettingsEnabledAlpha", &Settings::containerSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Containers", &Settings::containerSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###ContainerSettingsDisabledAlpha", &Settings::containerSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Containers", &Settings::containerSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Containers", &Settings::containerSettings.drawUnnamed);

			ButtonToggle("Show Container Name", &Settings::containerSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Container Distance", &Settings::containerSettings.showDistance);

			ButtonToggle("Container Text Shadowed", &Settings::containerSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Container Text Centered", &Settings::containerSettings.textCentered);

			if (ImGui::CollapsingHeader("Container Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Container Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::containerSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ContainerWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::containerSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Junk ESP Settings"))
		{
			ButtonToggle("Junk ESP Enabled", &Settings::junkSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###JunkSettingsEnabledDistance", &Settings::junkSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###JunkSettingsColor", Settings::junkSettings.color);
			Utils::ValidateRgb(Settings::junkSettings.color);

			ButtonToggle("Draw Enabled Junk", &Settings::junkSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###JunkSettingsEnabledAlpha", &Settings::junkSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			Utils::ValidateFloat(Settings::junkSettings.enabledAlpha, 0.0f, 1.0f);

			ButtonToggle("Draw Disabled Junk", &Settings::junkSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###JunkSettingsDisabledAlpha", &Settings::junkSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Junk", &Settings::junkSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Junk", &Settings::junkSettings.drawUnnamed);

			ButtonToggle("Show Junk Name", &Settings::junkSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Junk Distance", &Settings::junkSettings.showDistance);

			ButtonToggle("Junk Text Shadowed", &Settings::junkSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Junk Text Centered", &Settings::junkSettings.textCentered);

			if (ImGui::CollapsingHeader("Junk Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Junk Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::junkSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###JunkWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::junkSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Plan ESP Settings"))
		{
			ButtonToggle("Plan ESP Enabled", &Settings::planSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###PlanSettingsEnabledDistance", &Settings::planSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###PlanSettingsColor", Settings::planSettings.color);
			Utils::ValidateRgb(Settings::planSettings.color);

			ButtonToggle("Draw Enabled Plans", &Settings::planSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###PlanSettingsEnabledAlpha", &Settings::planSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Plans", &Settings::planSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###PlanSettingsDisabledAlpha", &Settings::planSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Known Plans", &Settings::customKnownRecipeSettings.knownRecipesEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unknown Plans", &Settings::customKnownRecipeSettings.unknownRecipesEnabled);

			ButtonToggle("Draw Named Plans", &Settings::planSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Plans", &Settings::planSettings.drawUnnamed);

			ButtonToggle("Show Plan Name", &Settings::planSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Plan Distance", &Settings::planSettings.showDistance);

			ButtonToggle("Plan Text Shadowed", &Settings::planSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Plan Text Centered", &Settings::planSettings.textCentered);

			if (ImGui::CollapsingHeader("Plan Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Plan Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::planSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###PlanWhitelis{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::planSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Magazine ESP Settings"))
		{
			ButtonToggle("Magazine ESP Enabled", &Settings::magazineSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###MagazineSettingsEnabledDistance", &Settings::magazineSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###MagazineSettingsColor", Settings::magazineSettings.color);
			Utils::ValidateRgb(Settings::magazineSettings.color);

			ButtonToggle("Draw Enabled Magazines", &Settings::magazineSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###MagazineSettingsEnabledAlpha", &Settings::magazineSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Magazines", &Settings::magazineSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###MagazineSettingsDisabledAlpha", &Settings::magazineSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Magazines", &Settings::magazineSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Magazines", &Settings::magazineSettings.drawUnnamed);

			ButtonToggle("Show Magazine Name", &Settings::magazineSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Magazine Distance", &Settings::magazineSettings.showDistance);

			ButtonToggle("Magazine Text Shadowed", &Settings::magazineSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Magazine Text Centered", &Settings::magazineSettings.textCentered);

			if (ImGui::CollapsingHeader("Magazine Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Magazine Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::magazineSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###MagazineWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::magazineSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Bobblehead ESP Settings"))
		{
			ButtonToggle("Bobblehead ESP Enabled", &Settings::bobbleheadSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###BobbleheadSettingsEnabledDistance", &Settings::bobbleheadSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###BobbleheadSettingsColor", Settings::bobbleheadSettings.color);
			Utils::ValidateRgb(Settings::bobbleheadSettings.color);

			ButtonToggle("Draw Enabled Bobbleheads", &Settings::bobbleheadSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###BobbleheadSettingsEnabledAlpha", &Settings::bobbleheadSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Bobbleheads", &Settings::bobbleheadSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###BobbleheadSettingsDisabledAlpha", &Settings::bobbleheadSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Bobbleheads", &Settings::bobbleheadSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Bobbleheads", &Settings::bobbleheadSettings.drawUnnamed);

			ButtonToggle("Show Bobblehead Name", &Settings::bobbleheadSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Bobblehead Distance", &Settings::bobbleheadSettings.showDistance);

			ButtonToggle("Bobblehead Text Shadowed", &Settings::bobbleheadSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Bobblehead Text Centered", &Settings::bobbleheadSettings.textCentered);

			if (ImGui::CollapsingHeader("Bobblehead Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Bobblehead Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::bobbleheadSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###BobbleheadWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::bobbleheadSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Item ESP Settings"))
		{
			ButtonToggle("Item ESP Enabled", &Settings::itemSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemSettingsEnabledDistance", &Settings::itemSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###ItemSettingsColor", Settings::itemSettings.color);
			Utils::ValidateRgb(Settings::itemSettings.color);

			ButtonToggle("Draw Enabled Items", &Settings::itemSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###ItemSettingsEnabledAlpha", &Settings::itemSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Items", &Settings::itemSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###ItemSettingsDisabledAlpha", &Settings::itemSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Items", &Settings::itemSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Items", &Settings::itemSettings.drawUnnamed);

			ButtonToggle("Show Item Name", &Settings::itemSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Item Distance", &Settings::itemSettings.showDistance);

			ButtonToggle("Item Text Shadowed", &Settings::itemSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Item Text Centered", &Settings::itemSettings.textCentered);

			if (ImGui::CollapsingHeader("Item Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Item Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::itemSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ItemWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::itemSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Flora ESP Settings"))
		{
			ButtonToggle("Flora ESP Enabled", &Settings::floraSettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###FloraSettingsEnabledDistance", &Settings::floraSettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###FloraSettingsColor", Settings::floraSettings.color);
			Utils::ValidateRgb(Settings::floraSettings.color);

			ButtonToggle("Draw Enabled Flora", &Settings::floraSettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###FloraSettingsEnabledAlpha", &Settings::floraSettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Flora", &Settings::floraSettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###FloraSettingsDisabledAlpha", &Settings::floraSettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Flora", &Settings::floraSettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Flora", &Settings::floraSettings.drawUnnamed);

			LargeButtonToggle("Draw Raw Crimson Flux Yielding Flora", &Settings::customFluxSettings.crimsonFluxEnabled);
			LargeButtonToggle("Draw Raw Cobalt Flux Yielding Flora", &Settings::customFluxSettings.cobaltFluxEnabled);
			LargeButtonToggle("Draw Raw Yellowcake Flux Yielding Flora", &Settings::customFluxSettings.yellowcakeFluxEnabled);
			LargeButtonToggle("Draw Raw Fluorescent Flux Yielding Flora", &Settings::customFluxSettings.fluorescentFluxEnabled);
			LargeButtonToggle("Draw Raw Violet Flux Yielding Flora", &Settings::customFluxSettings.violetFluxEnabled);

			ButtonToggle("Show Flora Name", &Settings::floraSettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Flora Distance", &Settings::floraSettings.showDistance);

			ButtonToggle("Flora Text Shadowed", &Settings::floraSettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Flora Text Centered", &Settings::floraSettings.textCentered);

			if (ImGui::CollapsingHeader("Flora Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Flora Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::floraSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###FloraWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::floraSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Entity ESP Settings"))
		{
			ButtonToggle("Entity ESP Enabled", &Settings::entitySettings.enabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###EntitySettingsEnabledDistance", &Settings::entitySettings.enabledDistance, 0, 3000, "Distance: %d");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::ColorEdit3("###EntitySettingsColor", Settings::entitySettings.color);
			Utils::ValidateRgb(Settings::entitySettings.color);

			ButtonToggle("Draw Enabled Entities", &Settings::entitySettings.drawEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###EntitySettingsEnabledAlpha", &Settings::entitySettings.enabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Disabled Entities", &Settings::entitySettings.drawDisabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###EntitySettingsDisabledAlpha", &Settings::entitySettings.disabledAlpha, 0.0f, 1.0f, "Alpha: %.2f");

			ButtonToggle("Draw Named Entities", &Settings::entitySettings.drawNamed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Unnamed Entities", &Settings::entitySettings.drawUnnamed);

			ButtonToggle("Show Entity Name", &Settings::entitySettings.showName);
			ImGui::SameLine(235.0f);
			ButtonToggle("Show Entity Distance", &Settings::entitySettings.showDistance);

			ButtonToggle("Entity Text Shadowed", &Settings::entitySettings.textShadowed);
			ImGui::SameLine(235.0f);
			ButtonToggle("Entity Text Centered", &Settings::entitySettings.textCentered);

			if (ImGui::CollapsingHeader("Entity Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Entity Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::floraSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###EntityWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::entitySettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabLoot()
{
	if (ImGui::BeginTabItem("Loot###LootTab"))
	{
		if (ImGui::CollapsingHeader("Scrap Looter"))
		{
			if (ErectusMemory::CheckScrapList())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				if (ImGui::Button("Loot Scrap (Keybind: CTRL+E)###LootSelectedScrapEnabled", ImVec2(224.0f, 0.0f)))
					ErectusMemory::LootScrap();
				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
				ImGui::Button("Loot Scrap (Keybind: CTRL+E)###LootSelectedScrapDisabled", ImVec2(224.0f, 0.0f));
				ImGui::PopStyleColor(3);
				ImGui::PopItemFlag();
			}

			ImGui::SameLine(235.0f);
			ButtonToggle("Scrap Looter Keybind Enabled", &Settings::scrapLooter.keybindEnabled);

			LargeButtonToggle("Scrap Looter ESP Override (Uses Junk ESP Settings)", &Settings::scrapLooter.scrapOverrideEnabled);

			ButtonToggle("Automatic Looting Enabled###ScrapAutomaticLootingEnabled", &Settings::scrapLooter.autoLootingEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Automatic Looting Status###ScrapAutomaticStatus", &Settings::scrapLooter.drawStatus);


			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Speed (Min): {0:d} ({1:d} ms)", Settings::scrapLooter.autoLootingSpeedMin, Settings::scrapLooter.autoLootingSpeedMin * 16);
				if (ImGui::SliderInt("###ScrapAutomaticSpeedMin", &Settings::scrapLooter.autoLootingSpeedMin, 10, 60, sliderText.c_str()))
				{
					if (Settings::scrapLooter.autoLootingSpeedMax < Settings::scrapLooter.autoLootingSpeedMin)
						Settings::scrapLooter.autoLootingSpeedMax = Settings::scrapLooter.autoLootingSpeedMin;
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Speed (Max): {0:d} ({1:d} ms)", Settings::scrapLooter.autoLootingSpeedMax, Settings::scrapLooter.autoLootingSpeedMax * 16);
				if (ImGui::SliderInt("###ScrapAutomaticSpeedMax", &Settings::scrapLooter.autoLootingSpeedMax, 10, 60, sliderText.c_str()))
				{
					if (Settings::scrapLooter.autoLootingSpeedMax < Settings::scrapLooter.autoLootingSpeedMin)
						Settings::scrapLooter.autoLootingSpeedMin = Settings::scrapLooter.autoLootingSpeedMax;
				}
			}

			ImGui::SetNextItemWidth(451.0f);
			ImGui::SliderInt("###ScrapLooterDistance", &Settings::scrapLooter.maxDistance, 1, 3000, "Scrap Looter Distance: %d");

			for (auto i = 0; i < 40; i++)
			{
				ButtonToggle(Settings::scrapLooter.nameList[i], &Settings::scrapLooter.enabledList[i]);

				ImGui::SameLine(235.0f);
				ImGui::SetNextItemWidth(224.0f);

				auto inputLabel = fmt::format("###ScrapReadOnly{:d}", i);
				auto inputText = fmt::format("{:08X}", Settings::scrapLooter.formIdList[i]);
				ImGui::InputText(inputLabel.c_str(), &inputText, ImGuiInputTextFlags_ReadOnly);
			}
		}

		if (ImGui::CollapsingHeader("Item Looter"))
		{
			if (ErectusMemory::CheckItemLooterSettings())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				if (ImGui::Button("Loot Items (Keybind: CTRL+R)###LootSelectedItemsEnabled", ImVec2(224.0f, 0.0f)))
					ErectusMemory::LootItems();
				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
				ImGui::Button("Loot Items (Keybind: CTRL+R)###LootSelectedItemsDisabled", ImVec2(224.0f, 0.0f));
				ImGui::PopStyleColor(3);
				ImGui::PopItemFlag();
			}

			ImGui::SameLine(235.0f);
			ButtonToggle("Item Looter Keybind Enabled", &Settings::itemLooter.keybindEnabled);

			ButtonToggle("Automatic Looting Enabled###ItemAutomaticLootingEnabled", &Settings::itemLooter.autoLootingEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Draw Automatic Looting Status###ItemAutomaticStatus", &Settings::itemLooter.drawStatus);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Speed (Min): {0:d} ({1:d} ms)", Settings::itemLooter.autoLootingSpeedMin, Settings::itemLooter.autoLootingSpeedMin * 16);
				if (ImGui::SliderInt("###ItemAutomaticSpeedMin", &Settings::itemLooter.autoLootingSpeedMin, 10, 60, sliderText.c_str()))
				{
					if (Settings::itemLooter.autoLootingSpeedMax < Settings::itemLooter.autoLootingSpeedMin)
						Settings::itemLooter.autoLootingSpeedMax = Settings::itemLooter.autoLootingSpeedMin;
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Speed (Max): {0:d} ({1:d} ms)", Settings::itemLooter.autoLootingSpeedMax, Settings::itemLooter.autoLootingSpeedMax * 16);
				if (ImGui::SliderInt("###ItemAutomaticSpeedMax", &Settings::itemLooter.autoLootingSpeedMax, 10, 60, sliderText.c_str()))
				{
					if (Settings::itemLooter.autoLootingSpeedMax < Settings::itemLooter.autoLootingSpeedMin)
						Settings::itemLooter.autoLootingSpeedMin = Settings::itemLooter.autoLootingSpeedMax;
				}
			}

			ButtonToggle("Weapons Enabled###ItemLooterWeaponsEnabled", &Settings::itemLooter.lootWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterWeaponsDistance", &Settings::itemLooter.lootWeaponsDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Armor Enabled###ItemLooterArmorEnabled", &Settings::itemLooter.lootArmorEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterArmorDistance", &Settings::itemLooter.lootArmorDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Ammo Enabled###ItemLooterAmmoEnabled", &Settings::itemLooter.lootAmmoEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterAmmoDistance", &Settings::itemLooter.lootAmmoDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Mods Enabled###ItemLooterModsEnabled", &Settings::itemLooter.lootModsEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterModsDistance", &Settings::itemLooter.lootModsDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Magazines Enabled###ItemLooterMagazinesEnabled", &Settings::itemLooter.lootMagazinesEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterMagazinesDistance", &Settings::itemLooter.lootMagazinesDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Bobbleheads Enabled###ItemLooterBobbleheadsEnabled", &Settings::itemLooter.lootBobbleheadsEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterBobbleheadsDistance", &Settings::itemLooter.lootBobbleheadsDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Aid Enabled###ItemLooterAidEnabled", &Settings::itemLooter.lootAidEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterAidDistance", &Settings::itemLooter.lootAidDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Known Plans Enabled###ItemLooterKnownPlansEnabled", &Settings::itemLooter.lootKnownPlansEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterKnownPlansDistance", &Settings::itemLooter.lootKnownPlansDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Unknown Plans Enabled###ItemLooterUnknownPlansEnabled", &Settings::itemLooter.lootUnknownPlansEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterUnknownPlansDistance", &Settings::itemLooter.lootUnknownPlansDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Misc Enabled###ItemLooterMiscEnabled", &Settings::itemLooter.lootMiscEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterMiscDistance", &Settings::itemLooter.lootMiscDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Other Enabled###ItemLooterUnlistedEnabled", &Settings::itemLooter.lootUnlistedEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterUnlistedDistance", &Settings::itemLooter.lootUnlistedDistance, 0, 3000, "Distance: %d");

			ButtonToggle("Item FormId List Enabled###ItemLooterListEnabled", &Settings::itemLooter.lootListEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###ItemLooterListDistance", &Settings::itemLooter.lootListDistance, 0, 3000, "Distance: %d");

			LargeButtonToggle("Item Looter Blacklist Enabled###ItemLooterBlacklistToggle", &Settings::itemLooter.blacklistToggle);

			if (ImGui::CollapsingHeader("Item Looter FormId List"))
			{
				for (auto i = 0; i < 100; i++)
				{
					auto toggleLabel = fmt::format("Item Looter Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::itemLooter.enabledList[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ItemLooterList{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::itemLooter.formIdList[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (ImGui::CollapsingHeader("Item Looter Blacklist"))
			{
				for (auto i = 0; i < 64; i++)
				{
					auto toggleLabel = fmt::format("Item Looter Blacklist: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::itemLooter.blacklistEnabled[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ItemLooterBlacklist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::itemLooter.blacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("NPC Looter (76m Distance Limit)"))
		{
			LargeButtonToggle("Automatic NPC Looting Enabled (Keybind: CTRL+COMMA)###NPCLooterEnabled", &Settings::npcLooter.enabled);

			LargeButtonToggle("Draw NPC Looter Status###NPCLooterStatusEnabled", &Settings::npcLooter.drawStatus);

			ButtonToggle("All Weapons Enabled###NPCLooterAllWeaponsEnabled", &Settings::npcLooter.allWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("All Armor Enabled###NPCLooterAllArmorEnabled", &Settings::npcLooter.allArmorEnabled);

			ButtonToggle("1* Weapons Enabled###NPCLooterOneStarWeaponsEnabled", &Settings::npcLooter.oneStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("1* Armor Enabled###NPCLooterOneStarArmorEnabled", &Settings::npcLooter.oneStarArmorEnabled);

			ButtonToggle("2* Weapons Enabled###NPCLooterTwoStarWeaponsEnabled", &Settings::npcLooter.twoStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("2* Armor Enabled###NPCLooterTwoStarArmorEnabled", &Settings::npcLooter.twoStarArmorEnabled);

			ButtonToggle("3* Weapons Enabled###NPCLooterThreeStarWeaponsEnabled", &Settings::npcLooter.threeStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("3* Armor Enabled###NPCLooterThreeStarArmorEnabled", &Settings::npcLooter.threeStarArmorEnabled);

			ButtonToggle("Ammo Enabled###NPCLooterAmmoEnabled", &Settings::npcLooter.ammoEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Mods Enabled###NPCLooterModsEnabled", &Settings::npcLooter.modsEnabled);

			ButtonToggle("Caps Enabled###NPCLooterCapsEnabled", &Settings::npcLooter.capsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Junk Enabled###NPCLooterJunkEnabled", &Settings::npcLooter.junkEnabled);

			ButtonToggle("Aid Enabled###NPCLooterAidEnabled", &Settings::npcLooter.aidEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Treasure Maps Enabled###NPCLooterTreasureMapsEnabled", &Settings::npcLooter.treasureMapsEnabled);

			ButtonToggle("Known Plans Enabled###NPCLooterKnownPlansEnabled", &Settings::npcLooter.knownPlansEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Unknown Plans Enabled###NPCLooterUnknownPlansEnabled", &Settings::npcLooter.unknownPlansEnabled);

			ButtonToggle("Misc Enabled###NPCLooterMiscEnabled", &Settings::npcLooter.miscEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Other Enabled###NPCLooterUnlistedEnabled", &Settings::npcLooter.unlistedEnabled);

			LargeButtonToggle("NPC Looter FormId List Enabled###NPCLooterListEnabled", &Settings::npcLooter.listEnabled);
			LargeButtonToggle("NPC Looter Blacklist Enabled###NPCLooterBlacklistToggle", &Settings::npcLooter.blacklistToggle);

			if (ImGui::CollapsingHeader("NPC Looter FormId List"))
			{
				for (auto i = 0; i < 100; i++)
				{
					auto toggleLabel = fmt::format("NPC Looter Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::npcLooter.enabledList[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###NPCLooterList{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::npcLooter.formIdList[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (ImGui::CollapsingHeader("NPC Looter Blacklist"))
			{
				for (auto i = 0; i < 64; i++)
				{
					auto toggleLabel = fmt::format("NPC Looter Blacklist: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::npcLooter.blacklistEnabled[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###NPCLooterBlacklist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::npcLooter.blacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Container Looter (6m Distance Limit)"))
		{
			LargeButtonToggle("Automatic Container Looting Enabled (Keybind: CTRL+PERIOD)###ContainerLooterEnabled", &Settings::containerLooter.enabled);

			LargeButtonToggle("Draw Container Looter Status###ContainerLooterStatusEnabled", &Settings::containerLooter.drawStatus);

			ButtonToggle("All Weapons Enabled###ContainerLooterAllWeaponsEnabled", &Settings::containerLooter.allWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("All Armor Enabled###ContainerLooterAllArmorEnabled", &Settings::containerLooter.allArmorEnabled);

			ButtonToggle("1* Weapons Enabled###ContainerLooterOneStarWeaponsEnabled", &Settings::containerLooter.oneStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("1* Armor Enabled###ContainerLooterOneStarArmorEnabled", &Settings::containerLooter.oneStarArmorEnabled);

			ButtonToggle("2* Weapons Enabled###ContainerLooterTwoStarWeaponsEnabled", &Settings::containerLooter.twoStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("2* Armor Enabled###ContainerLooterTwoStarArmorEnabled", &Settings::containerLooter.twoStarArmorEnabled);

			ButtonToggle("3* Weapons Enabled###ContainerLooterThreeStarWeaponsEnabled", &Settings::containerLooter.threeStarWeaponsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("3* Armor Enabled###ContainerLooterThreeStarArmorEnabled", &Settings::containerLooter.threeStarArmorEnabled);

			ButtonToggle("Ammo Enabled###ContainerLooterAmmoEnabled", &Settings::containerLooter.ammoEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Mods Enabled###ContainerLooterModsEnabled", &Settings::containerLooter.modsEnabled);

			ButtonToggle("Caps Enabled###ContainerLooterCapsEnabled", &Settings::containerLooter.capsEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Junk Enabled###ContainerLooterJunkEnabled", &Settings::containerLooter.junkEnabled);

			ButtonToggle("Aid Enabled###ContainerLooterAidEnabled", &Settings::containerLooter.aidEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Treasure Maps Enabled###ContainerLooterTreasureMapsEnabled", &Settings::containerLooter.treasureMapsEnabled);

			ButtonToggle("Known Plans Enabled###ContainerLooterKnownPlansEnabled", &Settings::containerLooter.knownPlansEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Unknown Plans Enabled###ContainerLooterUnknownPlansEnabled", &Settings::containerLooter.unknownPlansEnabled);

			ButtonToggle("Misc Enabled###ContainerLooterMiscEnabled", &Settings::containerLooter.miscEnabled);
			ImGui::SameLine(235.0f);
			ButtonToggle("Other Enabled###ContainerLooterUnlistedEnabled", &Settings::containerLooter.unlistedEnabled);

			LargeButtonToggle("Container Looter FormId List Enabled###ContainerLooterListEnabled", &Settings::containerLooter.listEnabled);

			LargeButtonToggle("Container Looter Blacklist Enabled###ContainerLooterBlacklistToggle", &Settings::containerLooter.blacklistToggle);

			if (ImGui::CollapsingHeader("Container Looter FormId List"))
			{
				for (auto i = 0; i < 100; i++)
				{
					auto toggleLabel = fmt::format("Container Looter Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::containerLooter.enabledList[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ContainerLooterList{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::containerLooter.formIdList[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (ImGui::CollapsingHeader("Container Looter Blacklist"))
			{
				for (auto i = 0; i < 64; i++)
				{
					auto toggleLabel = fmt::format("Container Looter Blacklist: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::containerLooter.blacklistEnabled[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ContainerLooterBlacklist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::containerLooter.blacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Flora Harvester (6m Distance Limit)"))
		{
			LargeButtonToggle("Automatic Flora Harvesting Enabled (Keybind: CTRL+P])###HarvesterEnabled", &Settings::harvester.enabled);
			LargeButtonToggle("Draw Flora Harvester Status###HarvesterStatusEnabled", &Settings::harvester.drawStatus);
			LargeButtonToggle("Flora Harvester ESP Override (Uses Flora ESP Settings)", &Settings::harvester.overrideEnabled);

			for (auto i = 0; i < 69; i++)
			{
				ButtonToggle(Settings::harvester.nameList[i], &Settings::harvester.enabledList[i]);

				ImGui::SameLine(235.0f);
				ImGui::SetNextItemWidth(224.0f);

				auto inputLabel = fmt::format("###HarvesterReadOnly{:d}", i);
				auto inputText = fmt::format("{:08X}", Settings::harvester.formIdList[i]);
				ImGui::InputText(inputLabel.c_str(), &inputText, ImGuiInputTextFlags_ReadOnly);
			}
		}

		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabCombat()
{
	if (ImGui::BeginTabItem("Combat###CombatTab"))
	{
		if (ImGui::CollapsingHeader("Weapon Editor"))
		{
			ButtonToggle("No Recoil", &Settings::weapons.noRecoil);
			ImGui::SameLine(235.0f);
			ButtonToggle("Infinite Ammo", &Settings::weapons.infiniteAmmo);

			ButtonToggle("No Spread", &Settings::weapons.noSpread);
			ImGui::SameLine(235.0f);
			ButtonToggle("Instant Reload", &Settings::weapons.instantReload);

			ButtonToggle("No Sway", &Settings::weapons.noSway);
			ImGui::SameLine(235.0f);
			ButtonToggle("Automatic Flag###WeaponAutomatic", &Settings::weapons.automaticflag);

			ButtonToggle("Capacity###WeaponCapacityEnabled", &Settings::weapons.capacityEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###WeaponCapacity", &Settings::weapons.capacity, 0, 999, "Capacity: %d");

			ButtonToggle("Speed###WeaponSpeedEnabled", &Settings::weapons.speedEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###WeaponSpeed", &Settings::weapons.speed, 0.0f, 100.0f, "Speed: %.2f");

			ButtonToggle("Reach###WeaponReachEnabled", &Settings::weapons.reachEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###WeaponReach", &Settings::weapons.reach, 0.0f, 999.0f, "Reach: %.2f");
		}

		if (ImGui::CollapsingHeader("Targeting Settings"))
		{
			ButtonToggle("Player Targeting (Keybind: T)", &Settings::targetting.lockPlayers);
			ImGui::SameLine(235.0f);
			ButtonToggle("NPC Targeting (Keybind: T)", &Settings::targetting.lockNpCs);

			ButtonToggle("Damage Redirection (Players)", &Settings::targetting.indirectPlayers);
			ImGui::SameLine(235.0f);
			ButtonToggle("Damage Redirection (NPCs)", &Settings::targetting.indirectNpCs);

			ButtonToggle("Send Damage (Players)", &Settings::targetting.directPlayers);
			ImGui::SameLine(235.0f);
			ButtonToggle("Send Damage (NPCs)", &Settings::targetting.directNpCs);

			SmallButtonToggle("Living###TargetLiving", &Settings::targetting.targetLiving);
			ImGui::SameLine(122.0f);
			SmallButtonToggle("Downed###TargetDowned", &Settings::targetting.targetDowned);
			ImGui::SameLine(235.0f);
			SmallButtonToggle("Dead###TargetDead", &Settings::targetting.targetDead);
			ImGui::SameLine(349.0f);
			SmallButtonToggle("Unknown###TargetUnknown", &Settings::targetting.targetUnknown);

			ButtonToggle("Ignore Render Distance###IgnoreRenderDistance", &Settings::targetting.ignoreRenderDistance);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderFloat("###TargetLockingFOV", &Settings::targetting.lockingFov, 5.0f, 40.0f, "Targeting FOV: %.2f");

			ButtonToggle("Ignore Essential NPCs###IgnoreEssentialNPCs", &Settings::targetting.ignoreEssentialNpCs);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::ColorEdit3("###TargetLockingColor", Settings::targetting.lockingColor);
			Utils::ValidateRgb(Settings::playerSettings.unknownColor);

			ButtonToggle("Automatic Retargeting###TargetLockingRetargeting", &Settings::targetting.retargeting);

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Cooldown: {0:d} ({1:d} ms)", Settings::targetting.cooldown, Settings::targetting.cooldown * 16);
				ImGui::SliderInt("###TargetLockingCooldown", &Settings::targetting.cooldown, 0, 120, sliderText.c_str());
			}

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Send Damage (Min): {0:d} ({1:d} ms)", Settings::targetting.sendDamageMin, Settings::targetting.sendDamageMin * 16);
				if (ImGui::SliderInt("###SendDamageMin", &Settings::targetting.sendDamageMin, 1, 60, sliderText.c_str()))
				{
					if (Settings::targetting.sendDamageMax < Settings::targetting.sendDamageMin)
						Settings::targetting.sendDamageMax = Settings::targetting.sendDamageMin;
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Send Damage (Max): {0:d} ({1:d} ms)", Settings::targetting.sendDamageMax, Settings::targetting.sendDamageMax * 16);
				if (ImGui::SliderInt("###SendDamageMax", &Settings::targetting.sendDamageMax, 1, 60, sliderText.c_str()))
				{
					if (Settings::targetting.sendDamageMax < Settings::targetting.sendDamageMin)
						Settings::targetting.sendDamageMin = Settings::targetting.sendDamageMax;
				}
			}

			{
				std::string favoritedWeaponsPreview = "[?] No Weapon Selected";
				if (Settings::targetting.favoriteIndex < 12)
				{
					favoritedWeaponsPreview = ErectusMemory::GetFavoritedWeaponText(BYTE(Settings::targetting.favoriteIndex));
					if (favoritedWeaponsPreview.empty())
					{
						favoritedWeaponsPreview = fmt::format("[{}] Favorited Item Invalid", ErectusMemory::FavoriteIndex2Slot(BYTE(Settings::targetting.favoriteIndex)));
					}
				}

				ImGui::SetNextItemWidth(451.0f);
				if (ImGui::BeginCombo("###BeginTempCombo", favoritedWeaponsPreview.c_str()))
				{
					for (const auto& item : ErectusMemory::GetFavoritedWeapons())
					{
						if (ImGui::Selectable(item.second.c_str()))
						{
							if (item.first)
								Settings::targetting.favoriteIndex = item.first - 1;
							else
								Settings::targetting.favoriteIndex = 12;
						}
					}
					Utils::ValidateInt(Settings::targetting.favoriteIndex, 0, 12);

					ImGui::EndCombo();
				}
			}
		}

		if (ImGui::CollapsingHeader("Melee Settings"))
		{
			LargeButtonToggle("Melee Enabled (Keybind: U)", &Settings::melee.enabled);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Melee Speed (Min): {0:d} ({1:d} ms)", Settings::melee.speedMin, Settings::melee.speedMin * 16);
				if (ImGui::SliderInt("###MeleeSpeedMin", &Settings::melee.speedMin, 1, 60, sliderText.c_str()))
				{
					if (Settings::melee.speedMax < Settings::melee.speedMin)
						Settings::melee.speedMax = Settings::melee.speedMin;
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				auto sliderText = fmt::format("Melee Speed (Max): {0:d} ({1:d} ms)", Settings::melee.speedMax, Settings::melee.speedMax * 16);
				if (ImGui::SliderInt("###MeleeSpeedMax", &Settings::melee.speedMax, 1, 60, sliderText.c_str()))
				{
					if (Settings::melee.speedMax < Settings::melee.speedMin)
						Settings::melee.speedMin = Settings::melee.speedMax;
				}
			}
		}

		if (ImGui::CollapsingHeader("One Position Kill"))
		{
			LargeButtonToggle("OPK Players (Keybind: CTRL+B)", &Settings::opk.playersEnabled);
			LargeButtonToggle("OPK NPCs (Keybind: CTRL+N)", &Settings::opk.npcsEnabled);
		}

		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabPlayer()
{
	if (ImGui::BeginTabItem("Player###PlayerTab"))
	{
		if (ImGui::CollapsingHeader("Local Player Settings"))
		{
			LargeButtonToggle("Position Spoofing (Keybind CTRL+L)##LocalPlayerPositionSpoofingEnabled", &Settings::customLocalPlayerSettings.positionSpoofingEnabled);
			ButtonToggle("Draw Position Status###LocalPlayerDrawPositionSpoofingEnabled", &Settings::customLocalPlayerSettings.drawPositionSpoofingStatus);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerPositionSpoofingHeight", &Settings::customLocalPlayerSettings.positionSpoofingHeight, -524287, 524287, "Spoofed Height: %d");

			ButtonToggle("Noclip (Keybind CTRL+Y)###NoclipEnabled", &Settings::customLocalPlayerSettings.noclipEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderFloat("###NoclipSpeed", &Settings::customLocalPlayerSettings.noclipSpeed, 0.0f, 2.0f, "Speed: %.5f");

			ButtonToggle("Client State", &Settings::customLocalPlayerSettings.clientState);
			ImGui::SameLine(235.0f);
			ButtonToggle("Automatic Client State", &Settings::customLocalPlayerSettings.automaticClientState);

			LargeButtonToggle("Freeze Action Points###LocalPlayerFreezeApEnabled", &Settings::customLocalPlayerSettings.freezeApEnabled);

			ButtonToggle("Action Points###LocalPlayerAPEnabled", &Settings::customLocalPlayerSettings.actionPointsEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerAP", &Settings::customLocalPlayerSettings.actionPoints, 0, 99999, "Action Points: %d");

			ButtonToggle("Strength###LocalPlayerStrengthEnabled", &Settings::customLocalPlayerSettings.strengthEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerStrength", &Settings::customLocalPlayerSettings.strength, 0, 99999, "Strength: %d");

			ButtonToggle("Perception###LocalPlayerPerceptionEnabled", &Settings::customLocalPlayerSettings.perceptionEnabled);
			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###LocalPlayerPerception", &Settings::customLocalPlayerSettings.perception, 0, 99999, "Perception: %d");

			ButtonToggle("Endurance###LocalPlayerEnduranceEnabled", &Settings::customLocalPlayerSettings.enduranceEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerEndurance", &Settings::customLocalPlayerSettings.endurance, 0, 99999, "Endurance: %d");

			ButtonToggle("Charisma###LocalPlayerCharismaEnabled", &Settings::customLocalPlayerSettings.charismaEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerCharisma", &Settings::customLocalPlayerSettings.charisma, 0, 99999, "Charisma: %d");

			ButtonToggle("Intelligence###LocalPlayerIntelligenceEnabled", &Settings::customLocalPlayerSettings.intelligenceEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerIntelligence", &Settings::customLocalPlayerSettings.intelligence, 0, 99999, "Intelligence: %d");

			ButtonToggle("Agility###LocalPlayerAgilityEnabled", &Settings::customLocalPlayerSettings.agilityEnabled);

			ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);

			ImGui::SliderInt("###LocalPlayerAgility", &Settings::customLocalPlayerSettings.agility, 0, 99999, "Agility: %d");

			ButtonToggle("Luck###LocalPlayerLuckEnabled", &Settings::customLocalPlayerSettings.luckEnabled);					ImGui::SameLine(235.0f);
			ImGui::SetNextItemWidth(224.0f);
			ImGui::SliderInt("###LocalPlayerLuck", &Settings::customLocalPlayerSettings.luck, 0, 99999, "Luck: %d");
		}

		if (ImGui::CollapsingHeader("Character Settings"))
		{
			LargeButtonToggle("Character Appearance Editing Enabled###ChargenEditingEnabled", &Settings::characterEditor.enabled);
			ImGui::SetNextItemWidth(451.0f);
			ImGui::SliderFloat("###ChargenThin", &Settings::characterEditor.thin, 0.0f, 1.0f, "Character Appearance (Thin): %f");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::SliderFloat("###ChargenMuscular", &Settings::characterEditor.muscular, 0.0f, 1.0f, "Character Appearance (Muscular): %f");

			ImGui::SetNextItemWidth(451.0f);
			ImGui::SliderFloat("###ChargenLarge", &Settings::characterEditor.large, 0.0f, 1.0f, "Character Appearance (Large): %f");
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabUtilities()
{
	if (ImGui::BeginTabItem("Utility###UtilityTab"))
	{
		if (ImGui::CollapsingHeader("Utility"))
		{
			ButtonToggle("Draw Local Player Data", &Settings::utilities.debugPlayer);

			ImGui::SameLine(235.0f);

			ButtonToggle("ESP Debug Mode", &Settings::utilities.debugEsp);

			{
				if (Settings::utilities.ptrFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					if (ImGui::Button("Get Pointer###GetPointerEnabled", ImVec2(224.0f, 0.0f)))
						getPtrResult = ErectusMemory::GetPtr(Settings::utilities.ptrFormId);

					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::Button("Get Pointer###GetPointerDisabled", ImVec2(224.0f, 0.0f));
					ImGui::PopItemFlag();

					ImGui::PopStyleColor(3);
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(80.0f);
				if (ImGui::InputScalar("###PtrFormIdText", ImGuiDataType_U32, &Settings::utilities.ptrFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal))
					getPtrResult = 0;
			}

			ImGui::SameLine(318.0f);

			{
				ImGui::SetNextItemWidth(141.0f);
				auto inputText = fmt::format("{:16X}", getPtrResult);
				ImGui::InputText("###PtrPointerText", &inputText, ImGuiInputTextFlags_ReadOnly);
			}

			{
				if (Settings::utilities.addressFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					if (ImGui::Button("Get Address###GetAddressEnabled", ImVec2(224.0f, 0.0f)))
						getAddressResult = ErectusMemory::GetAddress(Settings::utilities.addressFormId);

					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::Button("Get Address###GetAddressDisabled", ImVec2(224.0f, 0.0f));
					ImGui::PopItemFlag();

					ImGui::PopStyleColor(3);
				}
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(80.0f);

				if (ImGui::InputScalar("###AddressFormIdText", ImGuiDataType_U32, &Settings::utilities.addressFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal))
					getAddressResult = 0;
			}

			ImGui::SameLine(318.0f);

			{
				ImGui::SetNextItemWidth(141.0f);

				auto inputText = fmt::format("{:16X}", getAddressResult);
				ImGui::InputText("###AddressPointerText", &inputText, ImGuiInputTextFlags_ReadOnly);
			}
		}

		if (ImGui::CollapsingHeader("Reference Editor"))
		{
			ButtonToggle("Source FormId###SwapperSourceFormIdToggle", &swapperSourceToggle);

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				ImGui::InputScalar("###SwapperSourceFormIdText", ImGuiDataType_U32, & Settings::swapper.sourceFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			ButtonToggle("Destination FormId###SwapperDestinationFormIdToggle", &swapperDestinationToggle);

			ImGui::SameLine(235.0f);

			{
				ImGui::SetNextItemWidth(224.0f);
				ImGui::InputScalar("###SwapperDestinationFormIdText", ImGuiDataType_U32, &Settings::swapper.destinationFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			if (swapperSourceToggle && Settings::swapper.sourceFormId && swapperDestinationToggle && Settings::swapper.destinationFormId)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("Edit Reference (Overwrite Destination)###EditReferenceEnabled", ImVec2(451.0f, 0.0f)))
				{
					if (ErectusMemory::ReferenceSwap(Settings::swapper.sourceFormId, Settings::swapper.destinationFormId))
					{
						Settings::swapper.destinationFormId = Settings::swapper.sourceFormId;
						swapperSourceToggle = false;
						swapperDestinationToggle = false;
					}
				}

				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::Button("Edit Reference (Overwrite Destination)###EditReferenceDisabled", ImVec2(451.0f, 0.0f));
				ImGui::PopItemFlag();

				ImGui::PopStyleColor(3);
			}
		}

		if (ImGui::CollapsingHeader("Item Transferring"))
		{
			SmallButtonToggle("Source###TransferSourceFormIdToggle", &transferSourceToggle);

			ImGui::SameLine(122.0f);

			{
				ImGui::SetNextItemWidth(110.0f);
				ImGui::InputScalar("###TransferSourceFormIdText", ImGuiDataType_U32, &Settings::customTransferSettings.sourceFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			ImGui::SameLine(235.0f);

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("Get Player###TransferSourceLocalPlayer", ImVec2(110.0f, 0.0f)))
					Settings::customTransferSettings.sourceFormId = ErectusMemory::GetLocalPlayerFormId();

				ImGui::SameLine(349.0f);

				if (ImGui::Button("Get STASH###TransferSourceSTASH", ImVec2(110.0f, 0.0f)))
					Settings::customTransferSettings.sourceFormId = ErectusMemory::GetStashFormId();

				ImGui::PopStyleColor(3);
			}

			SmallButtonToggle("Destination###TransferDestinationFormIdToggle", &transferDestinationToggle);

			ImGui::SameLine(122.0f);

			{
				ImGui::SetNextItemWidth(110.0f);
				ImGui::InputScalar("###TransferDestinationFormIdText", ImGuiDataType_U32, &Settings::customTransferSettings.destinationFormId,
					nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
			}

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				ImGui::SameLine(235.0f);
				if (ImGui::Button("Get Player###TransferDestinationLocalPlayer", ImVec2(110.0f, 0.0f)))
					Settings::customTransferSettings.destinationFormId = ErectusMemory::GetLocalPlayerFormId();

				ImGui::SameLine(349.0f);
				if (ImGui::Button("Get STASH###TransferDestinationSTASH", ImVec2(110.0f, 0.0f)))
					Settings::customTransferSettings.destinationFormId = ErectusMemory::GetStashFormId();
				ImGui::PopStyleColor(3);
			}

			auto allowTransfer = false;

			if (transferSourceToggle && Settings::customTransferSettings.sourceFormId && transferDestinationToggle && Settings::customTransferSettings.destinationFormId)
			{
				if (Settings::customTransferSettings.useWhitelist)
					allowTransfer = ErectusMemory::CheckItemTransferList();
				else
					allowTransfer = true;
			}

			if (allowTransfer)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("Transfer Items###TransferItemsEnabled", ImVec2(451.0f, 0.0f)))
					ErectusMemory::TransferItems(Settings::customTransferSettings.sourceFormId, Settings::customTransferSettings.destinationFormId);

				ImGui::PopStyleColor(3);
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::Button("Transfer Items###TransferItemsDisabled", ImVec2(451.0f, 0.0f));
				ImGui::PopItemFlag();

				ImGui::PopStyleColor(3);
			}

			LargeButtonToggle("Use Item Transfer Whitelist", &Settings::customTransferSettings.useWhitelist);
			LargeButtonToggle("Use Item Transfer Blacklist", &Settings::customTransferSettings.useBlacklist);

			if (ImGui::CollapsingHeader("Item Transfer Whitelist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Transfer Whitelist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::customTransferSettings.whitelisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ItemTransferWhitelist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::customTransferSettings.whitelist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (ImGui::CollapsingHeader("Item Transfer Blacklist Settings"))
			{
				for (auto i = 0; i < 32; i++)
				{
					auto toggleLabel = fmt::format("Transfer Blacklist Slot: {0:d}", i);
					ButtonToggle(toggleLabel.c_str(), &Settings::customTransferSettings.blacklisted[i]);

					ImGui::SameLine(235.0f);
					ImGui::SetNextItemWidth(224.0f);

					auto inputLabel = fmt::format("###ItemTransferBlacklist{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::customTransferSettings.blacklist[i],
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}
		}

		if (ImGui::CollapsingHeader("Nuke Codes"))
		{
			ButtonToggle("Automatic Nuke Codes", &Settings::customNukeCodeSettings.automaticNukeCodes);

			ImGui::SameLine(235.0f);

			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

				if (ImGui::Button("Get Nuke Codes", ImVec2(224.0f, 0.0f)))
				{
					ErectusMemory::GetNukeCode(0x000921AE, alphaCode);
					ErectusMemory::GetNukeCode(0x00092213, bravoCode);
					ErectusMemory::GetNukeCode(0x00092214, charlieCode);
				}

				ImGui::PopStyleColor(3);
			}

			ButtonToggle("Draw Nuke Code Alpha", &Settings::customNukeCodeSettings.drawCodeAlpha);

			ImGui::SameLine(255.0f);

			auto text = fmt::format("{} - Alpha", fmt::join(alphaCode, " "));
			ImGui::Text(text.c_str());

			ButtonToggle("Draw Nuke Code Bravo", &Settings::customNukeCodeSettings.drawCodeBravo);

			ImGui::SameLine(255.0f);

			text = fmt::format("{} - Bravo", fmt::join(bravoCode, " "));
			ImGui::Text(text.c_str());

			ButtonToggle("Draw Nuke Code Charlie", &Settings::customNukeCodeSettings.drawCodeCharlie);

			ImGui::SameLine(255.0f);

			text = fmt::format("{} - Charlie", fmt::join(charlieCode, " "));
			ImGui::Text(text.c_str());
		}

		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabTeleporter()
{
	if (ImGui::BeginTabItem("Teleporter###TeleporterTab"))
	{
		for (auto i = 0; i < 16; i++)
		{
			auto teleportHeaderText = fmt::format("Teleport Slot: {0:d}", i);
			if (ImGui::CollapsingHeader(teleportHeaderText.c_str()))
			{
				{
					ImGui::SetNextItemWidth(110.0f);
					auto inputLabel = fmt::format("###TeleportDestinationX{:d}", i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].destination[0]);
				}
				
				ImGui::SameLine(122.0f);

				{
					ImGui::SetNextItemWidth(110.0f);
					auto inputLabel = fmt::format("###TeleportDestinationY{:d}", i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].destination[1]);
				}
				
				ImGui::SameLine(235.0f);

				{
					ImGui::SetNextItemWidth(110.0f);
					auto inputLabel = fmt::format("###TeleportDestinationZ{:d}", i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].destination[2]);
				}
				
				ImGui::SameLine(349.0f);

				{
					ImGui::SetNextItemWidth(110.0f);
					auto inputLabel = fmt::format("###TeleportDestinationW{:d}", i);
					ImGui::InputFloat(inputLabel.c_str(), &Settings::teleporter.entries[i].destination[3]);
				}
				
				{
					ImGui::SetNextItemWidth(110.0f);
					auto inputLabel = fmt::format("###TeleportCellFormId{:d}", i);
					ImGui::InputScalar(inputLabel.c_str(), ImGuiDataType_U32, &Settings::teleporter.entries[i].cellFormId,
						nullptr, nullptr, "%08lX", ImGuiInputTextFlags_CharsHexadecimal);
				}
				
				ImGui::SameLine(122.0f);

				{
					auto buttonLabel = fmt::format("Set Position###TeleportDestination{:d}", i);
					if (!Settings::teleporter.entries[i].disableSaving)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

						if (ImGui::Button(buttonLabel.c_str(), ImVec2(110.0f, 0.0f)))
							ErectusMemory::GetTeleportPosition(i);

						ImGui::PopStyleColor(3);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::Button(buttonLabel.c_str(), ImVec2(110.0f, 0.0f));
						ImGui::PopItemFlag();

						ImGui::PopStyleColor(3);
					}
				}
				
				ImGui::SameLine(235.0f);

				{
					auto buttonLabel = fmt::format("Lock###DisableSaving{:d}", i);
					SmallButtonToggle(buttonLabel.c_str(), &Settings::teleporter.entries[i].disableSaving);
				}

				ImGui::SameLine(349.0f);

				if (Settings::teleporter.entries[i].cellFormId)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));

					auto buttonLabel = fmt::format("Teleport###TeleportRequestEnabled{:d}", i);
					if (ImGui::Button(buttonLabel.c_str(), ImVec2(110.0f, 0.0f)))
						ErectusMemory::RequestTeleport(i);
					ImGui::PopStyleColor(3);
				}
				else
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.3f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));

					auto buttonLabel = fmt::format("Teleport###TeleportRequestDisabled{:d}", i);
					ImGui::Button(buttonLabel.c_str(), ImVec2(110.0f, 0.0f));
					ImGui::PopStyleColor(3);
					ImGui::PopItemFlag();
				}
			}
		}
		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenuTabBitMsgWriter()
{
	if (ImGui::BeginTabItem("BitMsgWriter###BitMsgWriterTab"))
	{
		LargeButtonToggle("Message Sender Enabled", &ErectusMemory::allowMessages);

		ImGui::EndTabItem();
	}
}

void Gui::OverlayMenu()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImVec2(static_cast<float>(App::windowSize[0]), static_cast<float>(App::windowSize[1])));
	ImGui::SetNextWindowCollapsed(false);

	if (ImGui::Begin("Erectus - Overlay Menu", nullptr,
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Exit"))
				App::CloseWnd();

			if (ImGui::MenuItem("Process Menu"))
				ErectusProcess::SetProcessMenu();

			if (ImGui::MenuItem("Overlay"))
			{
				if (!App::SetOverlayPosition(false, true))
					ErectusProcess::SetProcessMenu();
			}

			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("###OverlayMenuTabBar", ImGuiTabBarFlags_None))
		{
			OverlayMenuTabEsp();
			OverlayMenuTabLoot();
			OverlayMenuTabCombat();
			OverlayMenuTabPlayer();
			OverlayMenuTabUtilities();
			OverlayMenuTabTeleporter();
			OverlayMenuTabBitMsgWriter();

			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

bool Gui::ImGuiInitialize()
{
	ImGui::CreateContext();
	if (!ImGui_ImplWin32_Init(App::appHwnd))
		return false;

	if (!ImGui_ImplDX9_Init(Renderer::d3D9Device))
		return false;

	return true;
}

void Gui::ImGuiCleanup()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
