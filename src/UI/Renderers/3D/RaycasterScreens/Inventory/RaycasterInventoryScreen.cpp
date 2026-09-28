#include "UI/Renderers/3D/RaycasterScreens/Inventory/RaycasterInventoryScreen.h"
#include <iostream>
#include <vector>
#include "Core/Terminal/Appearance/Appearance.h"
#include "Core/Terminal/Appearance/Color.h"
#include "Domain/Items/Item.h"
#include "Domain/Characters/Character.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Inventory/InventoryScreenLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"

void RaycasterInventoryScreen::displayHeader(bool, int startY) {
    ScreenBaseMenu::displayFloatingLogo(ArtsInventory::inventoryLogo, startY, Color::YELLOW, "[ === INVENTARIO === ]");
}

void RaycasterInventoryScreen::displayBoxEquipped(Character*) {}
void RaycasterInventoryScreen::displayDetailItem(Item*) {}

void RaycasterInventoryScreen::renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) {
    std::vector<std::string> linesBase = lines;
    int interactiveIdx = 0;
    
    std::string strPocket = "BOLSO:"; // Auxiliar para identificar linhas nao iterativas caso necessario
    
    for (size_t i = 0; i < linesBase.size(); ++i) {
        if (linesBase[i].empty() || linesBase[i].find(strPocket) != std::string::npos || linesBase[i].find("   ") == 0) {
            // Already formatted or empty space
        } else {
            if (interactiveIdx == selectionCurrent) {
                linesBase[i] = Appearance::color(Color::GREEN) + " > " + linesBase[i] + Appearance::color(Color::WHITE) + "\033[48;2;25;25;25m";
            } else {
                linesBase[i] = "   " + linesBase[i];
            }
            interactiveIdx++;
        }
    }
    
    std::vector<std::string> boxEnd = BaseScreen::createBox(linesBase, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
    
    if (outH > 0 && outW > 0) {
        Raycaster::restoreLastFrame();
    }
    
    outW = Appearance::getVisualLength(boxEnd[0]);
    outH = (int)boxEnd.size();
    
    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    int startX = std::max(0, (terminalWidth - outW) / 2);
    int startY = std::max(0, (terminalHeight - outH) / 2);
    
    if (startY + outH > terminalHeight) startY = std::max(0, terminalHeight - outH);
    if (startX + outW > terminalWidth) startX = std::max(0, terminalWidth - outW);
    
    std::cout << "\033[?25l";
    for (size_t i = 0; i < boxEnd.size(); ++i) {
        if (startY + (int)i < terminalHeight) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << boxEnd[i];
        }
    }
    std::cout << std::flush;
}
