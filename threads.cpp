#include "threads.h"
#include "app.h"
#include "settings.h"
#include "utils.h"
#include "common.h"
#include <thread>

#include "ErectusMemory.h"
#include "ErectusProcess.h"
#include "Looter.h"

DWORD WINAPI Threads::BufferEntityListThread([[maybe_unused]] LPVOID lpParameter)
{
	while (!threadDestructionState)
	{
		ErectusMemory::UpdateBufferEntityList();
		std::this_thread::sleep_for(std::chrono::milliseconds(30 * 16));
	}

	ErectusMemory::entityDataBuffer.clear();

	bufferEntityListThreadActive = false;

	return 0xBEEF;
}

DWORD WINAPI Threads::BufferPlayerListThread([[maybe_unused]] LPVOID lpParameter)
{
	while (!threadDestructionState)
	{
		ErectusMemory::UpdateBufferPlayerList();
		std::this_thread::sleep_for(std::chrono::milliseconds(60 * 16));
	}

	ErectusMemory::playerDataBuffer.clear();

	bufferPlayerListThreadActive = false;

	return 0xFEED;
}

DWORD WINAPI Threads::WeaponEditorThread([[maybe_unused]] LPVOID lpParameter)
{
	while (!threadDestructionState)
	{
		ErectusMemory::oldWeaponListCounter++;
		if (ErectusMemory::oldWeaponListCounter > 60)
		{
			ErectusMemory::oldWeaponListCounter = 0;
			if (!ErectusMemory::oldWeaponListUpdated)
				ErectusMemory::oldWeaponListUpdated = ErectusMemory::UpdateOldWeaponData();

			if (ErectusMemory::oldWeaponListUpdated)
			{
				auto revertWeapons = true;
				if (ErectusMemory::WeaponEditingEnabled())
					revertWeapons = false;

				for (auto i = 0; i < ErectusMemory::oldWeaponListSize; i++)
				{
					if (threadDestructionState)
						break;
					ErectusMemory::EditWeapon(i, revertWeapons);
				}
			}
			ErectusMemory::InfiniteAmmo(Settings::weapons.infiniteAmmo);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	if (ErectusMemory::oldWeaponListUpdated)
	{
		for (auto i = 0; i < ErectusMemory::oldWeaponListSize; i++)
		{
			ErectusMemory::EditWeapon(i, true);
		}
	}

	ErectusMemory::InfiniteAmmo(false);

	ErectusMemory::DeleteOldWeaponList();

	weaponEditorThreadActive = false;

	return 0xDEAD;
}

DWORD WINAPI Threads::LockingThread([[maybe_unused]] LPVOID lpParameter)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	auto weaponIdRefreshCooldown = 0;

	DWORD weaponId = 0;
	BYTE shotsHit = 1;
	BYTE shotsFired = 0;

	DWORD64 targetingPage = 0;
	auto targetingPageValid = false;

	auto sendDamageCounter = 0;
	auto sendDamageThreshold = 0;

	auto targetLockingKeyPressed = false;
	auto targetLockingCooldown = 0;

	while (!threadDestructionState)
	{
		
		if (Settings::targetting.lockNpCs)
		{
			auto currentTargetValid = false;
			DWORD64 closestEntityPtr = 0;
			auto closestEntityDegrees = Settings::targetting.lockingFov;

			//refresh weaponId every 60 loops
			if (weaponIdRefreshCooldown == 0)
			{
				weaponIdRefreshCooldown = 60;
				weaponId = ErectusMemory::GetFavoritedWeaponId(BYTE(Settings::targetting.favoriteIndex));
			}
			weaponIdRefreshCooldown--;
			
			if (gApp->mode == App::Mode::Overlay && GetAsyncKeyState('T'))
			{
				targetLockingKeyPressed = true;
				if (targetLockingCooldown > 0)
					targetLockingCooldown--;
			}
			else
			{
				targetLockingKeyPressed = false;
				targetLockingCooldown = 0;
				ErectusMemory::targetLockedEntityPtr = 0;
			}

			if (ErectusMemory::targetLockedEntityPtr || targetLockingKeyPressed)
			{
				auto cameraData = ErectusMemory::GetCameraInfo();
				auto entities = ErectusMemory::entityDataBuffer;
				for (const auto& entity : entities)
				{
					if (!(entity.flag & CUSTOM_ENTRY_NPC) || !Settings::targetting.lockNpCs)
						continue;
					
					TesObjectRefr entityData{};
					if (!ErectusProcess::Rpm(entity.entityPtr, &entityData, sizeof entityData))
						continue;

					if (!ErectusMemory::IsTargetValid(entityData))
						continue;

					if (entity.entityPtr == ErectusMemory::targetLockedEntityPtr)
					{
						currentTargetValid = true;
					}
					else if (targetLockingKeyPressed && !targetLockingCooldown)
					{
						auto degrees = Utils::GetDegrees(entityData.position, cameraData.forward, cameraData.origin);
						if (degrees < closestEntityDegrees)
						{
							closestEntityDegrees = degrees;
							closestEntityPtr     = entity.entityPtr;
						}
					}
				}
			}

			if (!currentTargetValid)
			{
				if (ErectusMemory::targetLockedEntityPtr)
				{
					targetLockingCooldown = Settings::targetting.retargeting ? Settings::targetting.cooldown : -1;
					ErectusMemory::targetLockedEntityPtr = 0;
				}
				else if (closestEntityDegrees < Settings::targetting.lockingFov)
				{
					targetLockingCooldown = 0;
					ErectusMemory::targetLockedEntityPtr = closestEntityPtr;
				}
			}

			if (ErectusMemory::targetLockedEntityPtr)
			{
				ErectusMemory::DamageRedirection(ErectusMemory::targetLockedEntityPtr, &targetingPage, &targetingPageValid, false, true);

				sendDamageCounter++;
				if (sendDamageCounter > sendDamageThreshold)
				{
					sendDamageCounter = 0;
					sendDamageThreshold = Utils::GetRangedInt(Settings::targetting.sendDamageMin, Settings::targetting.sendDamageMax);
					ErectusMemory::SendDamage(ErectusMemory::targetLockedEntityPtr, weaponId, &shotsHit, &shotsFired, 1);
				}
			}
			else
			{
				ErectusMemory::DamageRedirection(ErectusMemory::targetLockedEntityPtr, &targetingPage, &targetingPageValid, false, false);
				sendDamageThreshold = 0;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	ErectusMemory::DamageRedirection(ErectusMemory::targetLockedEntityPtr, &targetingPage, &targetingPageValid, true, false);

	if (targetingPage)
		ErectusProcess::FreeEx(targetingPage);

	lockingThreadActive = false;

	return 0xCAFE;
}

DWORD WINAPI Threads::MultihackThread([[maybe_unused]] LPVOID lpParameter)
{
	auto meleeCounter = 0;
	auto meleeThreshold = 0;

	DWORD64 actorValuePage = 0;
	auto actorValuePageValid = false;

	DWORD64 opkPage = 0;
	auto opkPageValid = false;

	DWORD64 freezeApPage = 0;
	auto freezeApPageValid = false;

	auto loopCount = -1;
	while (!threadDestructionState)
	{
		loopCount = (loopCount + 1) % 30000;

		if (positionSpoofingToggle && Settings::localPlayer.positionSpoofingEnabled && Settings::localPlayer.clientState)
			ErectusMemory::SetClientState(2);

		if (noclipToggle && Settings::localPlayer.noclipEnabled && Settings::localPlayer.clientState)
			ErectusMemory::SetClientState(2);

		if ((positionSpoofingToggle || noclipToggle) && Settings::localPlayer.automaticClientState)
		{
			if (loopCount % 60 == 0) //every 60 loops
				ErectusMemory::SetClientState(2);
		}

		ErectusMemory::PositionSpoofing(positionSpoofingToggle);
		ErectusMemory::Noclip(noclipToggle);

		ErectusMemory::ActorValue(&actorValuePage, &actorValuePageValid, true);
		if (loopCount % 60 == 0) { //every 60 loops
			ErectusMemory::SetActorValueMaximum(0x000002C2, 100.0f, static_cast<float>(Settings::localPlayer.strength), Settings::localPlayer.strengthEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C3, 100.0f, static_cast<float>(Settings::localPlayer.perception), Settings::localPlayer.perceptionEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C4, 100.0f, static_cast<float>(Settings::localPlayer.endurance), Settings::localPlayer.enduranceEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C5, 100.0f, static_cast<float>(Settings::localPlayer.charisma), Settings::localPlayer.charismaEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C6, 100.0f, static_cast<float>(Settings::localPlayer.agility), Settings::localPlayer.agilityEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C7, 100.0f, static_cast<float>(Settings::localPlayer.intelligence), Settings::localPlayer.intelligenceEnabled);
			ErectusMemory::SetActorValueMaximum(0x000002C8, 100.0f, static_cast<float>(Settings::localPlayer.luck), Settings::localPlayer.luckEnabled);
		}

		ErectusMemory::FreezeActionPoints(&freezeApPage, &freezeApPageValid, true);
		ErectusMemory::OnePositionKill(&opkPage, &opkPageValid, true);

		if (opkPageValid)
		{
			if (opkNpcsToggle)
				ErectusMemory::SetOpkData(opkPage, true);
			else
				ErectusMemory::SetOpkData(opkPage, false);
		}

		if (Settings::customNukeCodeSettings.automaticNukeCodes)
		{
			if (loopCount % 300 == 0) { //every 300 loops
				ErectusMemory::UpdateNukeCodes();
			}
		}

		if (loopCount % 10 == 0) //every 10 loops
			ErectusMemory::ChargenEditing();

		if (Settings::melee.enabled)
		{
			if (gApp->mode == App::Mode::Overlay && GetAsyncKeyState('U'))
			{
				meleeCounter++;
				if (meleeCounter > meleeThreshold)
				{
					meleeCounter = 0;
					meleeThreshold = Utils::GetRangedInt(Settings::melee.speedMin, Settings::melee.speedMax);
					ErectusMemory::MeleeAttack();
				}
			}
			else
				meleeThreshold = 0;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	ErectusMemory::PositionSpoofing(false);
	ErectusMemory::Noclip(false);
	ErectusMemory::ActorValue(&actorValuePage, &actorValuePageValid, false);

	if (actorValuePage)
		ErectusProcess::FreeEx(actorValuePage);

	ErectusMemory::SetActorValueMaximum(0x000002C2, 100.0f, static_cast<float>(Settings::localPlayer.strength), false);
	ErectusMemory::SetActorValueMaximum(0x000002C3, 100.0f, static_cast<float>(Settings::localPlayer.perception), false);
	ErectusMemory::SetActorValueMaximum(0x000002C4, 100.0f, static_cast<float>(Settings::localPlayer.endurance), false);
	ErectusMemory::SetActorValueMaximum(0x000002C5, 100.0f, static_cast<float>(Settings::localPlayer.charisma), false);
	ErectusMemory::SetActorValueMaximum(0x000002C6, 100.0f, static_cast<float>(Settings::localPlayer.agility), false);
	ErectusMemory::SetActorValueMaximum(0x000002C7, 100.0f, static_cast<float>(Settings::localPlayer.intelligence), false);
	ErectusMemory::SetActorValueMaximum(0x000002C8, 100.0f, static_cast<float>(Settings::localPlayer.luck), false);

	ErectusMemory::OnePositionKill(&opkPage, &opkPageValid, false);

	if (opkPage)
		ErectusProcess::FreeEx(opkPage);

	ErectusMemory::FreezeActionPoints(&freezeApPage, &freezeApPageValid, false);

	if (freezeApPage)
		ErectusProcess::FreeEx(freezeApPage);

	multihackThreadActive = false;

	return 0xBEAD;
}

DWORD WINAPI Threads::Looter([[maybe_unused]] LPVOID lpParameter)
{
	while (!threadDestructionState)
	{
		Looter::Loot();
		std::this_thread::sleep_for(std::chrono::milliseconds(Utils::GetRangedInt(36, 72) * 16));
	}

	looterThreadActive = false;

	return 0xDEAF;
}

bool Threads::CreateProcessThreads()
{
	if (threadDestructionQueued)
		return false;

	if (threadDestructionState)
		return false;

	if (threadDestructionCounter)
		return false;

	if (!bufferEntityListThreadActive)
	{
		bufferEntityListThreadActive = CloseHandle(CreateThread(nullptr, 0, &BufferEntityListThread, nullptr, 0, nullptr));
		if (!bufferEntityListThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	if (!bufferPlayerListThreadActive)
	{
		bufferPlayerListThreadActive = CloseHandle(CreateThread(nullptr, 0, &BufferPlayerListThread, nullptr, 0, nullptr));
		if (!bufferPlayerListThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	if (!weaponEditorThreadActive)
	{
		weaponEditorThreadActive = CloseHandle(CreateThread(nullptr, 0, &WeaponEditorThread, nullptr, 0, nullptr));
		if (!weaponEditorThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	if (!lockingThreadActive)
	{
		lockingThreadActive = CloseHandle(CreateThread(nullptr, 0, &LockingThread, nullptr, 0, nullptr));
		if (!lockingThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	if (!multihackThreadActive)
	{
		multihackThreadActive = CloseHandle(CreateThread(nullptr, 0, &MultihackThread, nullptr, 0, nullptr));
		if (!multihackThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	if (!looterThreadActive)
	{
		looterThreadActive = CloseHandle(CreateThread(nullptr, 0, &Looter, nullptr, 0, nullptr));
		if (!looterThreadActive)
		{
			threadDestructionQueued = true;
			return false;
		}
	}

	return true;
}

bool Threads::ThreadDestruction()
{
	threadDestructionState = true;

	if (bufferEntityListThreadActive)
		return false;

	if (bufferPlayerListThreadActive)
		return false;

	if (weaponEditorThreadActive)
		return false;

	if (lockingThreadActive)
		return false;

	if (multihackThreadActive)
		return false;

	if (looterThreadActive)
		return false;

	threadCreationState = false;
	threadDestructionQueued = false;
	threadDestructionState = false;
	threadDestructionCounter = 0;

	return true;
}
