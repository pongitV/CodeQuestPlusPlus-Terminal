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

void ScreenInventoryRaycaster::displayHeader(bool, int startY) {
    int widthConsole = Appearance::getTerminalWidth();
    
    // Desenha o logo do INVENTARIO
    int soonHeight = ArtsInventory::soonInventory.size();
    int soonY = startY > 0 ? (startY - 1 - soonHeight) : 2;
    if (soonY < 0) soonY = 0;
    
    int compVisualSoon = 0;
    for (const auto& line : ArtsInventory::soonInventory) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    int soonX = (widthConsole - compVisualSoon) / 2;
    if (soonX < 0) soonX = 0;
    
    // Apenas desenha o logo sobre a tela atual, pulando espacos para nao pintar fundo preto
    std::string colorTitle = Appearance::color(Color::YELLOW);
    for (int i = 0; i < (int)ArtsInventory::soonInventory.size(); ++i) {
        Appearance::moveCursor(soonX, soonY + i);
        const std::string& line = ArtsInventory::soonInventory[i];
        
        std::string buffer = colorTitle;
        int spaceCount = 0;
        
        for (size_t j = 0; j < line.length(); ) {
            if (line[j] == ' ') {
                spaceCount++;
                j++;
            } else {
                if (spaceCount > 0) {
                    buffer += "\033[" + std::to_string(spaceCount) + "C";
                    spaceCount = 0;
                }
                
                unsigned char uc = line[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                
                buffer += line.substr(j, charLen);
                j += charLen;
            }
        }
        buffer += "\033[0m";
        std::cout << buffer;
    }
    std::cout << std::flush;
}

void ScreenInventoryRaycaster::displayBoxEquipped(Character*) {}
void ScreenInventoryRaycaster::displayDetailItem(Item*) {}

void ScreenInventoryRaycaster::renderMenu(const std::vector<std::string>& lines, const std::string& title, int selectionCurrent, int& outW, int& outH) {
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
    
    /*
     * Adicionar linhas vazias extras se necessario para manter o tamanho da caixa consistente?
     * E melhor se o InventarioCombate passar as linhas exatas.
     */
    std::vector<std::string> boxEnd = BaseScreen::createBox(linesBase, title, 0, Color::YELLOW, "\033[48;2;25;25;25m");
    
    if (outH > 0 && outW > 0) {
        Raycaster::restoreLastFrame();
    }
    
    outW = Appearance::getVisualLength(boxEnd[0]);
    outH = boxEnd.size();
    
    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    int startX = (terminalWidth - outW) / 2;
    int startY = (terminalHeight - outH) / 2;
    
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    
    std::cout << "\033[?25l";
    for (size_t i = 0; i < boxEnd.size(); ++i) {
        Appearance::moveCursor(startX, startY + i);
        std::cout << boxEnd[i];
    }
    std::cout << std::flush;
}
