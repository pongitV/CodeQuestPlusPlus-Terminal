#include "UI/Screens/Bestiary/BestiaryScreen.h"
#include "UI/PerspectiveManager.h"

void ScreenBestiary::displayList(Character* currentPlayer) {
    PerspectiveManager::getBestiaryUI().display({});
}

void ScreenBestiary::displaySheet(Character* currentPlayer, const std::string& nameEnemy, int indexDiscovered, const std::vector<std::string>& discovered) {
    PerspectiveManager::getBestiaryUI().displayDetail(nullptr);
}
