#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

class MenuScreen {
public:
    static void displayPanelSoonGame(const std::string& titleDaScreen = "", bool animateFadeIn = false);
    static bool displayConfirmationDeChooseWithArtSideASide(const std::string& typeDeChoose, const std::string& nameDaChoose, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay);
    static std::vector<std::string> composeFrameDeAttributes(const Attributes& stats, const std::string& titleDry, const std::string& titleSkill, const std::string& nameHab, const std::string& descHab, const std::string& titleSkill2 = "", const std::string& nameHab2 = "", const std::string& descHab2 = "");
    static int displayMainMenuOptions();
    static void displayTutorialDeParry(const std::string& infoBox = "");
};
