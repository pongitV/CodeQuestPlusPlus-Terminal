
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaBestiario.h"
#include "../Sistema/FuncionalidadeMenu.h"
#include "../Inimigos/Bestiario.h"
#include "../Sistema/Personagem.h"
#include "../Sistema/SimplificacoesAparencia.h"

static const std::vector<std::string> logoBestiario = {
    " ███████████                    █████     ███                       ███          ",
    "░░███░░░░░███                  ░░███     ░░░                       ░░░           ",
    " ░███    ░███  ██████   █████  ███████   ████   ██████   ████████  ████   ██████ ",
    " ░██████████  ███░░███ ███░░  ░░░███░   ░░███  ░░░░░███ ░░███░░███░░███  ███░░███",
    " ░███░░░░░███░███████ ░░█████   ░███     ░███   ███████  ░███ ░░░  ░███ ░███ ░███",
    " ░███    ░███░███░░░   ░░░░███  ░███ ███ ░███  ███░░███  ░███      ░███ ░███ ░███",
    " ███████████ ░░██████  ██████   ░░█████  █████░░████████ █████     █████░░██████ ",
    "░░░░░░░░░░░   ░░░░░░  ░░░░░░     ░░░░░  ░░░░░  ░░░░░░░░ ░░░░░     ░░░░░  ░░░░░░  "
};

static void exibirCabecalho(int largura, const std::string& tituloSecundario = "") {
    SimplificacoesAparencia::limparTela();
    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoBestiario, 85, SimplificacoesAparencia::cor(Cor::VERDE));
    std::cout << "\n" << std::string(largura, '=') << "\n";
    if (!tituloSecundario.empty()) {
        SimplificacoesAparencia::imprimirCentralizado(tituloSecundario);
        std::cout << std::string(largura, '=') << "\n\n";
    } else {
        std::cout << "\n";
    }
}

void TelaBestiario::exibirLista(Personagem* jogadorAtual) {
    if (jogadorAtual == nullptr) return;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    int largura = SimplificacoesAparencia::obterLarguraTerminal();
    Bestiario& bestiario = Bestiario::instancia();
    const auto& inimigos = bestiario.obterInimigosOrdenadosPorDificuldade();

    std::vector<std::string> descobertos;
    std::copy_if(inimigos.begin(), inimigos.end(), std::back_inserter(descobertos),
                 [&](const std::string& nome) { return bestiario.estaDescoberto(nome); });

    if (descobertos.empty()) {
        exibirCabecalho(largura);
        SimplificacoesAparencia::imprimirCentralizado("Nenhum inimigo descoberto ainda.");
        SimplificacoesAparencia::imprimirCentralizado("Explore e combata para desbloquear entries.");
        std::cout << "\n";
        SimplificacoesAparencia::imprimirCentralizado("[0] Voltar");
        std::cout << "\nEscolha: ";
        std::string escolha;
        std::cin >> escolha;
        return;
    }

    constexpr int maxDisplay = 10;
    int totalDescobertos = static_cast<int>(descobertos.size());
    int totalPages = std::max(1, (totalDescobertos + maxDisplay - 1) / maxDisplay);
    int currentPage = 0;

    while (true) {
        exibirCabecalho(largura);

        SimplificacoesAparencia::imprimirCentralizado("Encontrados: " + std::to_string(totalDescobertos) + "/" + std::to_string(inimigos.size()));
        std::cout << "\n";

        int inicio = currentPage * maxDisplay;
        int fim = std::min(inicio + maxDisplay, totalDescobertos);

        SimplificacoesAparencia::imprimirCentralizado("--- INIMIGOS ---");
        std::cout << "\n";

        for (int i = inicio; i < fim; ++i) {
            const std::string& nomeSelecionado = descobertos[i];
            const BestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
            if (!info) continue;

            std::string linhaInimigo = "[" + std::to_string(i + 1) + "] " + info->nome + " (" + info->mapa + " - " + info->habitat + ")";
            SimplificacoesAparencia::imprimirCentralizado(linhaInimigo);
        }

        std::cout << "\n";
        if (totalPages > 1) {
            SimplificacoesAparencia::imprimirCentralizado("[P] Pagina " + std::to_string(currentPage + 1) + "/" + std::to_string(totalPages), SimplificacoesAparencia::cor(Cor::CIANO));
            std::cout << "\n";
        }

        std::cout << "Escolha um numero (1-" << fim << "), [P] pagina, [0] Voltar: ";
        std::string escolhaStr;
        std::cin >> escolhaStr;

        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Entrada invalida. Escolha um numero (1-" << fim << "), [P] pagina, [0] Voltar: ";
            std::cin >> escolhaStr;
        }

        if (escolhaStr == "p" || escolhaStr == "P") {
            currentPage = (currentPage + 1) % totalPages;
            continue;
        } else if (escolhaStr == "0") {
            return;
        }

        try {
            int escolha = std::stoi(escolhaStr);
            if (escolha >= 1 && escolha <= fim) {
                exibirFicha(jogadorAtual, descobertos[escolha - 1], escolha - 1, descobertos);
            }
        } catch (const std::invalid_argument&) {
            // Ignora conversao falha, roda loop novamente
        } catch (const std::out_of_range&) {
            // Ignora overflow
        }
    }
}

void TelaBestiario::exibirFicha(Personagem* jogadorAtual, const std::string& nomeSelecionado, int /*indiceDescoberto*/, const std::vector<std::string>& /*descobertos*/) {
    if (jogadorAtual == nullptr) return;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    int largura = SimplificacoesAparencia::obterLarguraTerminal();
    Bestiario& bestiario = Bestiario::instancia();

    const BestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
    if (!info) return;

    bool visto = bestiario.estaDescoberto(nomeSelecionado);
    bool derrotado = bestiario.jaDerrotado(nomeSelecionado);

    auto imprimirSecaoBase = [largura](const std::string& titulo, bool exibirConteudo, const std::function<void()>& conteudoFnc, const std::string& textoOculto) {
        std::cout << "=== " << titulo << " ===\n\n";
        if (exibirConteudo) {
            conteudoFnc();
        } else {
            SimplificacoesAparencia::imprimirCentralizado("???", SimplificacoesAparencia::cor(Cor::CINZA));
            SimplificacoesAparencia::imprimirCentralizado(textoOculto, SimplificacoesAparencia::cor(Cor::CINZA));
        }
        std::cout << "\n" << std::string(largura, '-') << "\n\n";
    };

    while (true) {
        exibirCabecalho(largura, "FICHA DO INIMIGO");

        // Aparencia
        imprimirSecaoBase("APARENCIA", visto, [&]() {
            SimplificacoesAparencia::imprimirCentralizado(info->nome);
            std::cout << "\n";
            SimplificacoesAparencia::imprimirCentralizadoMultilinha(info->aparencia);
            std::cout << "\n";
            SimplificacoesAparencia::imprimirCentralizado("Lore: " + info->lore);
            std::cout << "\n";
            SimplificacoesAparencia::imprimirCentralizado("Fato Curioso: " + info->fatoCurioso);
        }, "(Nunca viu este inimigo em combate)");

        // Atributos
        imprimirSecaoBase("ATRIBUTOS", derrotado, [&]() {
            SimplificacoesAparencia::imprimirCentralizadoMultilinha(info->atributosTexto);
        }, "(Derrote o inimigo para descobrir os atributos)");

        // Habilidades
        std::cout << "=== HABILIDADES ===\n\n";
        
        auto processarHabilidade = [&](const std::string& hab, bool checarDescoberta) {
            size_t pos = hab.find(" |");
            std::string nomeHab = (pos != std::string::npos) ? hab.substr(0, pos) : hab;

            if (!checarDescoberta || bestiario.jaViuHabilidade(nomeSelecionado, nomeHab)) {
                SimplificacoesAparencia::imprimirCentralizado("  - " + hab);
                return true;
            }
            return false;
        };

        std::cout << "Ativas:\n";
        if (info->habilidadesAtivas.empty() || info->habilidadesAtivas[0].find("Nenhuma") != std::string::npos) {
            SimplificacoesAparencia::imprimirCentralizado("  Nenhuma", SimplificacoesAparencia::cor(Cor::CINZA));
        } else {
            bool temAtivas = false;
            for (const auto& hab : info->habilidadesAtivas) {
                if (processarHabilidade(hab, true)) temAtivas = true;
            }
            if (!temAtivas) {
                SimplificacoesAparencia::imprimirCentralizado("  ???", SimplificacoesAparencia::cor(Cor::CINZA));
                SimplificacoesAparencia::imprimirCentralizado("(Deixe uma habilidade ativa te acertar)", SimplificacoesAparencia::cor(Cor::CINZA));
            }
        }
        std::cout << "\n";

        std::cout << "Passivas:\n";
        if (info->habilidadePassiva.empty() || info->habilidadePassiva.find("Nenhuma") != std::string::npos) {
            SimplificacoesAparencia::imprimirCentralizado("  Nenhuma", SimplificacoesAparencia::cor(Cor::CINZA));
        } else {
            bool temPassivas = derrotado && processarHabilidade(info->habilidadePassiva, false);
            if (!temPassivas) {
                SimplificacoesAparencia::imprimirCentralizado("  ???", SimplificacoesAparencia::cor(Cor::CINZA));
                SimplificacoesAparencia::imprimirCentralizado("(Derrote o inimigo para descobrir as passivas)", SimplificacoesAparencia::cor(Cor::CINZA));
            }
        }
        std::cout << "\n" << std::string(largura, '-') << "\n\n";

        // Drops
        imprimirSecaoBase("DROPS", true, [&]() {
            bool temDrops = false;
            for (const auto& drop : info->drops) {
                if (bestiario.jaColetouDrop(nomeSelecionado, drop)) {
                    SimplificacoesAparencia::imprimirCentralizado("  - " + drop);
                    temDrops = true;
                }
            }
            if (!temDrops) {
                SimplificacoesAparencia::imprimirCentralizado("  ???", SimplificacoesAparencia::cor(Cor::CINZA));
                SimplificacoesAparencia::imprimirCentralizado("(Colete drops para desbloquear)", SimplificacoesAparencia::cor(Cor::CINZA));
            }
        }, "");

        SimplificacoesAparencia::imprimirCentralizado("[0] Retornar a lista");
        std::cout << "\nEscolha: ";
        std::string escolha;
        std::cin >> escolha;
        if (escolha == "0") break;
    }
}
