#include "UI/Screens/Menu/DifficultyScreen.h"
#include "UI/Screens/ScreenRegistry.h"

DifficultyScreen::Result DifficultyScreen::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    return ScreenRegistration::screenDifficulty(namePlayer, raceName, className);
}
