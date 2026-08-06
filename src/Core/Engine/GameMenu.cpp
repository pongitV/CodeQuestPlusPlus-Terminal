/*
 * Arquivo: GameMenu.cpp
 * Proposito: Implementacao do fluxo de menus principais e criacao de personagem.
 */

#include "Core/Engine/GameMenu.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "Domain/Characters/Classes/ClassFactory.h"
#include "Domain/Characters/Races/RaceFactory.h"
#include "Core/Engine/EnemyCreator.h"
#include "Domain/Items/Equipment/WeaponEquipment.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Items/ConsumableItem.h"
#include "Domain/Characters/Races/Dwarf.h"
#include "Domain/Characters/Races/Elf.h"
#include "Domain/Characters/Races/Human.h"
#include "Domain/Characters/Races/Orc.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Enemies/Goblin.h"
#include "Domain/Enemies/Slime.h"
#include "Domain/Enemies/Troll.h"
#include "Domain/Enemies/Fairy.h"
#include "Domain/Enemies/Mimic.h"
#include "Systems/Progression/Diary.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Inventory/InventoryScreen.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/Screens/Menu/OpeningScreen.h"
#include "UI/Screens/Menu/NameScreen.h"
#include "UI/Screens/Menu/RaceScreen.h"
#include "UI/Screens/Menu/ClassScreen.h"
#include "UI/Screens/Menu/DifficultyScreen.h"
#include "UI/Screens/Menu/ParryScreen.h"
#include "UI/Screens/Menu/IntroductionScreen.h"
#include "UI/Screens/ScreenRegistry.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/RandomGenerator.h"
#include "UI/Screens/BaseScreen.h"

std::unique_ptr<Character> GameMenu::mainMenu() 
{
    OpeningScreen::display();

    while (true) {
        int selection = MenuScreen::displayMainMenuOptions();
        
        std::string selectedOption;
        if (selection == 0) selectedOption = "Novo Jogo";
        else selectedOption = "Sair";
        
        if (selectedOption == "Novo Jogo") {
            auto newPlayer = startCharacterSystemCreation();
            if (newPlayer) return newPlayer;
        } else {
            if (ScreenRegistration::raycasterModeActive()) {
                return nullptr;
            } else {
                if (ScreenRegistration::confirmExit()) {
                    return nullptr;
                }
            }
        }
    }
}

std::unique_ptr<Character> GameMenu::startCharacterSystemCreation() 
{
    std::string characterName;
    std::unique_ptr<BaseRace> chosenRace;
    std::unique_ptr<BaseClass> chosenClass;
    bool parrySystemEnabled = false;
    ParryScreen::Result::Mode parryMode = ParryScreen::Result::Mode::Off;
    int chosenDifficultyLevel = 2;

    for (;;) {
        {
            auto result = NameScreen::display();
            if (result.returned) return nullptr;
            characterName = result.name;
        }

        std::string raceName;
        {
            auto result = RaceScreen::display(characterName);
            if (result.returned) continue;

            raceName = result.name;
            chosenRace = RaceFactory::createRace(result.selectedRace);
        }

        std::string className;
        {
            auto result = ClassScreen::display(characterName, raceName);
            if (result.returned) continue;

            className = result.name;
            chosenClass = ClassFactory::createClass(result.selectedClass);
        }

        {
            auto result = DifficultyScreen::display(characterName, raceName, className);
            if (result.returned) continue;
            chosenDifficultyLevel = result.index + 1;
        }

        {
            auto result = ParryScreen::display(characterName, raceName, className);
            if (result.returned) continue;
            parryMode = result.mode;
            parrySystemEnabled = result.mode != ParryScreen::Result::Mode::Off;
        }

        auto createdCharacter = std::make_unique<Character>(characterName, std::move(chosenRace), std::move(chosenClass));
        createdCharacter->setParryEnabled(parrySystemEnabled);
        createdCharacter->setModernParry(parryMode == ParryScreen::Result::Mode::Movement);
        createdCharacter->setDifficulty(static_cast<GameDifficulty>(chosenDifficultyLevel));

        Diary::instance().registerRace(createdCharacter->getRace()->getRaceName());
        Diary::instance().registerClass(createdCharacter->getClassName());

        for (Item* item : createdCharacter->getInventory()->getAllItems()) {
            Diary::instance().registerItem(Appearance::removeANSIColors(item->getItemName()));
        }

        IntroductionScreen::display();
        return createdCharacter;
    }
}
