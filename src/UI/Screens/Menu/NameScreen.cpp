#include "UI/Screens/Menu/NameScreen.h"
#include "UI/Screens/ScreenRegistry.h"

NameScreen::Result NameScreen::display() {
    return ScreenRegistration::screenName();
}
