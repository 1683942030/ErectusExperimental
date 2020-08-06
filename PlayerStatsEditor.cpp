#include "PlayerStatsEditor.h"

#include "common.h"
#include "ErectusMemory.h"
#include "ErectusProcess.h"
#include "settings.h"
#include "utils.h"

bool PlayerStatsEditor::Edit(const bool enabled)
{
	if (!actorValuePage && !Settings::localPlayer.IsEnabled())
		return false;

	if (!actorValuePage)
	{
		actorValuePage = ErectusProcess::AllocEx(sizeof(ActorValueHook));
		if (!actorValuePage)
			return false;
	}

	const auto localPlayerPtr = ErectusMemory::GetLocalPlayerPtr(false);
	if (!Utils::Valid(localPlayerPtr))
		return false;

	TesObjectRefr localPlayer{};
	if (!ErectusProcess::Rpm(localPlayerPtr, &localPlayer, sizeof localPlayer))
		return false;
	if (!Utils::Valid(localPlayer.vtable0050))
		return false;

	DWORD64 actorValueFunction;
	if (!ErectusProcess::Rpm(localPlayer.vtable0050 + 0x48, &actorValueFunction, sizeof actorValueFunction))
		return false;
	if (!Utils::Valid(actorValueFunction))
		return false;

	ActorValueHook actorValueHookData = {
		.actionPointsEnabled = static_cast<int>(Settings::localPlayer.actionPointsEnabled),
		.actionPoints = static_cast<float>(Settings::localPlayer.actionPoints),
		.strengthEnabled = static_cast<int>(Settings::localPlayer.strengthEnabled),
		.strength = static_cast<float>(Settings::localPlayer.strength),
		.perceptionEnabled = static_cast<int>(Settings::localPlayer.perceptionEnabled),
		.perception = static_cast<float>(Settings::localPlayer.perception),
		.enduranceEnabled = static_cast<int>(Settings::localPlayer.enduranceEnabled),
		.endurance = static_cast<float>(Settings::localPlayer.endurance),
		.charismaEnabled = static_cast<int>(Settings::localPlayer.charismaEnabled),
		.charisma = static_cast<float>(Settings::localPlayer.charisma),
		.intelligenceEnabled = static_cast<int>(Settings::localPlayer.intelligenceEnabled),
		.intelligence = static_cast<float>(Settings::localPlayer.intelligence),
		.agilityEnabled = static_cast<int>(Settings::localPlayer.agilityEnabled),
		.agility = static_cast<float>(Settings::localPlayer.agility),
		.luckEnabled = static_cast<int>(Settings::localPlayer.luckEnabled),
		.luck = static_cast<float>(Settings::localPlayer.luck),
		.originalFunction = ErectusProcess::exe + OFFSET_ACTOR_VALUE,
	};

	if (actorValueFunction != ErectusProcess::exe + OFFSET_ACTOR_VALUE && actorValueFunction != actorValuePage)
	{
		if (ErectusMemory::VtableSwap(localPlayer.vtable0050 + 0x48, ErectusProcess::exe + OFFSET_ACTOR_VALUE))
			ErectusProcess::FreeEx(actorValueFunction);
	}

	if (enabled)
	{
		if (actorValuePageValid)
		{
			ActorValueHook actorValueHookCheck;
			if (!ErectusProcess::Rpm(actorValuePage, &actorValueHookCheck, sizeof actorValueHookCheck))
				return false;
			if (memcmp(&actorValueHookData, &actorValueHookCheck, sizeof actorValueHookCheck) != 0)
			{
				if (!ErectusProcess::Wpm(actorValuePage, &actorValueHookData, sizeof actorValueHookData))
					return false;
			}
		}
		else
		{
			if (!ErectusProcess::Wpm(actorValuePage, &actorValueHookData, sizeof actorValueHookData))
				return false;
			if (!ErectusMemory::VtableSwap(localPlayer.vtable0050 + 0x48, actorValuePage))
				return false;
			actorValuePageValid = true;
		}

		SetActorValueMaximum(0x000002C2, 100.0f, static_cast<float>(Settings::localPlayer.strength), Settings::localPlayer.strengthEnabled);
		SetActorValueMaximum(0x000002C3, 100.0f, static_cast<float>(Settings::localPlayer.perception), Settings::localPlayer.perceptionEnabled);
		SetActorValueMaximum(0x000002C4, 100.0f, static_cast<float>(Settings::localPlayer.endurance), Settings::localPlayer.enduranceEnabled);
		SetActorValueMaximum(0x000002C5, 100.0f, static_cast<float>(Settings::localPlayer.charisma), Settings::localPlayer.charismaEnabled);
		SetActorValueMaximum(0x000002C6, 100.0f, static_cast<float>(Settings::localPlayer.agility), Settings::localPlayer.agilityEnabled);
		SetActorValueMaximum(0x000002C7, 100.0f, static_cast<float>(Settings::localPlayer.intelligence), Settings::localPlayer.intelligenceEnabled);
		SetActorValueMaximum(0x000002C8, 100.0f, static_cast<float>(Settings::localPlayer.luck), Settings::localPlayer.luckEnabled);
	}
	else
	{
		SetActorValueMaximum(0x000002C2, 100.0f, static_cast<float>(Settings::localPlayer.strength), false);
		SetActorValueMaximum(0x000002C3, 100.0f, static_cast<float>(Settings::localPlayer.perception), false);
		SetActorValueMaximum(0x000002C4, 100.0f, static_cast<float>(Settings::localPlayer.endurance), false);
		SetActorValueMaximum(0x000002C5, 100.0f, static_cast<float>(Settings::localPlayer.charisma), false);
		SetActorValueMaximum(0x000002C6, 100.0f, static_cast<float>(Settings::localPlayer.agility), false);
		SetActorValueMaximum(0x000002C7, 100.0f, static_cast<float>(Settings::localPlayer.intelligence), false);
		SetActorValueMaximum(0x000002C8, 100.0f, static_cast<float>(Settings::localPlayer.luck), false);

		if (actorValueFunction != ErectusProcess::exe + OFFSET_ACTOR_VALUE)
		{
			if (ErectusMemory::VtableSwap(localPlayer.vtable0050 + 0x48, ErectusProcess::exe + OFFSET_ACTOR_VALUE))
				ErectusProcess::FreeEx(actorValueFunction);
		}

		if (actorValuePage)
		{
			if (ErectusProcess::FreeEx(actorValuePage))
			{
				actorValuePage = 0;
				actorValuePageValid = false;
			}
		}
	}

	return true;
}

bool PlayerStatsEditor::SetActorValueMaximum(const DWORD formId, const float defaultValue, const float customValue, const bool enabled)
{
	if (!actorValuePage || !actorValuePageValid)
		return false;
	
	const auto actorValuePtr = ErectusMemory::GetPtr(formId);
	if (!Utils::Valid(actorValuePtr))
		return false;
	
	ActorValueInformation actorValueData{};
	if (!ErectusProcess::Rpm(actorValuePtr, &actorValueData, sizeof actorValueData))
		return false;

	if (enabled)
	{
		if (actorValueData.maximumValue != customValue)
		{
			actorValueData.maximumValue = customValue;
			return ErectusProcess::Wpm(actorValuePtr, &actorValueData, sizeof actorValueData);
		}
	}
	else
	{
		if (actorValueData.maximumValue != defaultValue)
		{
			actorValueData.maximumValue = defaultValue;
			return ErectusProcess::Wpm(actorValuePtr, &actorValueData, sizeof actorValueData);
		}
	}

	return true;
}
