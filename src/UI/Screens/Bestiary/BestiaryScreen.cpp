#include "UI/Screens/Bestiary/BestiaryScreen.h"
#include "UI/PerspectiveManager.h"

void ScreenBestiary::displayList([[maybe_unused]] Character* currentPlayer) {
    PerspectiveManager::getBestiaryUI().display({});
}

void ScreenBestiary::displaySheet([[maybe_unused]] Character* currentPlayer, [[maybe_unused]] const std::string& nameEnemy, [[maybe_unused]] int indexDiscovered, [[maybe_unused]] const std::vector<std::string>& discovered) {
    PerspectiveManager::getBestiaryUI().displayDetail(nullptr);
}
