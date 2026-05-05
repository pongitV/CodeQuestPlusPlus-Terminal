
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "TelaBestiario.h"
#include "../Sistemas/SistemaBestiario.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"

static const std::vector<std::string> logoBestiario = {
    " ███████████  ██████████  █████████  ███████████ █████   █████████   ███████████   █████    ███████   ",
    "░░███░░░░░███░░███░░░░░█ ███░░░░░███░█░░░███░░░█░░███   ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███ ",
    " ░███    ░███ ░███  █ ░ ░███    ░░░ ░   ░███  ░  ░███  ░███    ░███  ░███    ░███  ░███  ███     ░░███",
    " ░██████████  ░██████   ░░█████████     ░███     ░███  ░███████████  ░██████████   ░███ ░███      ░███",
    " ░███░░░░░███ ░███░░█    ░░░░░░░░███    ░███     ░███  ░███░░░░░███  ░███░░░░░███  ░███ ░███      ░███",
    " ░███    ░███ ░███ ░   █ ███    ░███    ░███     ░███  ░███    ░███  ░███    ░███  ░███ ░░███     ███ ",
    " ███████████  ██████████░░█████████     █████    █████ █████   █████ █████   █████ █████ ░░░███████░  ",
    "░░░░░░░░░░░  ░░░░░░░░░░  ░░░░░░░░░     ░░░░░    ░░░░░ ░░░░░   ░░░░░ ░░░░░   ░░░░░ ░░░░░    ░░░░░░░   "
};

static void exibirCabecalho(int largura, const std::string& tituloSecundario = "") {
    SimplificacoesAparencia::limparTela();
    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoBestiario, 101, SimplificacoesAparencia::cor(Cor::VERDE));
    std::cout << "\n" << std::string(largura, '=') << "\n";
    if (!tituloSecundario.empty()) {
        SimplificacoesAparencia::imprimirCentralizado(tituloSecundario);
        std::cout << std::string(largura, '=') << "\n\n";
    } else {
        std::cout << "\n";
    }
}

void TelaBestiario::exibirLista(SistemaPersonagem* jogadorAtual) {
    if (jogadorAtual == nullptr) return;

    int largura = SimplificacoesAparencia::obterLarguraTerminal();
    SistemaBestiario& bestiario = SistemaBestiario::instancia();
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

    constexpr int quantidadeMaximaPorPagina = 10;
    int totalDescobertos = static_cast<int>(descobertos.size());
    int totalDePaginas = std::max(1, (totalDescobertos + quantidadeMaximaPorPagina - 1) / quantidadeMaximaPorPagina);
    int paginaAtual = 0;

    while (true) {
        exibirCabecalho(largura);

        SimplificacoesAparencia::imprimirCentralizado("Encontrados: " + std::to_string(totalDescobertos) + "/" + std::to_string(inimigos.size()));
        std::cout << "\n";

        int indiceInicial = paginaAtual * quantidadeMaximaPorPagina;
        int indiceFinal = std::min(indiceInicial + quantidadeMaximaPorPagina, totalDescobertos);

        SimplificacoesAparencia::imprimirCentralizado("--- INIMIGOS ---");
        std::cout << "\n";

        for (int i = indiceInicial; i < indiceFinal; ++i) {
            const std::string& nomeSelecionado = descobertos[i];
            const SistemaBestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
            if (!info) continue;

            std::string linhaInimigo = "[" + std::to_string(i + 1) + "] " + info->nome + " (" + info->mapa + " - " + info->habitat + ")";
            SimplificacoesAparencia::imprimirCentralizado(linhaInimigo);
        }

        std::cout << "\n";
        if (totalDePaginas > 1) {
            SimplificacoesAparencia::imprimirCentralizado("[P] Pagina " + std::to_string(paginaAtual + 1) + "/" + std::to_string(totalDePaginas), SimplificacoesAparencia::cor(Cor::CIANO));
            std::cout << "\n";
        }

        std::cout << "Escolha um numero (1-" << indiceFinal << "), [P] pagina, [0] Voltar: ";
        std::string entradaDigitadaPeloJogador;
        std::cin >> entradaDigitadaPeloJogador;

        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Entrada invalida. Escolha um numero (1-" << indiceFinal << "), [P] pagina, [0] Voltar: ";
            std::cin >> entradaDigitadaPeloJogador;
        }

        if (entradaDigitadaPeloJogador == "p" || entradaDigitadaPeloJogador == "P") {
            paginaAtual = (paginaAtual + 1) % totalDePaginas;
            continue;
        } else if (entradaDigitadaPeloJogador == "0") {
            return;
        }

        try {
            int escolha = std::stoi(entradaDigitadaPeloJogador);
            if (escolha >= 1 && escolha <= indiceFinal) {
                exibirFicha(jogadorAtual, descobertos[escolha - 1], escolha - 1, descobertos);
            }
        } catch (const std::invalid_argument&) {
            // Ignora conversao falha, roda loop novamente
        } catch (const std::out_of_range&) {
            // Ignora overflow
        }
    }
}

void TelaBestiario::exibirFicha(SistemaPersonagem* jogadorAtual, const std::string& nomeSelecionado, int /*indiceDescoberto*/, const std::vector<std::string>& /*descobertos*/) {
    if (jogadorAtual == nullptr) return;

    int largura = SimplificacoesAparencia::obterLarguraTerminal();
    SistemaBestiario& bestiario = SistemaBestiario::instancia();

    const SistemaBestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
    if (!info) return;

    bool visto = bestiario.estaDescoberto(nomeSelecionado);
    bool derrotado = bestiario.jaDerrotado(nomeSelecionado);

    auto imprimirSecaoBase = [largura](const std::string& tituloDaSecao, bool deveExibirConteudo, const std::function<void()>& funcaoParaExibirConteudo, const std::string& textoCasoOculto) {
        std::cout << "=== " << tituloDaSecao << " ===\n\n";
        if (deveExibirConteudo) {
            funcaoParaExibirConteudo();
        } else {
            SimplificacoesAparencia::imprimirCentralizado("???", SimplificacoesAparencia::cor(Cor::CINZA));
            SimplificacoesAparencia::imprimirCentralizado(textoCasoOculto, SimplificacoesAparencia::cor(Cor::CINZA));
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
