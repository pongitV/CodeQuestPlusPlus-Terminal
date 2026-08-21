#include "UI/Renderers/3D/RaycasterScreens/Defeat/RaycasterDefeatScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterUtils.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include "UI/Screens/Defeat/DefeatScreenLayout.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"
#include <fstream>
#include <thread>
#include <chrono>

void RaycasterDefeatScreen::display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity,
    int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns)
{
    (void)obtainedGoldQuantity;
    (void)obtainedXpQuantity;
    int widthConsole = Appearance::getTerminalWidth();
    std::cout << "\033[?25l";
    InputControl::clearBuffer();

    // [PT-BR] Reutiliza o fundo 3D gerado e cacheado durante a batalha
    // [EN-US] Reuses 3D background generated and cached during battle
    std::vector<std::string> flatScreen = RaycasterRendererCombat::getLastBackgroundRendered();
    int height3D = std::max(10, Appearance::getTerminalHeight());
    
    // Fallback
    if (flatScreen.empty() || static_cast<int>(flatScreen.size()) < height3D * widthConsole) {
        std::string titleMap = "Cemiterio";
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

    // [PT-BR] Renderiza o logo de DERROTA
    // [EN-US] Renders DEFEAT logo
    int soonY = 2;
    int compVisualSoon = 0;
    for (const auto& line : ArtsDefeat::defeatLogo) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    int soonX = ScreenBaseMenu::calculateOffsetCentral(compVisualSoon, widthConsole);
    for (int i = 0; i < (int)ArtsDefeat::defeatLogo.size(); ++i) {
        if (soonY + i < height3D) {
            screenBackground[soonY + i] = Appearance::superimposeSoonAnsi(screenBackground[soonY + i], Appearance::splitUtf8(ArtsDefeat::defeatLogo[i]), soonX, "\033[1;38;2;255;50;50m", widthConsole);
        }
    }

    int startY = soonY + (int)ArtsDefeat::defeatLogo.size() + 2;

    MenuRaycasterUtils::s_background3DMenu = screenBackground;

    // [PT-BR] Funcao auxiliar para desenhar caixa de interface
    // [EN-US] Helper function to draw UI box
    auto drawBoxGray = [&](std::ostringstream& buff, int y, int x, int width, int height) {
        std::string bg = "\033[48;2;25;25;25m"; 
        std::string colorEdge = "\033[38;2;255;255;255m"; 
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

    int indexSelected = 0;
    bool popupOpen = false;
    // [PT-BR] Flag indicando se o jogador optou por sair do jogo
    // [EN-US] Flag indicating if player chose to exit game
    bool leavingGame = false;
    
    while (true) {
        std::ostringstream buffer;
        buffer << "\033[?25l\033[H"; 

        for (size_t y = 0; y < MenuRaycasterUtils::s_background3DMenu.size(); ++y) {
            buffer << MenuRaycasterUtils::s_background3DMenu[y];
            if (y < MenuRaycasterUtils::s_background3DMenu.size() - 1) buffer << "\n";
        }

        // [PT-BR] Caixa de estatisticas da batalha
    // [EN-US] Battle statistics box
        std::vector<std::string> boxStats;
        boxStats.push_back("\033[38;2;255;100;100mEstatisticas:\033[0m");
        boxStats.push_back(" Turnos Sobrevividos: \033[38;2;255;255;255m" + std::to_string(combatTurns) + "\033[0m");
        boxStats.push_back(" Dano Causado: \033[38;2;255;100;100m" + std::to_string(totalDamageCaused) + "\033[0m");
        boxStats.push_back(" Dano Recebido: \033[38;2;255;50;50m" + std::to_string(totalDamageReceived) + "\033[0m");
        boxStats.push_back(" Cura Recebida: \033[38;2;100;255;100m" + std::to_string(totalHealingReceived) + "\033[0m");

        int boxHeight = boxStats.size() + 2;
        int boxW = 40;
        int boxX = ScreenBaseMenu::calculateOffsetCentral(boxW, widthConsole);
        
        drawBoxGray(buffer, startY, boxX, boxW, boxHeight);
        
        for(int i = 0; i < (int)boxStats.size(); ++i) {
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m" + boxStats[i], startY + 1 + i, boxX + 2);
        }

        // [PT-BR] Popup interativo de selecao
    // [EN-US] Interactive selection popup
        int boxPopupW = 50;
        int boxPopupX = ScreenBaseMenu::calculateOffsetCentral(boxPopupW, widthConsole);
        int boxPopupY = startY + boxHeight + 2;

        if (popupOpen) {
            // Confirmacao
            drawBoxGray(buffer, boxPopupY, boxPopupX, boxPopupW, 5);
            std::string msgConfirm = leavingGame ? "Tem certeza que deseja sair?" : "Voltar ao menu principal?";
            int textX = boxPopupX + (boxPopupW - Appearance::getVisualLength(msgConfirm)) / 2;
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m\033[38;2;255;100;100m" + msgConfirm, boxPopupY + 1, textX);
            
            std::string msgS = " [S] Sim ";
            std::string msgN = " [N] Nao ";
            int btnY = boxPopupY + 3;
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m\033[38;2;200;200;200m" + msgS, btnY, boxPopupX + 8);
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, "\033[48;2;25;25;25m\033[38;2;200;200;200m" + msgN, btnY, boxPopupX + boxPopupW - 8 - 9);

        } else {
            // Escolha
            drawBoxGray(buffer, boxPopupY, boxPopupX, boxPopupW, 6);
            std::string opt0 = "Voltar ao menu principal";
            std::string opt1 = "Sair do jogo";

            std::string format0 = indexSelected == 0 ? "\033[48;2;200;200;200m\033[38;2;0;0;0m> " + opt0 + " <\033[0m" : "\033[48;2;25;25;25m\033[38;2;200;200;200m  " + opt0 + "  \033[0m";
            std::string format1 = indexSelected == 1 ? "\033[48;2;200;200;200m\033[38;2;0;0;0m> " + opt1 + " <\033[0m" : "\033[48;2;25;25;25m\033[38;2;200;200;200m  " + opt1 + "  \033[0m";

            int opt0X = boxPopupX + (boxPopupW - (opt0.length() + 4)) / 2;
            int opt1X = boxPopupX + (boxPopupW - (opt1.length() + 4)) / 2;

            MenuRaycasterUtils::superimposeTextAbsolute(buffer, format0, boxPopupY + 2, opt0X);
            MenuRaycasterUtils::superimposeTextAbsolute(buffer, format1, boxPopupY + 4, opt1X);
        }

        MenuRaycasterUtils::flushFrameForConsole(buffer.str());

        if (InputControl::pressedKey()) {
            unsigned char key = static_cast<unsigned char>(InputControl::readKey());
            if (popupOpen) {
                if (key == 's' || key == 'S') {
                    if (leavingGame) {
                        exit(0);
                    } else {
                        return; 
                    }
                } else if (key == 'n' || key == 'N') {
                    popupOpen = false;
                }
            } else {
                if (key == 72 || key == 'w' || key == 'W') { 
                    indexSelected--;
                    if (indexSelected < 0) indexSelected = 1;
                } else if (key == 80 || key == 's' || key == 'S') { 
                    indexSelected++;
                    if (indexSelected > 1) indexSelected = 0;
                } else if (key == '\r' || key == '\n' || key == ' ') {
                    leavingGame = (indexSelected == 1);
                    popupOpen = true;
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}
