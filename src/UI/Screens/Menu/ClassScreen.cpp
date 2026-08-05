#include "UI/Screens/Menu/ClassScreen.h"
#include "UI/Screens/ScreenRegistry.h"

ClassScreen::Result ClassScreen::display(const std::string& namePlayer, const std::string& raceName) {
    return ScreenRegistration::screenClass(namePlayer, raceName);
}
