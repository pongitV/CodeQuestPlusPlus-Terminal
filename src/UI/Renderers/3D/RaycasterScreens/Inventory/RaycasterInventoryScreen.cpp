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

void RaycasterInventoryScreen::displayHeader(bool, int startY) {
    int widthConsole = Appearance::getTerminalWidth();
    int soonHeight = (int)ArtsInventory::inventoryLogo.size();
    
    int compVisualSoon = 0;
    for (const auto& line : ArtsInventory::inventoryLogo) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    
    int soonY = startY > 0 ? (startY - 1 - soonHeight) : 1;
    if (soonY < 0) soonY = 0;
    int soonX = (widthConsole - compVisualSoon) / 2;
    if (soonX < 0) soonX = 0;
    
    // [PT-BR] Se a tela possui largura suficiente para o logo ASCII completo
    // [EN-US] If screen has sufficient width for full ASCII logo
    if (widthConsole >= compVisualSoon && (startY >= soonHeight + 1 || soonY == 0)) {
        std::string bgDark = "\033[48;2;20;20;20m";
        std::string colorTitle = Appearance::color(Color::YELLOW);
        std::string reset = "\033[0m";
        
        for (int i = 0; i < soonHeight; ++i) {
            Appearance::moveCursor(soonX, soonY + i);
            const std::string& line = ArtsInventory::inventoryLogo[i];
            
            std::string buffer = bgDark + colorTitle;
            for (size_t j = 0; j < line.length(); ) {
                unsigned char uc = line[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                
                buffer += line.substr(j, charLen);
                j += charLen;
            }
            buffer += reset;
            std::cout << buffer;
        }
        std::cout << std::flush;
    } else if (startY >= 2) {
        // [PT-BR] Fallback simplificado para terminais compactos
    // [EN-US] Simplified fallback for compact terminals
        std::string titleCompact = "[ === INVENTARIO === ]";
        int compCompact = Appearance::getVisualLength(titleCompact);
        int cx = std::max(0, (widthConsole - compCompact) / 2);
        int cy = std::max(0, startY - 1);
        Appearance::moveCursor(cx, cy);
        std::cout << Appearance::color(Color::YELLOW) << "\033[48;2;25;25;25m" << titleCompact << "\033[0m" << std::flush;
    }
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
