#include "TelaClasseIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Classes/FabricaClasses.h"
#include "../../../../Entidades/Personagem.h"

struct OpcaoClasse { TipoClasse tipo; std::string nome; };

TelaClasse::Resultado TelaClasseIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<OpcaoClasse> opcoesGerais;
    for (auto t : FabricaClasses::obterClassesJogaveis()) {
        auto temp = FabricaClasses::criarClasse(t);
        opcoesGerais.push_back({t, temp->obterNomeClasse()});
    }
    std::sort(opcoesGerais.begin(), opcoesGerais.end(), [](const OpcaoClasse& a, const OpcaoClasse& b) { return a.nome < b.nome; });
    
    int totalOpcoes = (int)opcoesGerais.size() + 1;
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + reset);
        blocoCentral.push_back(colorComment + "// Selecione sua classe" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "ClassRole " + colorPunct + "{");
        
        for (int i = 0; i < totalOpcoes; ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = (i == (int)opcoesGerais.size()) ? "Voltar (Raca)" : opcoesGerais[i].nome;
            
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
        TelaClasse::Resultado r;
        r.voltou = true;
        return r;
    }

    std::string classeNome = opcoesGerais[selecaoAtual].nome;
    std::vector<std::string> arteClasse;
    std::vector<std::string> infoClasse = { "Classe: " + classeNome };

    auto classeInstancia = FabricaClasses::criarClasse(opcoesGerais[selecaoAtual].tipo);
    std::vector<std::string> arteOriginal = classeInstancia->obterAparenciaClasseMenu();
    Atributos statsBase = classeInstancia->obterAtributosClasse();
    
    std::string habilidadeString = "Habilidade: " + classeInstancia->obterNomeHabilidadeClasse();
    infoClasse.push_back(habilidadeString);
    
    arteClasse = TelaMenuIDE::comprimirArteASCII(arteOriginal, 2, 2);

    std::vector<std::string> quadroAtributos = TelaMenuIDE::comporQuadroDeAtributos(
        statsBase, "ATRIBUTOS BASE", "HABILIDADE UNICA", classeNome,
        habilidadeString);

    for (const auto& linha : quadroAtributos) {
        infoClasse.push_back(linha);
    }

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", classeNome, infoClasse, arteClasse);
    if (!confirmou) {
        return exibir(nomeJogador, nomeRaca);
    }

    TelaClasse::Resultado r;
    r.indice = selecaoAtual;
    r.nome = classeNome;
    r.classeSelecionada = opcoesGerais[selecaoAtual].tipo;
    return r;
}