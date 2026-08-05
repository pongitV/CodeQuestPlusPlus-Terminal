#include "UI/Screens/Menu/RaceScreen.h"
#include "UI/Screens/ScreenRegistry.h"

RaceScreen::Result RaceScreen::display(const std::string& namePlayer) {
    return ScreenRegistration::screenRace(namePlayer);
}
