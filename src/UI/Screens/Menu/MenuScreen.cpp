#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/Screens/Menu/TutorialScreen.h"
#include "UI/Screens/ScreenRegistry.h"

void MenuScreen::displayGameLogoPanel(const std::string& screenTitle, bool animateFadeIn) {
    ScreenRegistry::panelLogo(screenTitle, animateFadeIn);
}

bool MenuScreen::displayChooseConfirmationWithArtSideBySide(const std::string& chooseType, const std::string& chooseName, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) {
    return ScreenRegistry::chooseConfirmation(chooseType, chooseName, informationForDisplay, asciiArtForDisplay);
}

std::vector<std::string> MenuScreen::composeAttributesFrame(const Attributes& stats, const std::string& dryTitle, const std::string& skillTitle, const std::string& skillName, const std::string& skillDesc, const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) {
    return ScreenRegistry::frameAttributes(stats, dryTitle, skillTitle, skillName, skillDesc, skillTitle2, skillName2, skillDesc2);
}

int MenuScreen::displayMainMenuOptions() {
    return ScreenRegistry::mainMenu();
}

void MenuScreen::displayParryTutorial(const std::string& infoBox) {
    TutorialScreen::displayParryTutorial(infoBox);
}
