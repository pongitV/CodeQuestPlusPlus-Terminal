#pragma once

#include "UI/Screens/IScreenManager.h"

class RaycasterScreenManager : public IManagerScreens {
public:
    void opening() override;
    void panelLogo(const std::string& screenTitle, bool animateFadeIn) override;
    bool chooseConfirmation(const std::string& chooseType, const std::string& chooseName,
        const std::vector<std::string>& informationForDisplay, const std::vector<std::string>& asciiArtForDisplay) override;
    std::vector<std::string> frameAttributes(const Attributes& stats,
        const std::string& dryTitle, const std::string& skillTitle,
        const std::string& skillName, const std::string& skillDesc,
        const std::string& skillTitle2, const std::string& skillName2, const std::string& skillDesc2) override;
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
