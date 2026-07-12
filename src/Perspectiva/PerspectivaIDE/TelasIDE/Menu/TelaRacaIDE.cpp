#include "TelaRacaIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Racas/FabricaRacas.h"
#include "../../../../Entidades/Personagem.h"

struct OpcaoRaca { TipoRaca tipo; std::string nome; };

TelaRaca::Resultado TelaRacaIDE::exibir(const std::string& nomeJogador) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<OpcaoRaca> opcoesGerais;
    for (auto t : FabricaRacas::obterRacasJogaveis()) {
        auto temp = FabricaRacas::criarRaca(t);
        opcoesGerais.push_back({t, temp->obterNomeRaca()});
    }
    std::sort(opcoesGerais.begin(), opcoesGerais.end(), [](const OpcaoRaca& a, const OpcaoRaca& b) { return a.nome < b.nome; });
    
    int totalOpcoes = (int)opcoesGerais.size() + 1;
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// JOGADOR: " + nomeJogador + reset);
        blocoCentral.push_back(colorComment + "// Selecione sua raca" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "Race " + colorPunct + "{");
        
        for (int i = 0; i < totalOpcoes; ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = (i == (int)opcoesGerais.size()) ? "Voltar (Nome)" : opcoesGerais[i].nome;
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            if (i < totalOpcoes - 1) {
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
            selecaoAtual = (selecaoAtual - 1 + totalOpcoes) % totalOpcoes;
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % totalOpcoes;
        } else if (tecla == '\r' || tecla == '\n') {
            break;
        }
    }

    if (selecaoAtual == (int)opcoesGerais.size()) {
        TelaRaca::Resultado r;
        r.voltou = true;
        return r;
    }

    std::string racaNome = opcoesGerais[selecaoAtual].nome;
    std::vector<std::string> arteRaca;
    std::vector<std::string> infoRaca = { "Raca: " + racaNome };

    auto racaInstancia = FabricaRacas::criarRaca(opcoesGerais[selecaoAtual].tipo);
    std::vector<std::string> arteOriginal = racaInstancia->obterAparenciaRaca();
    
    Atributos atr = racaInstancia->obterAtributosRaca();
    if (atr.forca != 0) infoRaca.push_back("Forca " + std::string(atr.forca > 0 ? "+" : "") + std::to_string(atr.forca));
    if (atr.destreza != 0) infoRaca.push_back("Destreza " + std::string(atr.destreza > 0 ? "+" : "") + std::to_string(atr.destreza));
    if (atr.constituicao != 0) infoRaca.push_back("Constituicao " + std::string(atr.constituicao > 0 ? "+" : "") + std::to_string(atr.constituicao));
    if (atr.inteligencia != 0) infoRaca.push_back("Inteligencia " + std::string(atr.inteligencia > 0 ? "+" : "") + std::to_string(atr.inteligencia));
    if (atr.sabedoria != 0) infoRaca.push_back("Sabedoria " + std::string(atr.sabedoria > 0 ? "+" : "") + std::to_string(atr.sabedoria));
    if (atr.resistencia != 0) infoRaca.push_back("Resistencia " + std::string(atr.resistencia > 0 ? "+" : "") + std::to_string(atr.resistencia));
    
    arteRaca = TelaMenuIDE::comprimirArteASCII(arteOriginal, 3, 3);

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("RACA", racaNome, infoRaca, arteRaca);
    if (!confirmou) {
        return exibir(nomeJogador); // recursion on cancel
    }

    TelaRaca::Resultado r;
    r.indice = selecaoAtual;
    r.nome = racaNome;
    r.racaSelecionada = opcoesGerais[selecaoAtual].tipo;
    return r;
}