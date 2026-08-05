#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/Screens/Menu/TutorialScreen.h"
#include "UI/Screens/ScreenRegistry.h"

void MenuScreen::displayPanelSoonGame(const std::string& titleDaScreen, bool animateFadeIn) {
    ScreenRegistration::panelSoon(titleDaScreen, animateFadeIn);
}

bool MenuScreen::displayConfirmationDeChooseWithArtSideASide(const std::string& typeDeChoose, const std::string& nameDaChoose, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) {
    return ScreenRegistration::confirmationChoose(typeDeChoose, nameDaChoose, informationForDisplay, artAsciiForDisplay);
}

std::vector<std::string> MenuScreen::composeFrameDeAttributes(const Attributes& stats, const std::string& titleDry, const std::string& titleSkill, const std::string& nameHab, const std::string& descHab, const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) {
    return ScreenRegistration::frameAttributes(stats, titleDry, titleSkill, nameHab, descHab, titleSkill2, nameHab2, descHab2);
}

int MenuScreen::displayMainMenuOptions() {
    return ScreenRegistration::mainMenu();
}

void MenuScreen::displayTutorialDeParry(const std::string& infoBox) {
    ScreenTutorial::displayTutorialDeParry(infoBox);
}
