#pragma once

#include <string>
#include <vector>

#include "Domain/Characters/Character.h"

class IDEMenuScreen {
public:
    static void displayGameLogoPanel(const std::string& screenTitle = "", bool animateFadeIn = false);
    static bool displayChooseConfirmationWithArtSideBySide(const std::string& chooseType, const std::string& chooseName, const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay);
    static std::vector<std::string> composeAttributesFrame(const Attributes& stats, const std::string& dryTitle, const std::string& skillTitle, const std::string& skillName, const std::string& skillDesc, const std::string& skillTitle2 = "", const std::string& skillName2 = "", const std::string& skillDesc2 = "");
    static int displayMainMenuOptions();
    static bool displayConfirmationExit();
    
    // Novas funcoes de utilidade
    static std::vector<std::string> compressArtASCII(const std::vector<std::string>& artOriginal, int factorY, int factorX);
    static void printCentralizedBlockIDE(const std::vector<std::string>& block, int recoilAdditionalX = 0);
    static int calculateSpaceY(int contentLines);
};
