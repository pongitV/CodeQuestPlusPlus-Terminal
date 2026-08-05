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

void ManagerScreensRaycaster::opening() {
    ScreenOpeningRaycaster::display();
}

void ManagerScreensRaycaster::panelSoon(const std::string& titleDaScreen, bool animateFadeIn) {
    ScreenMenuRaycaster::displayPanelSoonGame(titleDaScreen, animateFadeIn);
}

bool ManagerScreensRaycaster::confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) {
    return ScreenMenuRaycaster::displayConfirmationDeChooseWithArtSideASide(typeDeChoose, nameDaChoose, informationForDisplay, artAsciiForDisplay);
}

std::vector<std::string> ManagerScreensRaycaster::frameAttributes(const Attributes& stats,
    const std::string& titleDry, const std::string& titleSkill,
    const std::string& nameHab, const std::string& descHab,
    const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) {
    return ScreenMenuRaycaster::composeFrameDeAttributes(stats, titleDry, titleSkill, nameHab, descHab, titleSkill2, nameHab2, descHab2);
}

int ManagerScreensRaycaster::mainMenu() {
    return ScreenMenuRaycaster::displayMainMenuOptions();
}

void ManagerScreensRaycaster::tutorialParry(const std::string& infoBox) {
    ScreenTutorial::displayTutorialDeParry(infoBox); // Tutorial não é separado ainda
}

NameScreen::Result ManagerScreensRaycaster::screenName() {
    return ScreenNameRaycaster::display();
}

RaceScreen::Result ManagerScreensRaycaster::screenRace(const std::string& nameCharacter) {
    return ScreenRaceRaycaster::display(nameCharacter);
}

ClassScreen::Result ManagerScreensRaycaster::screenClass(const std::string& nameCharacter, const std::string& race) {
    return ScreenClassRaycaster::display(nameCharacter, race);
}

DifficultyScreen::Result ManagerScreensRaycaster::screenDifficulty(const std::string& nameCharacter, const std::string& race, const std::string& classObj) {
    return ScreenDifficultyRaycaster::display(nameCharacter, race, classObj);
}

ParryScreen::Result ManagerScreensRaycaster::screenParry(const std::string& nameCharacter, const std::string& race, const std::string& classObj) {
    return ScreenParryRaycaster::display(nameCharacter, race, classObj);
}

void ManagerScreensRaycaster::screenIntroduction() {
    ScreenIntroductionRaycaster::display();
}

bool ManagerScreensRaycaster::confirmExit() {
    return ScreenMenuRaycaster::displayConfirmationExit();
}
