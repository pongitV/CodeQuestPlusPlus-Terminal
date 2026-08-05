#include "UI/Renderers/IDE/IDEScreens/Bestiary/IDEBestiaryScreen.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <chrono>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Bestiary.h"
#include "Domain/Characters/Character.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Bestiary/BestiaryScreenLayout.h"

namespace {
    void displayHeader(int width, const std::string& titleSecondary = "", bool animate = false) {
        Appearance::clearScreen();
        Appearance::displayArtPanel(ArtsBestiary::soonBestiary, 101, Color::GREEN, titleSecondary, animate);
    }
}

void ScreenBestiaryGO::displayList(Character* currentPlayer) {
    if (currentPlayer == nullptr) return;

    int width = Appearance::getTerminalWidth();
    Bestiary& bestiary = Bestiary::instance();
    const auto& enemies = bestiary.getEnemiesOrderedByDifficulty();

    std::vector<std::string> discovered;
    std::copy_if(enemies.begin(), enemies.end(), std::back_inserter(discovered),
                 [&](const std::string& name) { return bestiary.thisDiscovered(name); });

    Appearance::sortAlphabetically(discovered);

    static auto lastAccess = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animateEntrance = BaseScreen::mustAnimateScreenEntry(lastAccess, 300);

    if (discovered.empty()) {
        displayHeader(width, "", animateEntrance);
        Appearance::printCentralized("Nenhum inimigo descoberto ainda.");
        Appearance::printCentralized("Explore e combata para desbloquear entries.");
        std::cout << "\n";
        Appearance::displayPrompt("[0] Voltar\n\nEscolha: ");
        std::string choice = InputControl::readEntryProtected();
        return;
    }

    constexpr int quantityMaximumByPage = 10;
    int totalDiscovered = static_cast<int>(discovered.size());
    int totalDePages = std::max(1, (totalDiscovered + quantityMaximumByPage - 1) / quantityMaximumByPage);
    int pageCurrent = 0;

    while (true) {
        displayHeader(width, "", animateEntrance);
        animateEntrance = false;

        Appearance::printCentralized("Encontrados: " + std::to_string(totalDiscovered) + "/" + std::to_string(enemies.size()));
        std::cout << "\n";

        int indexInitial = pageCurrent * quantityMaximumByPage;
        int indexEnd = std::min(indexInitial + quantityMaximumByPage, totalDiscovered);

        Appearance::printCentralized("═══ INIMIGOS ═══");
        std::cout << "\n";

        for (int i = indexInitial; i < indexEnd; ++i) {
            const std::string& nameSelected = discovered[i];
            const SystemBestiaryEnemyInfo* info = bestiary.getInfo(nameSelected);
            if (!info) continue;

            std::string lineEnemy = "[" + std::to_string(i + 1) + "] " + info->name + " (" + info->map + " - " + info->habitat + ")";
            Appearance::printCentralized(lineEnemy);
        }

        std::cout << "\n";
        if (totalDePages > 1) {
            Appearance::printCentralized("[P] Pagina " + std::to_string(pageCurrent + 1) + "/" + std::to_string(totalDePages), Appearance::color(Color::CYAN));
            std::cout << "\n";
        }

        Appearance::displayPrompt("Escolha um numero (1-" + std::to_string(indexEnd) + "), [P] pagina, [0] Voltar: ");
        std::string entryTypedFurPlayer = InputControl::readEntryProtected();

        if (entryTypedFurPlayer == "p" || entryTypedFurPlayer == "P") {
            pageCurrent = (pageCurrent + 1) % totalDePages;
            continue;
        } else if (entryTypedFurPlayer == "0") {
            return;
        }

        try {
            int choice = std::stoi(entryTypedFurPlayer);
            if (choice >= 1 && choice <= indexEnd) {
                displaySheet(currentPlayer, discovered[choice - 1], choice - 1, discovered);
            }
        } catch (const std::invalid_argument&) {
        } catch (const std::out_of_range&) {
        }
    }
}

void ScreenBestiaryGO::displaySheet(Character* currentPlayer, const std::string& nameSelected, int /*indiceDescoberto*/, const std::vector<std::string>& /*descobertos*/) {
    if (currentPlayer == nullptr) return;

    int width = Appearance::getTerminalWidth();
    Bestiary& bestiary = Bestiary::instance();

    const SystemBestiaryEnemyInfo* info = bestiary.getInfo(nameSelected);
    if (!info) return;

    bool visa = bestiary.thisDiscovered(nameSelected);
    bool defeated = bestiary.jaDefeated(nameSelected);
    int timesDefeated = bestiary.getQuantityDefeats(nameSelected);

    auto printDryBase = [width](const std::string& titleDaDry, bool mustDisplayContent, const std::function<void()>& functionForDisplayContent, const std::string& textCaseHidden) {
        Appearance::printCentralized("═══ " + titleDaDry + " ═══");
        std::cout << "\n";
        if (mustDisplayContent) {
            functionForDisplayContent();
        } else {
            Appearance::printCentralized("???", Appearance::color(Color::GRAY));
            Appearance::printCentralized(textCaseHidden, Appearance::color(Color::GRAY));
        }
        std::cout << "\n";
        BaseScreen::printLineDivider('-');
        std::cout << "\n";
    };

    while (true) {
        displayHeader(width, "FICHA DO INIMIGO");

        printDryBase("APARENCIA", visa, [&]() {
            Appearance::printCentralized(info->name);
            if (timesDefeated > 0) {
                Appearance::printCentralized("Derrotado: " + std::to_string(timesDefeated) + " vezes", Appearance::color(Color::YELLOW));
            }
            std::cout << "\n";
            Appearance::printCentralizedMultiline(info->appearance);
            std::cout << "\n";
            Appearance::printCentralized("Lore: " + info->lore);
            std::cout << "\n";
            Appearance::printCentralized("Fato Curioso: " + info->factCurious);
        }, "(Nunca viu este inimigo em combate)");

        printDryBase("ATRIBUTOS", defeated, [&]() {
            std::vector<std::string> attributesClean = info->attributesText;
            for (auto& line : attributesClean) {
                size_t post = 0;
                while ((post = line.find('|', post)) != std::string::npos) {
                    line.replace(post, 1, "║");
                    post += 3;
                }
            }
            Appearance::printBlockCentralized(attributesClean);
        }, "(Derrote o inimigo para descobrir os atributos)");

        Appearance::printCentralized("═══ HABILIDADES ═══");
        std::cout << "\n";

        std::vector<std::string> blockSkills;

        auto processSkill = [&](const std::string& hab) {
            std::string habClean = hab;
            size_t postPipe = 0;
            while ((postPipe = habClean.find('|', postPipe)) != std::string::npos) {
                habClean.replace(postPipe, 1, "║");
                postPipe += 3;
            }
            blockSkills.push_back("  - " + habClean);
        };

        blockSkills.push_back("Ativas:");
        if (info->skillsActive.empty() || info->skillsActive[0].find("Nenhuma") != std::string::npos) {
            blockSkills.push_back(Appearance::color(Color::GRAY) + "  Nenhuma" + Appearance::color(Color::RESET));
        } else {
            for (const auto& hab : info->skillsActive) {
                processSkill(hab);
            }
        }
        blockSkills.push_back("");

        blockSkills.push_back("Passivas:");
        if (info->skillPassive.empty() || info->skillPassive.find("Nenhuma") != std::string::npos) {
            blockSkills.push_back(Appearance::color(Color::GRAY) + "  Nenhuma" + Appearance::color(Color::RESET));
        } else {
            processSkill(info->skillPassive);
        }

        Appearance::printBlockCentralized(blockSkills);
        std::cout << "\n";
        BaseScreen::printLineDivider('-');
        std::cout << "\n";

        printDryBase("DROPS", true, [&]() {
            bool hasDrops = false;
            std::vector<std::string> blockDrops;
            for (const auto& drop : info->drops) {
                if (bestiary.jaCollectedDrop(nameSelected, drop)) {
                    blockDrops.push_back("  - " + drop);
                    hasDrops = true;
                }
            }
            if (!hasDrops) {
                blockDrops.push_back(Appearance::color(Color::GRAY) + "  ???" + Appearance::color(Color::RESET));
                blockDrops.push_back(Appearance::color(Color::GRAY) + "  (Colete drops para desbloquear)" + Appearance::color(Color::RESET));
            }
            Appearance::printBlockCentralized(blockDrops);
        }, "");

        Appearance::displayPrompt("[0] Retornar a lista\n\nEscolha: ");
        std::string choice = InputControl::readEntryProtected();
        if (choice == "0") break;
    }
}
