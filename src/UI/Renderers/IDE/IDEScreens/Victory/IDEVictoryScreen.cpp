#include "UI/Renderers/IDE/IDEScreens/Victory/IDEVictoryScreen.h"
#include <iostream>
#include <map>
#include <fstream>
#include <thread>
#include <chrono>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Combat/Combat.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Victory/VictoryScreenLayout.h"
#include "UI/Screens/Scene3DScreen.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"
#include "UI/PerspectiveManager.h"

class SceneVictoryIntro3D : public ScreenScene3D {
private:
    Character* currentPlayer;
    int obtainedGoldQuantity;
    int totalDamageCaused;
    int totalDamageReceived;
    int totalHealingReceived;
    int combatTurns;
    int biggerDamage;
    int itemsConsumed;
    int parriesTempted;
    int parriesEffective;
    std::vector<std::string> screen3D;
    std::string titleMap;

public:
    SceneVictoryIntro3D(Character* player, int gold, int damageCaused, int damageReceived, int healing, int shifts, int mDamage, int items, int pTry, int pEf, const std::string& map)
        : currentPlayer(player), obtainedGoldQuantity(gold), totalDamageCaused(damageCaused),
          totalDamageReceived(damageReceived), totalHealingReceived(healing), combatTurns(shifts), biggerDamage(mDamage), itemsConsumed(items), parriesTempted(pTry), parriesEffective(pEf), titleMap(map) {
        std::vector<Character*> empty;
        screen3D = RaycasterRendererCombat::renderFrame(titleMap, currentPlayer, empty);
    }

protected:
    std::vector<std::string> getLogo() const override {
        return ArtsVictory::victoryLogo;
    }

    std::string getLogoColor() const override {
        return "\033[1;32m";
    }

    std::vector<std::string> getBackgroundInitial() const override {
        return screen3D;
    }

    std::vector<std::string> composeBackgroundWithElements(float opacity) const override {
        std::vector<std::string> frameLines = screen3D;
        int terminalWidth = Appearance::getTerminalWidth();

        std::vector<std::string> isLines;
        isLines.push_back("");
        isLines.push_back(" Turnos         : " + std::to_string(combatTurns));
        isLines.push_back(" Dano Causado   : " + std::to_string(totalDamageCaused));
        isLines.push_back(" Maior Hit Dano : " + Appearance::color(Color::RED) + std::to_string(biggerDamage) + Appearance::color(Color::RESET));
        isLines.push_back(" Dano Recebido  : " + std::to_string(totalDamageReceived));
        isLines.push_back(" Cura Realizada : " + std::to_string(totalHealingReceived));
        isLines.push_back(" Itens Gastos   : " + std::to_string(itemsConsumed));

        std::string strParry = std::to_string(parriesEffective) + "/" + std::to_string(parriesTempted);
        if (parriesTempted > 0 && parriesEffective == parriesTempted) {
            strParry = Appearance::color(Color::YELLOW) + strParry + " (Perfeito!)" + Appearance::color(Color::RESET);
        }
        isLines.push_back(" Parries        : " + strParry);
        isLines.push_back("");
        std::vector<std::string> boxIs = BaseScreen::createBox(isLines, "ESTATISTICAS DE DESEMPENHO", 40, Color::CYAN);

        std::vector<std::string> progressLines;
        progressLines.push_back(" Evolucao do Personagem:");
        progressLines.push_back("");

        double xpPct = static_cast<double>(currentPlayer->getCurrentXp()) / std::max(1, currentPlayer->getXpForRise());
        std::string barXp = BaseScreen::generateBarGradient(xpPct, 12, Color::CYAN);
        progressLines.push_back(" XP: [" + barXp + Appearance::color(Color::RESET) + "] " + Appearance::color(Color::CYAN) + "+0" + Appearance::color(Color::RESET));

        int totalExcitedGold = currentPlayer->getInventory()->getGold() - obtainedGoldQuantity;
        progressLines.push_back(" Ouro Total: " + Appearance::color(Color::YELLOW) + std::to_string(totalExcitedGold) + "G " + Appearance::color(Color::RESET) + "(+0)");
        progressLines.push_back("");

        std::vector<std::string> boxProgress = BaseScreen::createBox(progressLines, "PROGRESSAO DE BATALHA", 40, Color::YELLOW);

        std::vector<std::string> dropsLines;
        dropsLines.push_back(Appearance::color(Color::GRAY) + " ???" + Appearance::color(Color::RESET));
        std::vector<std::string> boxDrops = BaseScreen::createBox(dropsLines, "SAQUE & DESCOBERTAS", 84, Color::MAGENTA);

        int isWidth = 40;
        int programWidth = 40;
        int gap = 4;
        int totalTopWidth = isWidth + gap + programWidth;

        int startIsX = (terminalWidth - totalTopWidth) / 2;
        if (startIsX < 0) startIsX = 0;
        int startProgramX = startIsX + isWidth + gap;
        int startDropsX = (terminalWidth - 84) / 2;
        if (startDropsX < 0) startDropsX = 0;

        std::vector<std::string> fadeIs, fadeProgram, fadeDrops;
        for (const auto& l : boxIs) fadeIs.push_back(Appearance::fatarLineAnsi(l, opacity));
        for (const auto& l : boxProgress) fadeProgram.push_back(Appearance::fatarLineAnsi(l, opacity));
        for (const auto& l : boxDrops) fadeDrops.push_back(Appearance::fatarLineAnsi(l, opacity));

        superimposePanel(frameLines, fadeIs, 16, startIsX);
        superimposePanel(frameLines, fadeProgram, 16, startProgramX);

        int yDrops = 16 + boxProgress.size() + 1;
        superimposePanel(frameLines, fadeDrops, yDrops, startDropsX);

        return frameLines;
    }
};

void IDEVictoryScreen::display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity,
    int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns,
    const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage,
    int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique,
    bool canRiseLevel, const std::vector<std::string>& newDiscoveries,
    const std::string& titleMap)
{
    bool isMode3D = PerspectiveManager::getInstance().is3DViewActive();

    if (!isMode3D) {
        Appearance::clearScreen();
    }

    int framesXP = 20;
    int framesTotal = framesXP + (dropsUnique.empty() ? 0 : static_cast<int>(dropsUnique.size())) + 1;

    if (isMode3D) {
        SceneVictoryIntro3D intro(currentPlayer, obtainedGoldQuantity, totalDamageCaused, totalDamageReceived, totalHealingReceived, combatTurns, biggerDamage, itemsConsumed, parriesTempted, parriesEffective, titleMap);
        intro.execute();
    }

    for (int frame = 0; frame <= framesTotal; ++frame) {
        int curGold = (obtainedGoldQuantity * std::min(frame, framesXP)) / framesXP;
        int curXp = (obtainedXpQuantity * std::min(frame, framesXP)) / framesXP;

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        std::vector<Character*> empty;
        std::vector<std::string> screen3D;

        if (isMode3D) {
            screen3D = RaycasterRendererCombat::renderFrame(titleMap, currentPlayer, empty);
        } else {
            Appearance::displayArtPanel(ArtsVictory::victoryLogo, 85, Color::GREEN, "", frame == 0);
        }

        std::vector<std::string> isLines;
        isLines.push_back("");
        isLines.push_back(" Turnos         : " + std::to_string(combatTurns));
        isLines.push_back(" Dano Causado   : " + std::to_string(totalDamageCaused));
        isLines.push_back(" Maior Hit Dano : " + Appearance::color(Color::RED) + std::to_string(biggerDamage) + Appearance::color(Color::RESET));
        isLines.push_back(" Dano Recebido  : " + std::to_string(totalDamageReceived));
        isLines.push_back(" Cura Realizada : " + std::to_string(totalHealingReceived));
        isLines.push_back(" Itens Gastos   : " + std::to_string(itemsConsumed));

        std::string strParry = std::to_string(parriesEffective) + "/" + std::to_string(parriesTempted);
        if (parriesTempted > 0 && parriesEffective == parriesTempted) {
            strParry = Appearance::color(Color::YELLOW) + strParry + " (Perfeito!)" + Appearance::color(Color::RESET);
        }
        isLines.push_back(" Parries        : " + strParry);
        isLines.push_back("");
        std::vector<std::string> boxIs = BaseScreen::createBox(isLines, "ESTATISTICAS DE DESEMPENHO", 40, Color::CYAN);

        std::vector<std::string> progressLines;
        progressLines.push_back(" Evolucao do Personagem:");
        progressLines.push_back("");

        double xpPct = static_cast<double>(currentPlayer->getCurrentXp()) / std::max(1, currentPlayer->getXpForRise());
        std::string barXp = BaseScreen::generateBarGradient(xpPct, 12, Color::CYAN);
        progressLines.push_back(" XP: [" + barXp + Appearance::color(Color::RESET) + "] " + Appearance::color(Color::CYAN) + "+" + std::to_string(curXp) + Appearance::color(Color::RESET));

        int totalExcitedGold = currentPlayer->getInventory()->getGold() - obtainedGoldQuantity + curGold;
        progressLines.push_back(" Ouro Total: " + Appearance::color(Color::YELLOW) + std::to_string(totalExcitedGold) + "G " + Appearance::color(Color::RESET) + "(+" + std::to_string(curGold) + ")");
        progressLines.push_back("");

        std::vector<std::string> boxProgress = BaseScreen::createBox(progressLines, "PROGRESSAO DE BATALHA", 40, Color::YELLOW);

        std::vector<std::string> dropsLines;
        if (dropsUnique.empty()) {
            if (frame >= framesXP) {
                dropsLines.push_back(Appearance::color(Color::GRAY) + " Nenhum item dropado." + Appearance::color(Color::RESET));
            } else {
                dropsLines.push_back(Appearance::color(Color::GRAY) + " ???" + Appearance::color(Color::RESET));
            }
        } else {
            if (frame <= framesXP) {
                dropsLines.push_back(Appearance::color(Color::GRAY) + " ???" + Appearance::color(Color::RESET));
            } else {
                int itemsShow = std::min((int)dropsUnique.size(), frame - framesXP);
                for (int i = 0; i < itemsShow; ++i) {
                    dropsLines.push_back(" " + std::to_string(dropsUnique[i].second) + "x " + dropsUnique[i].first);
                }
            }
        }

        if (frame == framesTotal) {
            if (!newDiscoveries.empty()) {
                dropsLines.push_back("");
                for (const auto& desc : newDiscoveries) {
                    dropsLines.push_back(Appearance::color(Color::CYAN) + " [!] " + desc + Appearance::color(Color::RESET));
                }
            }
            if (canRiseLevel) {
                dropsLines.push_back("");
                dropsLines.push_back(Appearance::color(Color::GREEN) + " *** VOCE PODE SUBIR DE NIVEL! ***" + Appearance::color(Color::RESET));
            }

            dropsLines.push_back("");
            std::string enterMsg = "\033[5m[ PRESSIONE ENTER PARA CONTINUAR ]\033[0m";
            int paddingSize = (80 - 34) / 2;
            std::string padding(paddingSize > 0 ? paddingSize : 0, ' ');
            dropsLines.push_back(padding + Appearance::color(Color::GREEN) + enterMsg + Appearance::color(Color::RESET));
        }

        std::vector<std::string> boxDrops = BaseScreen::createBox(dropsLines, "SAQUE & DESCOBERTAS", 84, Color::MAGENTA);

        if (isMode3D) {
            auto overlayPanel = [](std::vector<std::string>& background, const std::vector<std::string>& art, int startY, int startX) {
                for (size_t i = 0; i < art.size(); ++i) {
                    int y = startY + i;
                    if (y >= 0 && y < static_cast<int>(background.size())) {
                        background[y] = Appearance::superimposePanelOnAnsiLine(background[y], art[i], startX);
                    }
                }
            };

            std::vector<std::string> colorfulLogo = ArtsVictory::victoryLogo;
            for (auto& l : colorfulLogo) l = Appearance::color(Color::GREEN) + l + Appearance::color(Color::RESET);

            int terminalWidth = Appearance::getTerminalWidth();

            int soonWidth = 0;
            for(const auto& l : colorfulLogo) {
                soonWidth = std::max(soonWidth, Appearance::getVisualLength(l));
            }
            int soonX = (terminalWidth - soonWidth) / 2;
            if (soonX < 0) soonX = 0;

            int isWidth = 40;
            int programWidth = 40;
            int gap = 4;
            int totalTopWidth = isWidth + gap + programWidth;

            int startIsX = (terminalWidth - totalTopWidth) / 2;
            if (startIsX < 0) startIsX = 0;
            int startProgramX = startIsX + isWidth + gap;

            int startDropsX = (terminalWidth - 84) / 2;
            if (startDropsX < 0) startDropsX = 0;

            overlayPanel(screen3D, colorfulLogo, 2, soonX);
            overlayPanel(screen3D, boxIs, 16, startIsX);
            overlayPanel(screen3D, boxProgress, 16, startProgramX);

            int yDrops = 16 + boxProgress.size() + 1;
            overlayPanel(screen3D, boxDrops, yDrops, startDropsX);
        } else {
            Appearance::printSideASide(boxIs, boxProgress, 43, 4);
            std::cout << "\n";
            Appearance::printCentralizedMultiline(boxDrops);
        }

        if (!isMode3D) {
            std::cout << "\n";
        }

        if (isMode3D) {
            std::string renderStr = "";
            for (size_t i = 0; i < screen3D.size(); ++i) {
                renderStr += screen3D[i];
                if (i < screen3D.size() - 1) {
                    renderStr += "\n";
                }
            }
            std::cout << "\033[H" << renderStr;
        }

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str();
        if (!isMode3D) std::cout << "\033[J";
        std::cout << std::flush;

        if (frame < framesTotal) {
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    if (isMode3D) {
        std::cout << "\033[33;1H";
    }
    InputControl::waitForEnter("");
}
