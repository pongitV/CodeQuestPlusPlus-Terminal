#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaAtributos.h"
#include "../Sistema/Menu.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;
    Menu::limparTelaDoTerminal();
    
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif

    int largura = Menu::obterLarguraDoTerminalEmColunas();
    std::vector<std::string> logoFicha = 
    {
       "███████████  █████   █████████  █████   █████   █████████   ",
       "░░███░░░░░█ ░░███   ███░░░░░███░░███   ░░███   ███░░░░░███  ",
       " ░███   █ ░  ░███  ███     ░░░  ░███    ░███  ░███    ░███  ",
       " ░███████    ░███ ░███          ░███████████  ░███████████  ",
       " ░███░░░█    ░███ ░███          ░███░░░░░███  ░███░░░░░███  ",
       " ░███  ░     ░███ ░░███     ███ ░███    ░███  ░███    ░███  ",
       " █████       █████ ░░█████████  █████   █████ █████   █████ ",
       " ░░░░░       ░░░░░   ░░░░░░░░░  ░░░░░   ░░░░░ ░░░░░   ░░░░░ "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    Menu::imprimirLinhasCentralizadasNaTela(logoFicha, 59, "\033[34m"); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
    int quantidadeDeTurnosRestantesDoBuff = jogadorAtual->obterTurnosBuff();

    std::vector<std::string> linhas = {
        "NOME:           " + jogadorAtual->obterNome(), "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(), "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()), "XP:             " + std::to_string(jogadorAtual->obterXpAtual()) + " / " + std::to_string(jogadorAtual->obterXpParaSubir()),
        "DIFICULDADE:    " + std::string(jogadorAtual->obterDificuldade() == 1 ? "Facil" : (jogadorAtual->obterDificuldade() == 2 ? "Normal" : "Dificil")),
        "[PARRY]:        " + std::string(jogadorAtual->obterParryAtivado() ? "Ligado" : "Desligado"), "OURO:           " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G", "",
        "PASSIVA RACA:   " + jogadorAtual->obterRaca()->obterNomeHabilidadeRaca(), "-> " + jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca(), "",
        "ATIVA CLASSE:   " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse(), "-> " + jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse(), "",
        "ATRIBUTOS TOTAIS:", "- HP: " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + " (0)"
    };

    auto addAtributo = [&](std::string nome, int valorBase) 
    {
        std::string linha = "- " + nome + ": " + std::to_string(valorBase);
        if (quantidadeDeTurnosRestantesDoBuff > 0) {
            std::string sm = std::to_string(multiplicadorDeAtributosAtual); sm.erase(sm.find_last_not_of('0') + 1, std::string::npos); if (sm.back() == '.') sm += "0";
            linha += " (" + std::to_string(static_cast<int>(valorBase * multiplicadorDeAtributosAtual)) + "){x" + sm + "} por " + std::to_string(quantidadeDeTurnosRestantesDoBuff) + " turnos";
        } else linha += " (0)";
        linhas.push_back(linha);
    };

    std::string nomeDaClasse = jogadorAtual->obterNomeClasse();
    addAtributo(nomeDaClasse == "Guerreiro" ? "Forca [DANO]" : "Forca", jogadorAtual->obterForca()); addAtributo(nomeDaClasse == "Arqueiro" ? "Destreza [DANO]" : "Destreza", jogadorAtual->obterDestreza());
    addAtributo("Resistencia", jogadorAtual->obterResistencia()); addAtributo("Constituicao", jogadorAtual->obterConstituicao());
    addAtributo(nomeDaClasse == "Mago" ? "Inteligencia [DANO]" : "Inteligencia", jogadorAtual->obterInteligencia()); addAtributo(nomeDaClasse == "Bardo" ? "Sabedoria [DANO]" : "Sabedoria", jogadorAtual->obterSabedoria());
    
    Menu::imprimirLinhasCentralizadasNaTela(linhas, 0, "");
    std::cout << "\n" << std::string(largura, '=') << "\n";
}