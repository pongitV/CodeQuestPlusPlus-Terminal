#include "TelaClasseIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Entidades/Classes/ClasseBase.h"
#include "../../../../Entidades/Classes/Arqueiro.h"
#include "../../../../Entidades/Classes/Bardo.h"
#include "../../../../Entidades/Classes/Guerreiro.h"
#include "../../../../Entidades/Classes/Mago.h"
#include "../../../../Entidades/Classes/Necromante.h"

TelaClasse::Resultado TelaClasseIDE::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> classes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante", "Voltar (Raca)"};
    
    int selecaoAtual = 0;
    ControleDeInput::limparBuffer();

    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// JOGADOR: " + nomeJogador + " | RACA: " + nomeRaca + reset);
        blocoCentral.push_back(colorComment + "// Selecione sua classe" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "ClassRole " + colorPunct + "{");
        
        for (int i = 0; i < (int)classes.size(); ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = classes[i];
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            if (i < (int)classes.size() - 1) {
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
            selecaoAtual = (selecaoAtual - 1 + (int)classes.size()) % (int)classes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)classes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            break;
        }
    }

    if (selecaoAtual == 5) {
        TelaClasse::Resultado r;
        r.voltou = true;
        return r;
    }

    std::string classeNome = classes[selecaoAtual];
    std::vector<std::string> arteClasse;
    std::vector<std::string> infoClasse = { "Classe: " + classeNome };

    std::vector<std::string> arteOriginal;
    Atributos statsBase;
    if (classeNome == "Arqueiro") {
        arteOriginal = std::make_unique<Arqueiro>()->obterAparenciaClasseMenu();
        statsBase = { 8, 6, 10, 7, 6, 8, 7 };
        infoClasse.push_back("Habilidade: Tiro Certeiro (+50% dano, 75% chance)");
    } else if (classeNome == "Bardo") {
        arteOriginal = std::make_unique<Bardo>()->obterAparenciaClasseMenu();
        statsBase = { 8, 5, 8, 6, 8, 10, 9 };
        infoClasse.push_back("Habilidade: Melodia Curativa (cura aliados)");
    } else if (classeNome == "Guerreiro") {
        arteOriginal = std::make_unique<Guerreiro>()->obterAparenciaClasseMenu();
        statsBase = { 12, 9, 7, 9, 9, 5, 5 };
        infoClasse.push_back("Habilidade: Golpe Imparavel (ignora defesa)");
    } else if (classeNome == "Mago") {
        arteOriginal = std::make_unique<Mago>()->obterAparenciaClasseMenu();
        statsBase = { 6, 4, 6, 5, 12, 10, 12 };
        infoClasse.push_back("Habilidade: Bola de Fogo (dano em area)");
    } else if (classeNome == "Necromante") {
        arteOriginal = std::make_unique<Necromante>()->obterAparenciaClasseMenu();
        statsBase = { 7, 4, 7, 5, 10, 9, 11 };
        infoClasse.push_back("Habilidade: Invocar Esqueleto (cria aliado)");
    }

    arteClasse = TelaMenuIDE::comprimirArteASCII(arteOriginal, 2, 2);

    std::vector<std::string> quadroAtributos = TelaMenuIDE::comporQuadroDeAtributos(
        statsBase, "ATRIBUTOS BASE", "HABILIDADE UNICA", classeNome,
        infoClasse.size() > 1 ? infoClasse[1] : "");

    for (const auto& linha : quadroAtributos) {
        infoClasse.push_back(linha);
    }

    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("CLASSE", classeNome, infoClasse, arteClasse);
    if (!confirmou) {
        return exibir(nomeJogador, nomeRaca);
    }

    TelaClasse::Resultado r;
    r.indice = selecaoAtual;
    return r;
}