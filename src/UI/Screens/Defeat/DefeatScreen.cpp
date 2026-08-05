#include "UI/Screens/Defeat/DefeatScreen.h"
#include "UI/PerspectiveManager.h"

void ScreenDefeat::display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat)
{
    PerspectiveManager::getDefeatUI().display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat);
}
