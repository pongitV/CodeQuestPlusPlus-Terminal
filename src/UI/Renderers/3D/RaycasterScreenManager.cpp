#include "UI/Renderers/3D/RaycasterScreenManager.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterOpeningScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterMenuScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterNameScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterRaceScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterClassScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterDifficultyScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterParryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterIntroductionScreen.h"
#include "UI/Screens/Menu/TutorialScreen.h"

void RaycasterScreenManager::opening() {
    ScreenOpeningRaycaster::display();
}

void RaycasterScreenManager::panelLogo(const std::string& screenTitle, bool animateFadeIn) {
    ScreenMenuRaycaster::displayGameLogoPanel(screenTitle, animateFadeIn);
}

bool RaycasterScreenManager::chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) {
    return ScreenMenuRaycaster::displayChooseConfirmationWithArtSideBySide(chooseType, chooseName, informationForDisplay, asciiArtForDisplay);
}

std::vector<std::string> RaycasterScreenManager::frameAttributes(const Attributes& stats,
    const std::string& dryTitle, const std::string& skillTitle,
    const std::string& skillName, const std::string& skillDesc,
    const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) {
    return ScreenMenuRaycaster::composeAttributesFrame(stats, dryTitle, skillTitle, skillName, skillDesc, skillTitle2, skillName2, skillDesc2);
}

int RaycasterScreenManager::mainMenu() {
    return ScreenMenuRaycaster::displayMainMenuOptions();
}

void RaycasterScreenManager::tutorialParry(const std::string& infoBox) {
    TutorialScreen::displayParryTutorial(infoBox); // Tutorial nao e separado ainda
}

NameScreen::Result RaycasterScreenManager::screenName() {
    return ScreenNameRaycaster::display();
}

RaceScreen::Result RaycasterScreenManager::screenRace(const std::string& characterName) {
    return ScreenRaceRaycaster::display(characterName);
}

ClassScreen::Result RaycasterScreenManager::screenClass(const std::string& characterName, const std::string& race) {
    return ScreenClassRaycaster::display(characterName, race);
}

DifficultyScreen::Result RaycasterScreenManager::screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return ScreenDifficultyRaycaster::display(characterName, race, classObj);
}

ParryScreen::Result RaycasterScreenManager::screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return ScreenParryRaycaster::display(characterName, race, classObj);
}

void RaycasterScreenManager::screenIntroduction() {
    ScreenIntroductionRaycaster::display();
}

bool RaycasterScreenManager::confirmExit() {
    return ScreenMenuRaycaster::displayConfirmationExit();
}
