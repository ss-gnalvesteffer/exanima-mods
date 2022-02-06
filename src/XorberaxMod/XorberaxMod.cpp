#include "pch.h"
#include "Tools.h"
#include "ItemRepository.h"
#include "ContainerItem.h"
#include "XorberaxMod.h"

const DWORD _textSectionBaseAddress = 0x401000;
auto _itemRepository = new ItemRepository();

void AddFullArsenalToPracticeArena()
{
    auto itemIds = _itemRepository->GetArsenalItemIds();
    auto totalItemIds = itemIds->size();

    // Overwrite total items to load.
    *(DWORD*)0x60BF0C = totalItemIds;

    // Build new item array.
    ContainerItem* containerItems = new ContainerItem[totalItemIds];
    for (int itemIndex = 0; itemIndex < totalItemIds; ++itemIndex)
    {
        containerItems[itemIndex].ItemID = itemIds->at(itemIndex);
    }

    // Make character customization screen point to new item array.
    unsigned char* containerItemAddressBytes = (unsigned char*)&containerItems;
    unsigned char patch[] = { 0xBD, containerItemAddressBytes[0], containerItemAddressBytes[1], containerItemAddressBytes[2], containerItemAddressBytes[3] };
    Tools::memcpy_s((PVOID)(_textSectionBaseAddress + 0xFFB4C), (char*)patch, 5);
}

void ForceCharacterCustomizationItemContainersToReloadOnNavigation()
{
    unsigned char patch[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
    Tools::memcpy_s((PVOID)(_textSectionBaseAddress + 0xFFAC0), (char*)patch, 6); // NOPs a conditional jump that checks if the weapon inventory container is already loaded, so that it reloads to grab new items.
}

void SuppressItemErrors()
{
    unsigned char patch[] = { 0xC3 };
    Tools::memcpy_s((PVOID)(_textSectionBaseAddress + 0x177794), (char*)patch, 1);
}

void XorberaxMod::Start()
{
    AddFullArsenalToPracticeArena();
    ForceCharacterCustomizationItemContainersToReloadOnNavigation();
    SuppressItemErrors();
}