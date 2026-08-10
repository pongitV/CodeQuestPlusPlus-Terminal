#pragma once

#include <string>
#include <vector>
#include <functional>
#include "Domain/Characters/Character.h"
#include "UI/Screens/Menu/NameScreen.h"
#include "UI/Screens/Menu/RaceScreen.h"
#include "UI/Screens/Menu/ClassScreen.h"
#include "UI/Screens/Menu/DifficultyScreen.h"
#include "UI/Screens/Menu/ParryScreen.h"

class ScreenRegistration {
public:
    static bool raycasterModeActive();

    // --- TelaAbertura ---
    static void opening();

    // --- TelaMenu ---
    static void panelSoon(const std::string& titleDaScreen = "", bool animateFadeIn = false);
    static bool confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay);
    static std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& titleDry, const std::string& titleSkill,
        const std::string& nameHab, const std::string& descHab,
        const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2);
    static int mainMenu();
    static void tutorialParry(const std::string& infoBox = "");

    // --- TelaNome ---
    static NameScreen::Result screenName();

    // --- TelaRaca ---
    static RaceScreen::Result screenRace(const std::string& characterName);

    // --- TelaClasse ---
    static ClassScreen::Result screenClass(const std::string& characterName, const std::string& race);

    // --- TelaDificuldade ---
    static DifficultyScreen::Result screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj);

    // --- TelaParry ---
    static ParryScreen::Result screenParry(const std::string& characterName, const std::string& race, const std::string& classObj);

    // --- TelaIntroducao ---
    static void screenIntroduction();
    
    // --- Sair do Jogo ---
    static bool confirmExit();
};
