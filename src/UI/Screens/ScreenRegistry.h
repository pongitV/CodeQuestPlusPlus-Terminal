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

    // [PT-BR] --- Tela de Abertura ---
    // [EN-US] --- Opening Screen ---
    static void opening();

    // [PT-BR] --- Tela de Menu e Selecoes ---
    // [EN-US] --- Menu and Selection Screens ---
    static void panelLogo(const std::string& screenTitle = "", bool animateFadeIn = false);
    static bool chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay);
    static std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& dryTitle, const std::string& skillTitle,
        const std::string& skillName, const std::string& skillDesc,
        const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2);
    static int mainMenu();
    static void tutorialParry(const std::string& infoBox = "");

    // [PT-BR] --- Tela de Nome ---
    // [EN-US] --- Name Screen ---
    static NameScreen::Result screenName();

    // [PT-BR] --- Tela de Raca ---
    // [EN-US] --- Race Screen ---
    static RaceScreen::Result screenRace(const std::string& characterName);

    // [PT-BR] --- Tela de Classe ---
    // [EN-US] --- Class Screen ---
    static ClassScreen::Result screenClass(const std::string& characterName, const std::string& race);

    // [PT-BR] --- Tela de Dificuldade ---
    // [EN-US] --- Difficulty Screen ---
    static DifficultyScreen::Result screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj);

    // [PT-BR] --- Tela de Parry ---
    // [EN-US] --- Parry Screen ---
    static ParryScreen::Result screenParry(const std::string& characterName, const std::string& race, const std::string& classObj);

    // [PT-BR] --- Tela de Introducao ---
    // [EN-US] --- Introduction Screen ---
    static void screenIntroduction();
    
    // [PT-BR] --- Sair do Jogo ---
    // [EN-US] --- Exit Game ---
    static bool confirmExit();
};
