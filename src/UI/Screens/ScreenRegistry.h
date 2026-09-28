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

class ScreenRegistry {
public:
    static bool raycasterModeActive();

    // Tela de Abertura
    static void opening();

    // Tela de Menu e Selecoes
    static void panelLogo(const std::string& screenTitle = "", bool animateFadeIn = false);
    static bool chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay);
    static std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& sectionTitle, const std::string& skillTitle,
        const std::string& skillName, const std::string& skillDesc,
        const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2);
    static int mainMenu();
    static void tutorialParry(const std::string& infoBox = "");

    // Tela de Nome
    static NameScreen::Result screenName();

    // Tela de Raca
    static RaceScreen::Result screenRace(const std::string& characterName);

    // Tela de Classe
    static ClassScreen::Result screenClass(const std::string& characterName, const std::string& race);

    // Tela de Dificuldade
    static DifficultyScreen::Result screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj);

    // Tela de Parry
    static ParryScreen::Result screenParry(const std::string& characterName, const std::string& race, const std::string& classObj);

    // Tela de Introducao
    static void screenIntroduction();
    
    // Sair do Jogo
    static bool confirmExit();
};
