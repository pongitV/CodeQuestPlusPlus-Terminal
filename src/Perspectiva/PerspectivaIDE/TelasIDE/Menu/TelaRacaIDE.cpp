#include "TelaRacaIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../../Entidades/Racas/Anao.h"
#include "../../../../Entidades/Racas/Elfo.h"
#include "../../../../Entidades/Racas/Humano.h"
#include "../../../../Entidades/Racas/Orc.h"

TelaRaca::Resultado TelaRacaIDE::exibir(const std::string& nomeJogador) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> racas = {"Dwarf", "Elfo", "Humano", "Ork", "Voltar (Nome)"};
    
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// JOGADOR: " + nomeJogador + reset);
        blocoCentral.push_back(colorComment + "// Selecione sua raca" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "Race " + colorPunct + "{");
        
        for (int i = 0; i < (int)racas.size(); ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = racas[i];
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            if (i < (int)racas.size() - 1) {
                linha += colorPunct + ",";
            }
            blocoCentral.push_back(linha);
        }
        
        blocoCentral.push_back(colorPunct + "};");

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
            selecaoAtual = (selecaoAtual - 1 + (int)racas.size()) % (int)racas.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)racas.size();
        } else if (tecla == '\r' || tecla == '\n') {
            break;
        }
    }

    if (selecaoAtual == 4) {
        TelaRaca::Resultado r;
        r.voltou = true;
        return r;
    }

    std::string racaNome = racas[selecaoAtual];
    std::vector<std::string> arteRaca;
    std::vector<std::string> infoRaca = { "Raca: " + racaNome };

    std::vector<std::string> arteOriginal;
    if (racaNome == "Dwarf") {
        arteOriginal = std::make_unique<Dwarf>()->obterAparenciaRaca();
        infoRaca.push_back("Resistencia +2");
        infoRaca.push_back("Constituicao +2");
    } else if (racaNome == "Elfo") {
        arteOriginal = std::make_unique<Elfo>()->obterAparenciaRaca();
        infoRaca.push_back("Destreza +2");
        infoRaca.push_back("Sabedoria +1");
    } else if (racaNome == "Humano") {
        arteOriginal = std::make_unique<Humano>()->obterAparenciaRaca();
        infoRaca.push_back("Forca +1");
        infoRaca.push_back("Inteligencia +1");
        infoRaca.push_back("Sabedoria +1");
    } else if (racaNome == "Ork") {
        arteOriginal = std::make_unique<Ork>()->obterAparenciaRaca();
        infoRaca.push_back("Forca +3");
        infoRaca.push_back("Destreza -1");
    }
    
    arteRaca = TelaMenuIDE::comprimirArteASCII(arteOriginal, 3, 3);

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", racaNome, infoRaca, arteRaca);
    if (!confirmou) {
        return exibir(nomeJogador);
    }

    TelaRaca::Resultado r;
    r.indice = selecaoAtual;
    return r;
}