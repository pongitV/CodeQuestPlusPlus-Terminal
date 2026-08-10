#pragma once

#include <string>
#include <vector>
#include "Domain/Characters/Character.h"
#include "UI/Screens/Menu/NameScreen.h"
#include "UI/Screens/Menu/RaceScreen.h"
#include "UI/Screens/Menu/ClassScreen.h"
#include "UI/Screens/Menu/DifficultyScreen.h"
#include "UI/Screens/Menu/ParryScreen.h"

// Interface abstrata (DIP) para que RegistroTelas nao dependa de implementacoes concretas (Raycaster ou IDE)
class IManagerScreens {
public:
    virtual ~IManagerScreens() = default;

    virtual void opening() = 0;
    virtual void panelSoon(const std::string& titleDaScreen, bool animateFadeIn) = 0;
    virtual bool confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) = 0;
    virtual std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& titleDry, const std::string& titleSkill,
        const std::string& nameHab, const std::string& descHab,
        const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) = 0;
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
