#pragma once

#include "UI/Screens/IScreenManager.h"

class ManagerScreensRaycaster : public IManagerScreens {
public:
    void opening() override;
    void panelSoon(const std::string& titleDaScreen, bool animateFadeIn) override;
    bool confirmationChoose(const std::string& typeDeChoose, const std::string& nameDaChoose,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& artAsciiForDisplay) override;
    std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& titleDry, const std::string& titleSkill,
        const std::string& nameHab, const std::string& descHab,
        const std::string& titleSkill2, const std::string& nameHab2, const std::string& descHab2) override;
    int mainMenu() override;
    void tutorialParry(const std::string& infoBox) override;
    NameScreen::Result screenName() override;
    RaceScreen::Result screenRace(const std::string& characterName) override;
    ClassScreen::Result screenClass(const std::string& characterName, const std::string& race) override;
    DifficultyScreen::Result screenDifficulty(const std::string& characterName, const std::string& race, const std::string& classObj) override;
    ParryScreen::Result screenParry(const std::string& characterName, const std::string& race, const std::string& classObj) override;
    void screenIntroduction() override;
    bool confirmExit() override;
};
