#pragma once

#include <string>
#include <vector>
#include "Domain/Characters/Character.h"
#include "UI/Screens/Menu/NameScreen.h"
#include "UI/Screens/Menu/RaceScreen.h"
#include "UI/Screens/Menu/ClassScreen.h"
#include "UI/Screens/Menu/DifficultyScreen.h"
#include "UI/Screens/Menu/ParryScreen.h"

// [PT-BR] Interface abstrata (DIP) para que RegistroTelas nao dependa de implementacoes concretas (Raycaster ou IDE)
// [EN-US] Abstract interface (DIP) so ScreenRegistry does not depend on concrete implementations (Raycaster or IDE)
class IManagerScreens {
public:
    virtual ~IManagerScreens() = default;

    virtual void opening() = 0;
    virtual void panelLogo(const std::string& screenTitle, bool animateFadeIn) = 0;
    virtual bool chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) = 0;
    virtual std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& dryTitle, const std::string& skillTitle,
        const std::string& skillName, const std::string& skillDesc,
        const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) = 0;
    virtual int mainMenu() = 0;
    virtual void tutorialParry(const std::string& infoBox) = 0;
    virtual NameScreen::Result screenName() = 0;
    virtual RaceScreen::Result screenRace(const std::string& characterName) = 0;
    virtual ClassScreen::Result screenClass(const std::string& characterName, const std::string& race) = 0;
    virtual DifficultyScreen::Result screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) = 0;
    virtual ParryScreen::Result screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) = 0;
    virtual void screenIntroduction() = 0;
    virtual bool confirmExit() = 0;
};
