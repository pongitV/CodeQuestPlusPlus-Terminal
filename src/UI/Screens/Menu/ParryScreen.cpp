#include "UI/Screens/Menu/ParryScreen.h"
#include "UI/Screens/ScreenRegistry.h"

ParryScreen::Result ParryScreen::display(const std::string& namePlayer, const std::string& raceName, const std::string& className) {
    return ScreenRegistry::screenParry(namePlayer, raceName, className);
}
