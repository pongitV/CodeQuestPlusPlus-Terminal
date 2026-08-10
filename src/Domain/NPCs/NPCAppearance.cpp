#include "Domain/NPCs/NPCAppearance.h"
#include "Domain/NPCs/NPCAppearanceLayout.h"
#include "Core/Engine/GameMenu.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Progression/Progression.h"
#include <iostream>

void NPCAppearance::interact(Character* player) {
    InputControl::executeLoopMenuPopup(
        [this, player]() { this->displayDialogue(player); },
        [this, player]() { return this->getOptionsMenu(player, Appearance::getTerminalWidth()); },
        [this, player](const std::string& op) { this->processOption(player, op, Appearance::getTerminalWidth()); return true; },
        getPlaceName(), getHeaderColor(), getArtASCII()
    );
}

std::string NPCAppearance::getPlaceName() const {
    return "SALA DE CUSTOMIZACAO";
}

Color NPCAppearance::getHeaderColor() const {
    return Color::BROWN_OFCOURSE;
}

Color NPCAppearance::getArtColor() const {
    return Color::BROWN_OFCOURSE;
}

const std::vector<std::string>& NPCAppearance::getArtASCII() const {
    return NPCAppearanceLayouts::artAppearance;
}

void NPCAppearance::displayDialogue(Character* player) {
    std::vector<std::string> lines = {
        "Saudacoes, viajante! Eu sou Anok.",
        "Deseja renovar seu estilo?",
        "Aqui voce pode comprar novos icones de exibicao para o mapa",
        "e novas cores de fundo do terminal para deixar sua jornada unica!"
    };
    Appearance::displayPopup("ANOK - ESTILISTA REAL", lines, Color::BROWN_OFCOURSE, getArtASCII());
}

std::vector<std::string> NPCAppearance::getOptionsMenu(Character* player, int /*larguraDoTerminal*/) {
    return {
        "Comprar Icones",
        "Comprar Cores de Fundo",
        "Mudar Aparencia Atual",
        "Voltar"
    };
}

void NPCAppearance::processOption(Character* player, const std::string& option, int terminalWidth) {
    auto& progress = Progression::instance();

    if (option == "Comprar Icones") {
        std::vector<std::pair<std::string, std::pair<char, int>>> iconsStore = {
            {"Coracao (â™¥)", {'H', 100}}, // Usando caractere comum H ou simbolo se o terminal suportar. Para seguranca de UTF-8, usamos caracteres visiveis elegantes
            {"Estrela (*)", {'S', 150}},
            {"Espadas (X)", {'X', 200}},
            {"Coroa (K)", {'K', 400}},
            {"Cifrao ($)", {'$', 300}}
        };

        std::vector<std::string> optionsItem;
        std::vector<std::pair<std::string, std::pair<char, int>>> available;

        for (const auto& item : iconsStore) {
            std::string flag = "Aparencia_Icone_" + std::string(1, item.second.first);
            if (!progress.getFlag(flag)) {
                optionsItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                available.push_back(item);
            }
        }

        if (optionsItem.empty()) {
            Appearance::displayPopup("ANOK", {"Voce ja comprou todos os icones disponiveis!"}, Color::BROWN_OFCOURSE, getArtASCII());
            return;
        }

        optionsItem.push_back("Voltar");

        int choice = InputControl::readMenuSelectionInPopup(
            "COMPRAR ICONES",
            {"Seu Ouro: " + std::to_string(player->getInventory()->getGold()) + "G", "Selecione um icone para comprar:"},
            optionsItem, Color::BROWN_OFCOURSE, getArtASCII()
        );

        if (choice >= 0 && choice < (int)available.size()) {
            auto selected = available[choice];
            if (player->getInventory()->getGold() >= selected.second.second) {
                player->getInventory()->addGold(-selected.second.second);
                std::string flag = "Aparencia_Icone_" + std::string(1, selected.second.first);
                progress.setFlag(flag, true);
                
                Appearance::displayPopup("COMPRA REALIZADA", {"Voce comprou o icone: " + selected.first}, Color::BROWN_OFCOURSE, getArtASCII());
            } else {
                Appearance::displayPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para esta compra!"}, Color::RED, getArtASCII());
            }
        }
    }
    else if (option == "Comprar Cores de Fundo") {
        std::vector<std::pair<std::string, std::pair<Color, int>>> colorsStore = {
            {"Fundo Azul", {Color::BG_BLUE, 150}},
            {"Fundo Verde", {Color::BG_GREEN, 150}},
            {"Fundo Vermelho", {Color::BG_RED, 200}},
            {"Fundo Magenta", {Color::BG_MAGENTA, 250}},
            {"Fundo Ciano", {Color::BG_CYAN, 250}}
        };

        std::vector<std::string> optionsItem;
        std::vector<std::pair<std::string, std::pair<Color, int>>> available;

        for (const auto& item : colorsStore) {
            std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second.first));
            if (!progress.getFlag(flag)) {
                optionsItem.push_back(item.first + " - " + std::to_string(item.second.second) + "G");
                available.push_back(item);
            }
        }

        if (optionsItem.empty()) {
            Appearance::displayPopup("ANOK", {"Voce ja comprou todas as cores disponiveis!"}, Color::BROWN_OFCOURSE, getArtASCII());
            return;
        }

        optionsItem.push_back("Voltar");

        int choice = InputControl::readMenuSelectionInPopup(
            "COMPRAR CORES DE FUNDO",
            {"Seu Ouro: " + std::to_string(player->getInventory()->getGold()) + "G", "Selecione uma cor para comprar:"},
            optionsItem, Color::BROWN_OFCOURSE, getArtASCII()
        );

        if (choice >= 0 && choice < (int)available.size()) {
            auto selected = available[choice];
            if (player->getInventory()->getGold() >= selected.second.second) {
                player->getInventory()->addGold(-selected.second.second);
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(selected.second.first));
                progress.setFlag(flag, true);
                
                Appearance::displayPopup("COMPRA REALIZADA", {"Voce comprou a cor de fundo: " + selected.first}, Color::BROWN_OFCOURSE, getArtASCII());
            } else {
                Appearance::displayPopup("OURO INSUFICIENTE", {"Voce nao tem ouro suficiente para esta compra!"}, Color::RED, getArtASCII());
            }
        }
    }
    else if (option == "Mudar Aparencia Atual") {
        std::vector<std::string> subOptions = {"Mudar Icone do Jogador", "Mudar Cor de Fundo do Terminal", "Voltar"};
        int subChoose = InputControl::readMenuSelectionInPopup(
            "MUDAR APARENCIA",
            {"Escolha o que deseja customizar:"},
            subOptions, Color::BROWN_OFCOURSE, getArtASCII()
        );

        if (subChoose == 0) {
            // Icone
            std::vector<std::pair<std::string, char>> iconsAvailable = {
                {"Icone Padrao (@)", '@'}
            };

            std::vector<std::pair<std::string, char>> iconsStore = {
                {"Coracao (â™¥)", 'H'},
                {"Estrela (*)", 'S'},
                {"Espadas (X)", 'X'},
                {"Coroa (K)", 'K'},
                {"Cifrao ($)", '$'}
            };

            for (const auto& item : iconsStore) {
                std::string flag = "Aparencia_Icone_" + std::string(1, item.second);
                if (progress.getFlag(flag)) {
                    iconsAvailable.push_back(item);
                }
            }

            std::vector<std::string> optionsMenu;
            for (const auto& ic : iconsAvailable) {
                std::string status = (Appearance::customPlayerIcon == ic.second) ? " (Equipado)" : "";
                optionsMenu.push_back(ic.first + status);
            }
            optionsMenu.push_back("Voltar");

            int choice = InputControl::readMenuSelectionInPopup(
                "SELECIONAR ICONE",
                {"Selecione o icone de exibicao no mapa:"},
                optionsMenu, Color::BROWN_OFCOURSE, getArtASCII()
            );

            if (choice >= 0 && choice < (int)iconsAvailable.size()) {
                Appearance::customPlayerIcon = iconsAvailable[choice].second;
                Appearance::displayPopup("ICONE ALTERADO", {"Icone alterado com sucesso para: " + iconsAvailable[choice].first}, Color::BROWN_OFCOURSE, getArtASCII());
            }
        }
        else if (subChoose == 1) {
            // Cor de Fundo
            std::vector<std::pair<std::string, Color>> colorsAvailable = {
                {"Fundo Padrao (Preto)", Color::RESET}
            };

            std::vector<std::pair<std::string, Color>> colorsStore = {
                {"Fundo Azul", Color::BG_BLUE},
                {"Fundo Verde", Color::BG_GREEN},
                {"Fundo Vermelho", Color::BG_RED},
                {"Fundo Magenta", Color::BG_MAGENTA},
                {"Fundo Ciano", Color::BG_CYAN}
            };

            for (const auto& item : colorsStore) {
                std::string flag = "Aparencia_Fundo_" + std::to_string(static_cast<uint32_t>(item.second));
                if (progress.getFlag(flag)) {
                    colorsAvailable.push_back(item);
                }
            }

            std::vector<std::string> optionsMenu;
            for (const auto& c : colorsAvailable) {
                std::string status = (Appearance::colorBackgroundActive == c.second) ? " (Equipado)" : "";
                optionsMenu.push_back(c.first + status);
            }
            optionsMenu.push_back("Voltar");

            int choice = InputControl::readMenuSelectionInPopup(
                "SELECIONAR COR DE FUNDO",
                {"Selecione a cor de fundo do terminal:"},
                optionsMenu, Color::BROWN_OFCOURSE, getArtASCII()
            );

            if (choice >= 0 && choice < (int)colorsAvailable.size()) {
                Appearance::colorBackgroundActive = colorsAvailable[choice].second;
                Appearance::clearScreen(); // Limpa e redesenha a tela inteira para aplicar a cor de fundo
                Appearance::displayPopup("COR ALTERADA", {"Cor de fundo alterada com sucesso!"}, Color::BROWN_OFCOURSE, getArtASCII());
            }
        }
    }
}

