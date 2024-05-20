#include "AutoTotem.h"
#include "../../ModuleManager.h"

using namespace std;

AutoTotem::AutoTotem() : IModule(0, Category::PLAYER, "Automatically puts items in your offhand")
{
	item.addEntry(EnumEntry("Vanilla", 0));
	registerEnumSetting("Mode", &item, 0);
	registerEnumSetting("Call", &calling, 0);
	calling.addEntry("Tick", 0);
	calling.addEntry("PreTick", 1);
	registerIntSetting("MinHealth", &health, health, 1, 20);
	registerBoolSetting("Health", &healthMode, healthMode);
	registerEnumSetting("Notify", &mode, 0);
	mode.addEntry("Off", 0);
	mode.addEntry("Chat", 1);
	mode.addEntry("Notification", 2);
}

const char *AutoTotem::getRawModuleName()
{
	return ("Offhand");
}
const char *AutoTotem::getModuleName()
{
	switch (item.getSelectedValue())
	{
	case 0:
		name = string("Offhand ") + string(GRAY) + string("Vanilla");
		break;
	case 1:
		name = string("Offhand ") + string(GRAY) + string("Inventory");
		break;
	}
	return name.c_str();
}

void AutoTotem::onTick(C_GameMode *gm)
{
	auto notifs = moduleMgr->getModule<Notifications>();
	C_PlayerInventoryProxy *supplies = g_Data.getLocalPlayer()->getSupplies();
	C_Inventory *inv = supplies->inventory;
	C_InventoryTransactionManager *manager = g_Data.getLocalPlayer()->getTransactionManager();
	C_ItemStack *current = g_Data.getLocalPlayer()->getEquippedTotem();

	bool run = false;
	bool setOffhand = false;
	int healthy = g_Data.getLocalPlayer()->getHealth();

	if (healthMode)
	{
		if (current->item == NULL && healthy < health)
			run = true;
	}

	if (!healthMode)
	{
		if (current->item == NULL)
			run = true;
	}

	if (run)
	{
		if (item.selected == 0 && calling.getSelectedValue() == 0)
		{
			for (int i = 0; i < 36; i++)
			{
				C_ItemStack *totem = inv->getItemStack(i);
				if (totem->item != NULL && (*totem->item)->itemId == 568)
				{
					C_InventoryAction first(i, totem, nullptr);
					C_InventoryAction second(37, nullptr, totem);
					g_Data.getLocalPlayer()->setOffhandSlot(totem);
					manager->addInventoryAction(first);
					manager->addInventoryAction(second);
					if (mode.getSelectedValue() == 1)
					{
						g_Data.getClientInstance()->getGuiData()->displayClientMessageF("<%sAutoTotem%s> %sAdding totem to offhand", GREEN, WHITE, DARK_GREEN);
					}
					if (mode.getSelectedValue() == 2)
					{
						auto notification = g_Data.addNotification("AutoTotem:", "Adding totem to offhand");
						notification->duration = notifs->enabledDur;
					}
					break;
				}
			}
		}
	}
	auto player = g_Data.getLocalPlayer();
	if (player == nullptr)
		return;

	if (item.getSelectedValue() == 0 && calling.getSelectedValue() == 1)
	{
		C_PlayerInventoryProxy* supplies = g_Data.getLocalPlayer()->getSupplies();
		C_Inventory* inv = supplies->inventory;
		C_ItemStack* current = g_Data.getLocalPlayer()->getEquippedTotem();
		if (current->item == NULL)
		{
			for (int i = 1; i < 36; i++)
			{
				C_ItemStack* totem = inv->getItemStack(i);
				if (totem->item != NULL && (*totem->item)->itemId == 568)
				{
					g_Data.getLocalPlayer()->setOffhandSlot(totem);
					inv->removeItem(i, 1);
					if (mode.getSelectedValue() == 1)
					{
						g_Data.getClientInstance()->getGuiData()->displayClientMessageF("<%sAutoTotem%s> %sAdding totem to offhand", GREEN, WHITE, DARK_GREEN);
					}
					if (mode.getSelectedValue() == 2)
					{
						auto notification = g_Data.addNotification("AutoTotem:", "Adding totem to offhand");
						notification->duration = notifs->enabledDur;
					}
					break;
				}
			}
		}
	}
}
void AutoTotem::onPreTick(C_GameMode *gm)
{
}