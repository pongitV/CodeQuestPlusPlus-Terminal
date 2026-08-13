#include "UI/Renderers/IDE/IDEScreens/Pause/IDEPauseScreen.h"
#include <iostream>
#include <string>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Character.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "UI/PerspectiveManager.h"

int IDEPauseScreen::renderMenuPause() {
    std::cout << "\033[?25l";
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("JOGO EM PAUSE", false);
    std::cout << "\n";

    std::vector<std::string> options = {
        "Voltar ao jogo",
        "Configuracoes",
        "Sair do jogo"
    };

    return InputControl::readSelectionMenuWithArrows(options, true);
}

int IDEPauseScreen::renderSettingsMenu(Character* player) {
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("CONFIGURACOES", false);
    std::cout << "\n";

    std::string difStr;
    switch (player->getDifficulty()) {
        case GameDifficulty::Easy: difStr = Appearance::color(Color::GREEN) + "Facil" + Appearance::color(Color::RESET); break;
        case GameDifficulty::Normal: difStr = Appearance::color(Color::YELLOW) + "Normal" + Appearance::color(Color::RESET); break;
        case GameDifficulty::Difficult: difStr = Appearance::color(Color::RED) + "Dificil" + Appearance::color(Color::RESET); break;
    }

    std::string statusParry = player->getParryActivated() ? Appearance::color(Color::GREEN) + "LIGADO" + Appearance::color(Color::RESET) : Appearance::color(Color::RED) + "DESLIGADO" + Appearance::color(Color::RESET);
    std::string typeParryStr = player->getParryModern() ? Appearance::color(Color::CYAN) + "Movimento (Moderno)" + Appearance::color(Color::RESET) : Appearance::color(Color::RED) + "Digitacao (Antigo)" + Appearance::color(Color::RESET);

    std::vector<std::string> optionsConfig = {
        "Dificuldade do Mundo: " + difStr,
        "Sistema de PARRY: " + statusParry,
        "Metodo do PARRY: " + typeParryStr,
        "Aparencia do Jogador no Mapa",
        "Cor de Fundo do Terminal",
        "Sensibilidade do Mouse",
        "Voltar"
    };

    return InputControl::readSelectionMenuWithArrows(optionsConfig, true);
}

int IDEPauseScreen::renderMenuAppearance(Character* player) {
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("APARENCIA NO MAPA", false);
    std::cout << "\n";

    std::string colorCurrentStr;
    switch (Appearance::customPlayerColor) {
        case Color::GREEN: colorCurrentStr = Appearance::color(Color::GREEN) + "Verde" + Appearance::color(Color::RESET); break;
        case Color::BLUE: colorCurrentStr = Appearance::color(Color::BLUE) + "Azul" + Appearance::color(Color::RESET); break;
        case Color::YELLOW: colorCurrentStr = Appearance::color(Color::YELLOW) + "Amarelo" + Appearance::color(Color::RESET); break;
        case Color::WHITE: colorCurrentStr = Appearance::color(Color::WHITE) + "Branco" + Appearance::color(Color::RESET); break;
        case Color::MAGENTA: colorCurrentStr = Appearance::color(Color::MAGENTA) + "Magenta" + Appearance::color(Color::RESET); break;
        case Color::CYAN: colorCurrentStr = Appearance::color(Color::CYAN) + "Ciano" + Appearance::color(Color::RESET); break;
        case Color::RED: colorCurrentStr = Appearance::color(Color::RED) + "Vermelho" + Appearance::color(Color::RESET); break;
        default: colorCurrentStr = Appearance::color(Color::WHITE) + "Padrao" + Appearance::color(Color::RESET); break;
    }

    std::vector<std::string> optionsAppearance = {
        "Cor do Jogador: " + colorCurrentStr,
        "Icone do Jogador: " + Appearance::color(Appearance::customPlayerColor) + std::string(1, Appearance::customPlayerIcon) + Appearance::color(Color::RESET),
        "Voltar"
    };

    return InputControl::readSelectionMenuWithArrows(optionsAppearance, true);
}

int IDEPauseScreen::renderMenuBackground(int colorBackgroundCurrentIndex) {
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("COR DE FUNDO", false);
    std::cout << "\n";

    std::vector<std::string> namesFunds = {
        "Preto (Padrao)", "Cinza Escuro", "Azul Escuro",
        "Vermelho Escuro", "Verde Escuro", "Roxo Escuro"
    };
    std::vector<std::string> optionsBackground;
    for (int i = 0; i < 6; ++i) {
        if (i == colorBackgroundCurrentIndex) optionsBackground.push_back(namesFunds[i] + Appearance::color(Color::GREEN) + " [ATIVO]" + Appearance::color(Color::RESET));
        else optionsBackground.push_back(namesFunds[i]);
    }
    optionsBackground.push_back("Voltar");

    return InputControl::readSelectionMenuWithArrows(optionsBackground, true);
}

int IDEPauseScreen::renderMenuSensitivity(int percentX, int percentY) {
    Appearance::clearScreen();
    MenuScreen::displayGameLogoPanel("SENSIBILIDADE", false);
    std::cout << "\n";

    std::vector<std::string> optionsSens = {
        "Horizontal (X): " + std::to_string(percentX) + "%  [ ENTER p/ Digitar Novo Valor ]",
        "Vertical (Y): " + std::to_string(percentY) + "%  [ ENTER p/ Digitar Novo Valor ]",
        "Voltar"
    };

    return InputControl::readSelectionMenuWithArrows(optionsSens, true);
}
