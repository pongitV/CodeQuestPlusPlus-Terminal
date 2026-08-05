#include "Core/Terminal/Appearance/Appearance.h"
#include "UI/Screens/BaseScreen.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

namespace {
    std::vector<std::string> historyBattle;
}

void Appearance::registerBattleLog(const std::string& text) {
    if (!historyBattle.empty() && historyBattle.back() == text) return;
    historyBattle.push_back(text);
    CombatScreen::addFixedMessage(text);
}

void Appearance::cleanLogBattle() {
    historyBattle.clear();
}

void Appearance::displayLatestLogs(int quantity) {
    if (historyBattle.empty()) return;
    int home = std::max(0, static_cast<int>(historyBattle.size()) - quantity);
    int total = static_cast<int>(historyBattle.size()) - home;
    for (int i = 0; i < total; ++i) {
        int index = home + i;
        printCentralized(removeANSIColors(historyBattle[index]), color(Color::GRAY));
    }
    std::cout << "\n";
}

void Appearance::displayHistoryComplete() {
    if (historyBattle.empty()) {
        clearScreen();
        displayTextPanel("HISTORICO DE BATALHA", Color::CYAN);
        printCentralized("O historico esta vazio.", color(Color::GRAY));
        std::cout << "\n";
        BaseScreen::printLineDivider();
        InputControl::waitForEnter();
        return;
    }

    int linesByPage = std::max(5, getTerminalHeight() - 12);
    int totalPages = (static_cast<int>(historyBattle.size()) + linesByPage - 1) / linesByPage;
    int pageCurrent = totalPages - 1;

    while (true) {
        clearScreen();
        displayTextPanel("HISTORICO DE BATALHA", Color::CYAN);

        int home = pageCurrent * linesByPage;
        int end = std::min(home + linesByPage, static_cast<int>(historyBattle.size()));
        std::vector<std::string> page(historyBattle.begin() + home, historyBattle.begin() + end);
        
        printBlockCentralized(page);
        std::cout << "\n";
        BaseScreen::printLineDivider();
        printCentralized("Pagina " + std::to_string(pageCurrent + 1) + " de " + std::to_string(totalPages), color(Color::CYAN));
        
        if (totalPages > 1) {
            std::string choice = InputControl::readEntryProtected("[A] Pagina Anterior | [D] Proxima Pagina | [0] Sair\n\nEscolha: ");
            if (choice == "0") break;
            if ((choice == "a" || choice == "A") && pageCurrent > 0) pageCurrent--;
            if ((choice == "d" || choice == "D") && pageCurrent < totalPages - 1) pageCurrent++;
        } else {
            std::cout << "\n";
            InputControl::waitForEnter();
            break;
        }
    }
    std::cout << "\n";
}
