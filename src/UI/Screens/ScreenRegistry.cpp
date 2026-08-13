#include "UI/Screens/ScreenRegistry.h"
#include "UI/PerspectiveManager.h"
#include "UI/Screens/Menu/TutorialScreen.h"
#include "UI/PerspectiveChangedException.h"


bool ScreenRegistry::raycasterModeActive() {
    return PerspectiveManager::getInstance().is3DViewActive();
}

void ScreenRegistry::opening() {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->opening();
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistry::panelLogo(const std::string& screenTitle, bool animateFadeIn) {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->panelLogo(screenTitle, animateFadeIn);
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

bool ScreenRegistry::chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->chooseConfirmation(chooseType, chooseName, informationForDisplay, asciiArtForDisplay);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

std::vector<std::string> ScreenRegistry::frameAttributes(const Attributes& stats,
    const std::string& dryTitle, const std::string& skillTitle,
    const std::string& skillName, const std::string& skillDesc,
    const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->frameAttributes(stats, dryTitle, skillTitle, skillName, skillDesc, skillTitle2, skillName2, skillDesc2);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

int ScreenRegistry::mainMenu() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->mainMenu();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistry::tutorialParry(const std::string& infoBox) {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->tutorialParry(infoBox);
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

NameScreen::Result ScreenRegistry::screenName() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenName();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

bool ScreenRegistry::confirmExit() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->confirmExit();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

RaceScreen::Result ScreenRegistry::screenRace(const std::string& characterName) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenRace(characterName);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

ClassScreen::Result ScreenRegistry::screenClass(const std::string& characterName, const std::string& race) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenClass(characterName, race);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

DifficultyScreen::Result ScreenRegistry::screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenDifficulty(characterName, race, classObj);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

ParryScreen::Result ScreenRegistry::screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenParry(characterName, race, classObj);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistry::screenIntroduction() {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->screenIntroduction();
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}
