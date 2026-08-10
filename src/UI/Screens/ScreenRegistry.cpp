#include "UI/Screens/ScreenRegistry.h"
#include "UI/PerspectiveManager.h"
#include "UI/Screens/Menu/TutorialScreen.h"
#include "UI/PerspectiveChangedException.h"


bool ScreenRegistration::raycasterModeActive() {
    return PerspectiveManager::getInstance().is3DViewActive();
}

void ScreenRegistration::opening() {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->opening();
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistration::panelSoon(const std::string& titleDaScreen, bool animateFadeIn) {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->panelSoon(titleDaScreen, animateFadeIn);
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

bool ScreenRegistration::confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
    const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->confirmationChoose(typeDeChoose, nameDaChoose, informationForDisplay, artAsciiForDisplay);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

std::vector<std::string> ScreenRegistration::frameAttributes(const Attributes& stats,
    const std::string& titleDry, const std::string& titleSkill,
    const std::string& nameHab, const std::string& descHab,
    const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->frameAttributes(stats, titleDry, titleSkill, nameHab, descHab, titleSkill2, nameHab2, descHab2);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

int ScreenRegistration::mainMenu() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->mainMenu();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistration::tutorialParry(const std::string& infoBox) {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->tutorialParry(infoBox);
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

NameScreen::Result ScreenRegistration::screenName() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenName();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

bool ScreenRegistration::confirmExit() {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->confirmExit();
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

RaceScreen::Result ScreenRegistration::screenRace(const std::string& characterName) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenRace(characterName);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

ClassScreen::Result ScreenRegistration::screenClass(const std::string& characterName, const std::string& race) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenClass(characterName, race);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

DifficultyScreen::Result ScreenRegistration::screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenDifficulty(characterName, race, classObj);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

ParryScreen::Result ScreenRegistration::screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) {
    while (true) {
        try {
            return PerspectiveManager::getInstance().getManagerScreens()->screenParry(characterName, race, classObj);
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}

void ScreenRegistration::screenIntroduction() {
    while (true) {
        try {
            PerspectiveManager::getInstance().getManagerScreens()->screenIntroduction();
            return;
        } catch (const PerspectiveChangedException&) {
            continue;
        }
    }
}
