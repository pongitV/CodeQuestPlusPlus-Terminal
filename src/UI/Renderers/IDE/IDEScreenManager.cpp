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

void ManagerScreensGO::opening() {
    ScreenOpeningGO::display();
}

void ManagerScreensGO::panelSoon(const std::string& titleDaScreen, bool animateFadeIn) {
    ScreenMenuGO::displayPanelSoonGame(titleDaScreen, animateFadeIn);
}

bool ManagerScreensGO::confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) {
    return ScreenMenuGO::displayConfirmationDeChooseWithArtSideASide(typeDeChoose, nameDaChoose, informationForDisplay, artAsciiForDisplay);
}

std::vector<std::string> ManagerScreensGO::frameAttributes(const Attributes& stats,
    const std::string& titleDry, const std::string& titleSkill,
    const std::string& nameHab, const std::string& descHab,
    const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) {
    return ScreenMenuGO::composeFrameDeAttributes(stats, titleDry, titleSkill, nameHab, descHab, titleSkill2, nameHab2, descHab2);
}

int ManagerScreensGO::mainMenu() {
    return ScreenMenuGO::displayMainMenuOptions();
}

void ManagerScreensGO::tutorialParry(const std::string& infoBox) {
    ScreenTutorial::displayTutorialDeParry(infoBox);
}

NameScreen::Result ManagerScreensGO::screenName() {
    return ScreenNameGO::display();
}

RaceScreen::Result ManagerScreensGO::screenRace(const std::string& characterName) {
    return ScreenRaceGO::display(characterName);
}

ClassScreen::Result ManagerScreensGO::screenClass(const std::string& characterName, const std::string& race) {
    return ScreenClassGO::display(characterName, race);
}

DifficultyScreen::Result ManagerScreensGO::screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return ScreenDifficultyGO::display(characterName, race, classObj);
}

ParryScreen::Result ManagerScreensGO::screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) {
    return ScreenParryGO::display(characterName, race, classObj);
}

void ManagerScreensGO::screenIntroduction() {
    ScreenIntroductionGO::display();
}

bool ManagerScreensGO::confirmExit() {
    return ScreenMenuGO::displayConfirmationExit();
}
