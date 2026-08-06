#include "UI/Renderers/3D/RaycasterScreens/Victory/RaycasterVictoryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "UI/Screens/Victory/VictoryScreenLayout.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <map>

void ScreenVictoryRaycaster::display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained,
    int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat,
    const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage,
    int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique,
    bool canRiseLevel, const std::vector<std::string>& newDiscoveries,
    const std::string& titleMap)
{
    int widthConsole = Appearance::getTerminalWidth();
    std::cout << "\033[?25l";
    InputControl::clearBuffer();
    // Agrupar inimigos para exibicao
    std::map<std::string, int> enemiesGrouped;
    for (const auto& ini : enemiesDefeated) {
        enemiesGrouped[ini]++;
    }

    /*
     * Usa o mesmo fundo gerado e cacheado durante a batalha,
     * garantindo que a tela pareca estatica no momento em que o combate acabou
     */
    std::vector<std::string> flatScreen = RaycasterRendererCombat::getLastBackgroundRendered();
    int height3D = std::max(10, Appearance::getTerminalHeight());
    
    // Fallback de seguranca se o cache falhar ou se o terminal mudar de tamanho do nada
    if (flatScreen.empty() || static_cast<int>(flatScreen.size()) < height3D * widthConsole) {
        std::vector<std::string> arena = RaycasterRendererCombat::getArenaByTitle(titleMap);
        float jX = static_cast<float>(arena[0].size()) / 2.0f;
        float jY = static_cast<float>(arena.size()) - 2.0f;
        flatScreen = Raycaster::drawFrameStatic3D(arena, jX, jY, -1.57f, titleMap, currentPlayer, height3D);
    }

    std::vector<std::string> screenBackground(height3D);
    for (int y = 0; y < height3D; ++y) {
        std::string line = "";
        for (int x = 0; x < widthConsole; ++x) {
            line += flatScreen[y * widthConsole + x];
        }
        line += "\033[0m"; // Garante reset
        screenBackground[y] = line;
    }

    auto stringForCharsUtf8 = [](const std::string& str) {
        std::vector<std::string> chars;
        size_t i = 0;
        while (i < str.length()) {
            int charLen = 1;
            unsigned char c = static_cast<unsigned char>(str[i]);
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3;
            else if ((c & 0xF8) == 0xF0) charLen = 4;
            chars.push_back(str.substr(i, charLen));
            i += charLen;
        }
        return chars;
    };

    // Desenha o logo do Vitoria
    int soonY = 2;
    int compVisualSoon = 0;
    for (const auto& line : ArtsVictory::soonVictory) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    int soonX = ScreenBaseMenu::calculateOffsetCentral(compVisualSoon, widthConsole);
    for (int i = 0; i < (int)ArtsVictory::soonVictory.size(); ++i) {
        if (soonY + i < height3D) {
            screenBackground[soonY + i] = Appearance::superimposeSoonAnsi(screenBackground[soonY + i], stringForCharsUtf8(ArtsVictory::soonVictory[i]), soonX, "\033[1;38;2;50;255;50m", widthConsole);
        }
    }

    int startY = soonY + (int)ArtsVictory::soonVictory.size() + 2;

    MenuRaycasterUtils::s_background3DMenu = screenBackground;

    // Funcao auxiliar para desenhar uma caixa cinza diretamente na stream de string do buffer
    auto drawBoxGray = [&](std::ostringstream& buff, int y, int x, int width, int height) {
        std::string bg = "\033[48;2;25;25;25m"; // Cinza HUD
        std::string colorEdge = "\033[38;2;255;255;255m"; // Branco
        std::string reset = "\033[0m";
        
        std::string traces = "";
        for (int i = 0; i < width - 2; ++i) traces += "═";
        std::string emptyLine = "";
        for (int i = 0; i < width - 2; ++i) emptyLine += " ";
        
        MenuRaycasterUtils::superimposeTextAbsolute(buff, bg + colorEdge + "╔" + traces + "╗" + reset, y, x);
        for (int i = 1; i < height - 1; ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buff, bg + colorEdge + "║" + bg + emptyLine + colorEdge + "║" + reset, y + i, x);
        }
        MenuRaycasterUtils::superimposeTextAbsolute(buff, bg + colorEdge + "╚" + traces + "╝" + reset, y + height - 1, x);
    };

    while (true) {
        std::ostringstream buffer;
        buffer << "\033[?25l\033[H"; // Cursor home LOGO NO INICIO para nao rolar o terminal

        for (size_t y = 0; y < MenuRaycasterUtils::s_background3DMenu.size(); ++y) {
            buffer << MenuRaycasterUtils::s_background3DMenu[y];
            if (y < MenuRaycasterUtils::s_background3DMenu.size() - 1) buffer << "\n";
        }

        // Caixa Inimigos
        std::vector<std::string> boxEnemies;
        boxEnemies.push_back("\033[38;2;200;200;200mInimigos Derrotados:\033[0m");
        if (enemiesGrouped.empty()) boxEnemies.push_back(" \033[38;2;100;100;100mNenhum\033[0m");
        for (auto const& [name, qty] : enemiesGrouped) {
            boxEnemies.push_back(" \033[38;2;255;100;100m" + std::to_string(qty) + "x " + name + "\033[0m");
        }

        // Caixa Recompensas separada
        std::vector<std::string> boxDrops;
        boxDrops.push_back("\033[38;2;255;215;0mRecompensas:\033[0m");
        boxDrops.push_back(" \033[38;2;255;215;0mOuro: " + std::to_string(quantityDeGoldObtained) + "\033[0m" + "  \033[38;2;0;255;255mXP: " + std::to_string(quantityDeXpObtained) + "\033[0m");
        if (!dropsUnique.empty()) boxDrops.push_back("\033[38;2;200;200;200mItens:\033[0m");
        for (auto const& drop : dropsUnique) {
            std::string nameLower = drop.first;
            std::string colorRarity = "\033[38;2;150;150;150m"; // Cinza (Comum)
            if (nameLower.find("Pocao") != std::string::npos || nameLower.find("Madeira") != std::string::npos || nameLower.find("Talisma") != std::string::npos) colorRarity = "\033[38;2;50;255;50m"; // Verde
            if (nameLower.find("Elixir") != std::string::npos || nameLower.find("Gosma") != std::string::npos || nameLower.find("Talisma") != std::string::npos) colorRarity = "\033[38;2;50;150;255m"; // Azul
            if (nameLower.find("Magica") != std::string::npos || nameLower.find("Encantado") != std::string::npos) colorRarity = "\033[38;2;200;50;255m"; // Roxo
            if (nameLower.find("Exterminio") != std::string::npos || nameLower.find("Cristal") != std::string::npos || nameLower.find("Cavaleiro") != std::string::npos) colorRarity = "\033[38;2;255;215;0m"; // Ouro

            boxDrops.push_back(" " + colorRarity + std::to_string(drop.second) + "x " + drop.first + "\033[0m");
        }

        // Caixa Estatisticas
        std::vector<std::string> boxStats;
        boxStats.push_back("\033[38;2;150;150;255mEstatisticas de Combate:\033[0m");
        boxStats.push_back(" Turnos: \033[38;2;255;255;255m" + std::to_string(shiftsCombat) + "\033[0m");
        boxStats.push_back(" Dano Causado: \033[38;2;255;100;100m" + std::to_string(totalDeDamageCaused) + "\033[0m");
        boxStats.push_back(" Dano Recebido: \033[38;2;255;50;50m" + std::to_string(totalDeDamageReceived) + "\033[0m");
        boxStats.push_back(" Cura Recebida: \033[38;2;50;255;50m" + std::to_string(cureTotalReceived) + "\033[0m");
        boxStats.push_back(" Maior Hit: \033[38;2;255;150;0m" + std::to_string(biggerDamage) + "\033[0m");
        boxStats.push_back(" Parrys Perfeitos: \033[38;2;0;255;255m" + std::to_string(parriesPerfect) + "\033[0m");

        int boxHeight = std::max((int)boxEnemies.size(), (int)boxStats.size());
        
        int totalW = 60;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(totalW, widthConsole);
        drawBoxGray(buffer, startY, boxX, totalW, boxHeight + 4);

        int col1X = boxX + 2;
        int col2X = boxX + 32;

        for(int i = 0; i < (int)boxEnemies.size(); ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m" + boxEnemies[i], startY + 2 + i, col1X);
        }
        for(int i = 0; i < (int)boxStats.size(); ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m" + boxStats[i], startY + 2 + i, col2X);
        }
        
        int boxDropsY = startY + boxHeight + 5;
        int boxDropsW = 50;
        int boxDropsX = ScreenBaseMenu::calculateOffsetCentral(boxDropsW, widthConsole);
        drawBoxGray(buffer, boxDropsY, boxDropsX, boxDropsW, boxDrops.size() + 2);
        
        for(int i = 0; i < (int)boxDrops.size(); ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m" + boxDrops[i], boxDropsY + 1 + i, boxDropsX + 2);
        }

        MenuRaycasterUtils::flushFrameForConsole(buffer.str());

        if (InputControl::pressedKey()) {
            unsigned char key = static_cast<unsigned char>(InputControl::readKey());
            if (key == '\r' || key == '\n' || key == ' ') {
                break;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}
