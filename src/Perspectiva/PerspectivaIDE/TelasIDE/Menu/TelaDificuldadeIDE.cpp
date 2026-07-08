#include "TelaDificuldadeIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

TelaDificuldade::Resultado TelaDificuldadeIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> opcoes = {"FACIL", "NORMAL", "DIFICIL", "Voltar (Classe)"};
    
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + reset);
        blocoCentral.push_back(colorComment + "// Selecione o nivel de desafio da sua jornada" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "DifficultyLevel " + colorPunct + "{");
        
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = opcoes[i];
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            if (i < (int)opcoes.size() - 1) {
                linha += colorPunct + ",";
            }
            blocoCentral.push_back(linha);
        }
        
        blocoCentral.push_back(colorPunct + "};");
        blocoCentral.push_back("");

        if (selecaoAtual == 0) {
            blocoCentral.push_back(colorComment + "// FACIL: Inimigos com 1x Atributos, sem habilidades adicionais" + reset);
        } else if (selecaoAtual == 1) {
            blocoCentral.push_back(colorComment + "// NORMAL: Inimigos com 1.5x Atributos, com habilidades de raca" + reset);
        } else if (selecaoAtual == 2) {
            blocoCentral.push_back(colorComment + "// DIFICIL: Inimigos com 2x Atributos, com habilidades de raca e classe" + reset);
        }

        int espacosY = TelaMenuIDE::calcularEspacoY(blocoCentral.size());
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";
        
        TelaMenuIDE::imprimirBlocoCentralizadoIDE(blocoCentral);

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            break;
        }
    }

    if (selecaoAtual == 3) {
        TelaDificuldade::Resultado r;
        r.voltou = true;
        return r;
    }

    TelaDificuldade::Resultado r;
    r.indice = selecaoAtual;
    return r;
}