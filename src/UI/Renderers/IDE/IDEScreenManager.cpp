#include "UI/Renderers/IDE/IDEScreenManager.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEOpeningScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEMenuScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDENameScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDERaceScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEClassScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEDifficultyScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEParryScreen.h"
#include "UI/Renderers/IDE/IDEScreens/Menu/IDEIntroductionScreen.h"
#include "UI/Screens/Menu/TutorialScreen.h"

void IDEScreenManager::opening() {
    IDEOpeningScreen::display();
}

void IDEScreenManager::panelLogo(const std::string& screenTitle, bool animateFadeIn) {
    IDEMenuScreen::displayGameLogoPanel(screenTitle, animateFadeIn);
}

bool IDEScreenManager::chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) {
    return IDEMenuScreen::displayChooseConfirmationWithArtSideBySide(chooseType, chooseName, informationForDisplay, asciiArtForDisplay);
}

std::vector<std::string> IDEScreenManager::frameAttributes(const Attributes& stats,
    const std::string& dryTitle, const std::string& skillTitle,
    const std::string& skillName, const std::string& skillDesc,
    const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) {
    return IDEMenuScreen::composeAttributesFrame(stats, dryTitle, skillTitle, skillName, skillDesc, skillTitle2, skillName2, skillDesc2);
}

int IDEScreenManager::mainMenu() {
    return IDEMenuScreen::displayMainMenuOptions();
}

void IDEScreenManager::tutorialParry(const std::string& infoBox) {
    TutorialScreen::displayParryTutorial(infoBox);
}

NameScreen::Result IDEScreenManager::screenName() {
    return IDENameScreen::display();
}

RaceScreen::Result IDEScreenManager::screenRace(const std::string& characterName) {
    return IDERaceScreen::display(characterName);
}

ClassScreen::Result IDEScreenManager::screenClass(const std::string& characterName, const std::string& race) {
    return IDEClassScreen::display(characterName, race);
}

DifficultyScreen::Result IDEScreenManager::screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return IDEDifficultyScreen::display(characterName, race, classObj);
}

ParryScreen::Result IDEScreenManager::screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return IDEParryScreen::display(characterName, race, classObj);
}

void IDEScreenManager::screenIntroduction() {
    IDEIntroductionScreen::display();
}

bool IDEScreenManager::confirmExit() {
    return IDEMenuScreen::displayConfirmationExit();
}
