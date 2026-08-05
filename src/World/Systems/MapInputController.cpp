#include "World/Systems/MapInputController.h"
#include "Core/Utils/InputDispatcher.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Pause/PauseScreen.h"
#include "Core/Engine/Debug.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"

bool MapInputController::processInputAndCommands(char key, Character* player, int& nextPositionX, int& nextPositionY, const std::function<void()>& restoreScreen)
{
    static bool initialized = false;
    static InputDispatcher dispatcher;
    if (!initialized) {
        initialized = true;
        dispatcher.registerAction(27, [=]() { PauseScreen::display(player); restoreScreen(); });
        dispatcher.registerAction(static_cast<int>('\\'), [=]() { Debug::displayDebugMenu(player); restoreScreen(); });
        dispatcher.registerAction(static_cast<int>('`'),  [=]() { Debug::displayDebugMenu(player); restoreScreen(); });
        dispatcher.registerAction(static_cast<int>('='),  [=]() { Debug::displayDebugMenu(player); restoreScreen(); });
    }

    if (dispatcher.execute(static_cast<int>(key))) return true;

    MapCommand command = InputControl::translateKeyToCommand(key);

    if (command == MapCommand::Up) { nextPositionY--; return false; }
    if (command == MapCommand::Down) { nextPositionY++; return false; }
    if (command == MapCommand::Left) { nextPositionX--; return false; }
    if (command == MapCommand::Right) { nextPositionX++; return false; }

    if (command == MapCommand::Inventory)
    {
        CombatInventory::manageInventory(player);
        restoreScreen();
        return true;
    }
    if (command == MapCommand::CharacterSheet)
    {
        AttributesScreen::managePlayerCharacterSheet(player);
        restoreScreen();
        return true;
    }
    if (command == MapCommand::Bestiary)
    {
        DiaryScreen::display(player);
        restoreScreen();
        return true;
    }
    return false;
}
