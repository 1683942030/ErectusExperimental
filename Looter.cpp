﻿#include "Looter.h"

#include <memory>

#include "common.h"
#include "ErectusProcess.h"
#include "MsgSender.h"
#include "settings.h"
#include "utils.h"

bool Looter::ShouldLootJunk(const ItemInfo& item)
{
	if (!item.base.componentArraySize || item.base.componentArraySize > 0x7FFF)
		return false;

	if (!Utils::Valid(item.base.componentArrayPtr))
		return false;

	auto components = std::make_unique<Component[]>(item.base.componentArraySize);
	if (!ErectusProcess::Rpm(item.base.componentArrayPtr, components.get(), item.base.componentArraySize * sizeof(Component)))
		return false;

	for (auto i = 0; i < item.base.componentArraySize; i++)
	{
		if (!Utils::Valid(components[i].componentReferencePtr))
			continue;
		if (!Utils::Valid(components[i].componentCountReferencePtr))
			continue;

		TesItem componentData{};
		if (!ErectusProcess::Rpm(components[i].componentReferencePtr, &componentData, sizeof componentData))
			continue;

		if (Settings::looter.selection.junk.components.contains(componentData.formId) && Settings::looter.selection.junk.components.find(componentData.formId)->second)
			return true;
	}

	return false;
}

bool Looter::ShouldLootFloraLeveled(const LeveledList& list)
{
	if (!Utils::Valid(list.listEntryArrayPtr) || !list.listEntryArraySize)
		return false;

	auto listEntryData = std::make_unique<ListEntry[]>(list.listEntryArraySize);

	if (!ErectusProcess::Rpm(list.listEntryArrayPtr, listEntryData.get(), list.listEntryArraySize * sizeof(ListEntry)))
		return false;

	for (BYTE i = 0; i < list.listEntryArraySize; i++)
	{
		if (!Utils::Valid(listEntryData[i].referencePtr))
			continue;

		TesItem referenceData{};
		if (!ErectusProcess::Rpm(listEntryData[i].referencePtr, &referenceData, sizeof referenceData))
			continue;
		if (referenceData.formType == static_cast<BYTE>(FormTypes::TesLevItem))
		{
			LeveledList innerList{};
			memcpy(&innerList, &referenceData, sizeof innerList);
			if (ShouldLootFloraLeveled(innerList))
				return true;
		}
		else if (Settings::looter.selection.flora.components.contains(referenceData.formId) && Settings::looter.selection.flora.components.find(referenceData.formId)->second)
			return true;
	}

	return false;
}

bool Looter::ShouldLootFlora(const ItemInfo& item)
{
	if (item.base.formId == 0x000183C6)
		return Settings::looter.selection.flora.components.find(item.base.formId)->second;

	if (!Utils::Valid(item.base.harvestedPtr))
		return false;

	TesItem harvestedData{};
	if (!ErectusProcess::Rpm(item.base.harvestedPtr, &harvestedData, sizeof harvestedData))
		return false;
	if (harvestedData.formType == static_cast<BYTE>(FormTypes::TesLevItem))
	{
		LeveledList leveledListData{};
		memcpy(&leveledListData, &harvestedData, sizeof leveledListData);
		return ShouldLootFloraLeveled(leveledListData);
	}
	return Settings::looter.selection.flora.components.contains(harvestedData.formId) && Settings::looter.selection.flora.components.find(harvestedData.formId)->second;
}

bool Looter::ShouldLootItem(const ItemInfo& item, const DWORD64 displayPtr = 0)
{
	BYTE rank;

	if (auto found = Settings::looter.selection.blacklist.find(item.base.formId); found != Settings::looter.selection.blacklist.end()) {
		if (found->second)
			return false;
	}

	if (auto found = Settings::looter.selection.whitelist.find(item.base.formId); found != Settings::looter.selection.whitelist.end()) {
		if (found->second)
			return true;
	}

	switch (item.type)
	{
	case ItemTypes::Weapons:
		if (Settings::looter.selection.weapons.all)
			return true;
		rank = GetLegendaryRank(displayPtr);
		return Settings::looter.selection.weapons.oneStar && rank >= 1 || Settings::looter.selection.weapons.twoStar && rank >= 2 || Settings::looter.selection.weapons.threeStar && rank >= 3;
	case ItemTypes::Apparel:
		if (Settings::looter.selection.apparel.all)
			return true;
		rank = GetLegendaryRank(displayPtr);
		return Settings::looter.selection.apparel.oneStar && rank >= 1 || Settings::looter.selection.apparel.twoStar && rank >= 2 || Settings::looter.selection.apparel.threeStar && rank >= 3;
	case ItemTypes::Misc:
		return Settings::looter.selection.misc.all;
	case ItemTypes::Aid:
		return Settings::looter.selection.aid.all;
	case ItemTypes::AidBobblehead:
		return Settings::looter.selection.aid.all || Settings::looter.selection.aid.bobbleheads;
	case ItemTypes::AidMagazine:
		return Settings::looter.selection.aid.all || Settings::looter.selection.aid.magazines;
	case ItemTypes::Notes:
		return Settings::looter.selection.notes.all;
	case ItemTypes::NotesKnownPlan:
		return Settings::looter.selection.notes.all || Settings::looter.selection.notes.plansKnown;
	case ItemTypes::NotesUnknownPlan:
		return Settings::looter.selection.notes.all || Settings::looter.selection.notes.plansUnknown;
	case ItemTypes::NotesTreasureMap:
		return Settings::looter.selection.notes.all || Settings::looter.selection.notes.treasureMaps;
	case ItemTypes::Junk:
		return Settings::looter.selection.junk.all || ShouldLootJunk(item);
	case ItemTypes::Flora:
		return Settings::looter.selection.flora.all || ShouldLootFlora(item);
	case ItemTypes::Mod:
		return Settings::looter.selection.mods.all;
	case ItemTypes::Ammo:
		return Settings::looter.selection.ammo.all;
	case ItemTypes::Currency:
		return Settings::looter.selection.other.caps;
	default:
		return false;
	}
}

void Looter::LootGroundItem(const ItemInfo& item, const TesObjectRefr& player)
{
	if (!MsgSender::IsEnabled())
		return;

	if (!Settings::looter.looters.groundItems)
		return;

	if (!ShouldLootItem(item))
		return;

	if (Utils::GetDistance(item.refr.position, player.position) * 0.01f > 76.f)
		return;

	RequestActivateRefMessage requestActivateRefMessageData{
		.vtable = ErectusProcess::exe + VTABLE_REQUESTACTIVATEREFMSG,
		.formId = item.refr.formId,
		.choice = 0xFF,
		.forceActivate = 0
	};
	MsgSender::Send(&requestActivateRefMessageData, sizeof requestActivateRefMessageData);
}

void Looter::LootContainer(const ItemInfo& item, const TesObjectRefr& player)
{
	if (!MsgSender::IsEnabled())
		return;

	switch (item.type)
	{
	case ItemTypes::Npc:
		if (!Settings::looter.looters.npcs)
			return;
		
		if (item.refr.formId == 0x00000007 || ErectusMemory::CheckHealthFlag(item.refr.healthFlag) != 0x3)
			return;
		if (Utils::GetDistance(item.refr.position, player.position) * 0.01f > 76.f)
			return;
		break;

	case ItemTypes::Container:
		if (!Settings::looter.looters.containers)
			return;
		
		if (Utils::GetDistance(item.refr.position, player.position) * 0.01f > 6.f)
			return;
		if (!ContainerValid(item.base))
			return;
		break;

	default:
		return;
	}

	if (!Utils::Valid(item.refr.inventoryPtr))
		return;

	Inventory inventory{};
	if (!ErectusProcess::Rpm(item.refr.inventoryPtr, &inventory, sizeof inventory))
		return;
	if (!Utils::Valid(inventory.entryArrayBegin) || inventory.entryArrayEnd <= inventory.entryArrayBegin)
		return;

	auto entryArraySize = (inventory.entryArrayEnd - inventory.entryArrayBegin) / sizeof(InventoryEntry);
	auto entries = std::make_unique<InventoryEntry[]>(entryArraySize);
	if (!ErectusProcess::Rpm(inventory.entryArrayBegin, entries.get(), entryArraySize * sizeof(InventoryEntry)))
		return;

	for (DWORD64 i = 0; i < entryArraySize; i++)
	{
		if (!Utils::Valid(entries[i].referencePtr))
			continue;
		if (!Utils::Valid(entries[i].displayPtr) || entries[i].iterations < entries[i].displayPtr) //???
			continue;

		TesItem baseItem{};
		if (!ErectusProcess::Rpm(entries[i].referencePtr, &baseItem, sizeof baseItem))
			continue;
		if (baseItem.recordFlagA >> 2 & 1) //???
			continue;

		auto inventoryItem = ErectusMemory::GetItemInfo(TesObjectRefr(), baseItem);
		if (!ShouldLootItem(inventoryItem, entries[i].displayPtr))
			continue;

		auto iterations = (entries[i].iterations - entries[i].displayPtr) / sizeof(ItemCount);
		if (!iterations || iterations > 0x7FFF)
			continue;

		auto itemCount = std::make_unique<ItemCount[]>(iterations);
		if (!ErectusProcess::Rpm(entries[i].displayPtr, itemCount.get(), iterations * sizeof(ItemCount)))
			continue;

		auto count = 0;
		for (DWORD64 c = 0; c < iterations; c++)
		{
			count += itemCount[c].count;
		}

		if (count == 0)
			continue;

		TransferMessage transferMessageData = {
			.vtable = ErectusProcess::exe + VTABLE_REQUESTTRANSFERITEMMSG,
			.sourceEntityId = item.refr.formId,
			.playerEntityId = 0,
			.bShouldSendResult = false,
			.destEntityId = player.formId,
			.itemServerHandleId = entries[i].itemId,
			.stashAccessEntityId = 0x00000000,
			.bCreateIfMissing = false,
			.bIsExpectingResult = false,
			.count = count,
		};
		MsgSender::Send(&transferMessageData, sizeof transferMessageData);
	}
}

void Looter::LootFlora(const ItemInfo& item, const TesObjectRefr& player)
{
	if (!MsgSender::IsEnabled())
		return;

	if (!Settings::looter.looters.flora)
		return;

	if (ErectusMemory::IsFloraHarvested(item.refr.harvestFlagA, item.refr.harvestFlagB))
		return;

	if (Utils::GetDistance(item.refr.position, player.position) * 0.01f > 6.f)
		return;

	if (!ShouldLootItem(item))
		return;

	RequestActivateRefMessage requestActivateRefMessageData{
		.vtable = ErectusProcess::exe + VTABLE_REQUESTACTIVATEREFMSG,
		.formId = item.refr.formId,
		.choice = 0xFF,
		.forceActivate = 0
	};
	MsgSender::Send(&requestActivateRefMessageData, sizeof requestActivateRefMessageData);
}


void Looter::Loot()
{
	if (!MsgSender::IsEnabled())
		return;

	if (Settings::looter.mode == LooterSettings::Mode::Disabled || Settings::looter.mode == LooterSettings::Mode::Keybind && !lootItemsRequested)
		return;
	if (!Settings::looter.looters.npcs && !Settings::looter.looters.groundItems && !Settings::looter.looters.containers && !Settings::looter.looters.flora)
		return;
	if (!Settings::looter.selection.IsEnabled())
		return;

	auto localPlayerPtr = ErectusMemory::GetLocalPlayerPtr(true);
	if (!Utils::Valid(localPlayerPtr))
		return;

	TesObjectRefr localPlayer{};
	if (!ErectusProcess::Rpm(localPlayerPtr, &localPlayer, sizeof localPlayer))
		return;

	auto entityPtrs = ErectusMemory::GetEntityPtrList();
	for (const auto& entityPtr : entityPtrs)
	{
		TesObjectRefr entity{};
		if (!ErectusProcess::Rpm(entityPtr, &entity, sizeof entity))
			continue;

		if (!Utils::Valid(entity.baseObjectPtr))
			continue;

		if (entity.spawnFlag != 0x02)
			continue;

		switch (entity.formType)
		{
		case (static_cast<BYTE>(FormTypes::TesActor)):
			if (!Settings::looter.looters.npcs || ErectusMemory::CheckHealthFlag(entity.healthFlag) != 0x3)
				continue;
			break;
		case (static_cast<BYTE>(FormTypes::TesObjectRefr)):
			if (!Settings::looter.looters.groundItems && !Settings::looter.looters.containers && !Settings::looter.looters.flora)
				continue;
			break;
		default:
			continue;
		}

		TesItem baseItem{};
		if (!ErectusProcess::Rpm(entity.baseObjectPtr, &baseItem, sizeof baseItem))
			continue;

		auto item = ErectusMemory::GetItemInfo(entity, baseItem);
		switch (item.type)
		{
		case ItemTypes::Npc:
		case ItemTypes::Container:
			LootContainer(item, localPlayer);
			break;
		case ItemTypes::Flora:
			LootFlora(item, localPlayer);
			break;
		case ItemTypes::Invalid:
		case ItemTypes::Other:
			break;
		default:
			LootGroundItem(item, localPlayer);
		}
	}
	lootItemsRequested = false;
}

bool Looter::ContainerValid(const TesItem& referenceData)
{
	if (!Utils::Valid(referenceData.keywordArrayData00C0))
		return false;

	int nifTextLength;
	if (!ErectusProcess::Rpm(referenceData.keywordArrayData00C0 + 0x10, &nifTextLength, sizeof nifTextLength))
		return false;
	if (nifTextLength == 41)
	{
		char containerMarkerCheck[sizeof"ContainerMarker"];
		if (!ErectusProcess::Rpm(referenceData.keywordArrayData00C0 + 0x2E, &containerMarkerCheck, sizeof containerMarkerCheck))
			return false;

		containerMarkerCheck[15] = '\0';
		if (!strcmp(containerMarkerCheck, "ContainerMarker"))
			return false;
	}

	if (!Utils::Valid(referenceData.namePtr00B0))
		return false;

	DWORD64 nameBuffer;
	if (!ErectusProcess::Rpm(referenceData.namePtr00B0 + 0x10, &nameBuffer, sizeof nameBuffer))
		return false;
	if (!nameBuffer)
		return false;

	if (!Utils::Valid(nameBuffer))
		nameBuffer = referenceData.namePtr00B0;

	int nameTextLength;
	if (!ErectusProcess::Rpm(nameBuffer + 0x10, &nameTextLength, sizeof nameTextLength))
		return false;
	if (!nameTextLength || nameTextLength > 0x7FFF)
		return false;

	return true;
}

BYTE Looter::GetLegendaryRank(const DWORD64 displayPtr)
{
	if (!Utils::Valid(displayPtr))
		return 0;

	DWORD64 instancedArrayPtr;
	if (!ErectusProcess::Rpm(displayPtr, &instancedArrayPtr, sizeof instancedArrayPtr))
		return 0;
	if (!Utils::Valid(instancedArrayPtr))
		return 0;

	ItemInstancedArray itemInstancedArrayData{};
	if (!ErectusProcess::Rpm(instancedArrayPtr, &itemInstancedArrayData, sizeof itemInstancedArrayData))
		return 0;
	if (!Utils::Valid(itemInstancedArrayData.arrayPtr) || itemInstancedArrayData.arrayEnd < itemInstancedArrayData.arrayPtr)
		return 0;

	const auto instancedArraySize = (itemInstancedArrayData.arrayEnd - itemInstancedArrayData.arrayPtr) / sizeof(DWORD64);
	if (!instancedArraySize || instancedArraySize > 0x7FFF)
		return 0;

	auto instancedArray = std::make_unique<DWORD64[]>(instancedArraySize);
	if (!ErectusProcess::Rpm(itemInstancedArrayData.arrayPtr, instancedArray.get(), instancedArraySize * sizeof(DWORD64)))
		return 0;

	DWORD64 objectInstanceExtraPtr = 0;
	for (DWORD64 i = 0; i < instancedArraySize; i++)
	{
		if (!Utils::Valid(instancedArray[i]))
			continue;

		ExtraTextDisplayData extraTextDisplayDataData{};
		if (!ErectusProcess::Rpm(instancedArray[i], &extraTextDisplayDataData, sizeof extraTextDisplayDataData))
			continue;

		const auto rttiNamePtr = ErectusMemory::RttiGetNamePtr(extraTextDisplayDataData.vtable);
		if (!rttiNamePtr)
			continue;

		char rttiNameCheck[sizeof".?AVBGSObjectInstanceExtra@@"];
		if (!ErectusProcess::Rpm(rttiNamePtr, &rttiNameCheck, sizeof rttiNameCheck))
			continue;
		if (strcmp(rttiNameCheck, ".?AVBGSObjectInstanceExtra@@") != 0)
			continue;

		objectInstanceExtraPtr = instancedArray[i];
		break;
	}
	if (!objectInstanceExtraPtr)
		return 0;

	ObjectInstanceExtra objectInstanceExtraData{};
	if (!ErectusProcess::Rpm(objectInstanceExtraPtr, &objectInstanceExtraData, sizeof objectInstanceExtraData))
		return 0;
	if (!Utils::Valid(objectInstanceExtraData.modDataPtr))
		return 0;

	ModInstance modInstanceData{};
	if (!ErectusProcess::Rpm(objectInstanceExtraData.modDataPtr, &modInstanceData, sizeof modInstanceData))
		return 0;
	if (!Utils::Valid(modInstanceData.modListPtr) || !modInstanceData.modListSize)
		return 0;

	const DWORD64 modArraySize = modInstanceData.modListSize / 0x8;
	if (!modArraySize || modArraySize > 0x7FFF)
		return 0;

	auto modArray = std::make_unique<DWORD[]>(modArraySize * 2);
	if (!ErectusProcess::Rpm(modInstanceData.modListPtr, modArray.get(), modArraySize * 2 * sizeof(DWORD)))
		return 0;

	BYTE legendaryRank = 0;
	for (DWORD64 i = 0; i < modArraySize; i++)
	{
		if (LEGENDARYEFFECT_FORMIDS.count(modArray[i * 2]) > 0)
			legendaryRank++;
	}
	return legendaryRank;
}
