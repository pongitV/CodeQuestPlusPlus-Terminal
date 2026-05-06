
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "TelaBestiario.h"
#include "../Sistemas/SistemaBestiario.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"

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
    Aparencia::limparTela();
    Aparencia::exibirLogoAscii(logoBestiario, 101, Cor::VERDE, tituloSecundario);
}

void TelaBestiario::exibirLista(SistemaPersonagem* jogadorAtual) {
    if (jogadorAtual == nullptr) return;

    int largura = Aparencia::obterLarguraTerminal();
    SistemaBestiario& bestiario = SistemaBestiario::instancia();
    const auto& inimigos = bestiario.obterInimigosOrdenadosPorDificuldade();

    std::vector<std::string> descobertos;
    std::copy_if(inimigos.begin(), inimigos.end(), std::back_inserter(descobertos),
                 [&](const std::string& nome) { return bestiario.estaDescoberto(nome); });

    if (descobertos.empty()) {
        exibirCabecalho(largura);
        Aparencia::imprimirCentralizado("Nenhum inimigo descoberto ainda.");
        Aparencia::imprimirCentralizado("Explore e combata para desbloquear entries.");
        std::cout << "\n";
        Aparencia::exibirPrompt("[0] Voltar\n\nEscolha: ");
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

        Aparencia::imprimirCentralizado("Encontrados: " + std::to_string(totalDescobertos) + "/" + std::to_string(inimigos.size()));
        std::cout << "\n";

        int indiceInicial = paginaAtual * quantidadeMaximaPorPagina;
        int indiceFinal = std::min(indiceInicial + quantidadeMaximaPorPagina, totalDescobertos);

        Aparencia::imprimirCentralizado("--- INIMIGOS ---");
        std::cout << "\n";

        for (int i = indiceInicial; i < indiceFinal; ++i) {
            const std::string& nomeSelecionado = descobertos[i];
            const SistemaBestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
            if (!info) continue;

            std::string linhaInimigo = "[" + std::to_string(i + 1) + "] " + info->nome + " (" + info->mapa + " - " + info->habitat + ")";
            Aparencia::imprimirCentralizado(linhaInimigo);
        }

        std::cout << "\n";
        if (totalDePaginas > 1) {
            Aparencia::imprimirCentralizado("[P] Pagina " + std::to_string(paginaAtual + 1) + "/" + std::to_string(totalDePaginas), Aparencia::cor(Cor::CIANO));
            std::cout << "\n";
        }

        Aparencia::exibirPrompt("Escolha um numero (1-" + std::to_string(indiceFinal) + "), [P] pagina, [0] Voltar: ");
        std::string entradaDigitadaPeloJogador;
        std::cin >> entradaDigitadaPeloJogador;

        while (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            Aparencia::exibirPrompt("Entrada invalida. Escolha um numero (1-" + std::to_string(indiceFinal) + "), [P] pagina, [0] Voltar: ");
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

    int largura = Aparencia::obterLarguraTerminal();
    SistemaBestiario& bestiario = SistemaBestiario::instancia();

    const SistemaBestiarioEnemyInfo* info = bestiario.obterInfo(nomeSelecionado);
    if (!info) return;

    bool visto = bestiario.estaDescoberto(nomeSelecionado);
    bool derrotado = bestiario.jaDerrotado(nomeSelecionado);

    auto imprimirSecaoBase = [largura](const std::string& tituloDaSecao, bool deveExibirConteudo, const std::function<void()>& funcaoParaExibirConteudo, const std::string& textoCasoOculto) {
        Aparencia::imprimirCentralizado("=== " + tituloDaSecao + " ===");
        std::cout << "\n";
        if (deveExibirConteudo) {
            funcaoParaExibirConteudo();
        } else {
            Aparencia::imprimirCentralizado("???", Aparencia::cor(Cor::CINZA));
            Aparencia::imprimirCentralizado(textoCasoOculto, Aparencia::cor(Cor::CINZA));
        }
        std::cout << "\n";
        Aparencia::imprimirLinhaDivisoria('-');
        std::cout << "\n";
    };

    while (true) {
        exibirCabecalho(largura, "FICHA DO INIMIGO");

        // Aparencia
        imprimirSecaoBase("APARENCIA", visto, [&]() {
            Aparencia::imprimirCentralizado(info->nome);
            std::cout << "\n";
            Aparencia::imprimirCentralizadoMultilinha(info->aparencia);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Lore: " + info->lore);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Fato Curioso: " + info->fatoCurioso);
        }, "(Nunca viu este inimigo em combate)");

        // Atributos
        imprimirSecaoBase("ATRIBUTOS", derrotado, [&]() {
            Aparencia::imprimirBlocoCentralizado(info->atributosTexto);
        }, "(Derrote o inimigo para descobrir os atributos)");

        // Habilidades
        Aparencia::imprimirCentralizado("=== HABILIDADES ===");
        std::cout << "\n";
        
        std::vector<std::string> blocoHabilidades;

        auto processarHabilidade = [&](const std::string& hab, bool checarDescoberta) {
            size_t pos = hab.find(" |");
            std::string nomeHab = (pos != std::string::npos) ? hab.substr(0, pos) : hab;

            if (!checarDescoberta || bestiario.jaViuHabilidade(nomeSelecionado, nomeHab)) {
                blocoHabilidades.push_back("  - " + hab);
                return true;
            }
            return false;
        };

        blocoHabilidades.push_back("Ativas:");
        if (info->habilidadesAtivas.empty() || info->habilidadesAtivas[0].find("Nenhuma") != std::string::npos) {
            blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  Nenhuma" + Aparencia::cor(Cor::RESET));
        } else {
            bool temAtivas = false;
            for (const auto& hab : info->habilidadesAtivas) {
                if (processarHabilidade(hab, true)) temAtivas = true;
            }
            if (!temAtivas) {
                blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  ???" + Aparencia::cor(Cor::RESET));
                blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  (Deixe uma habilidade ativa te acertar)" + Aparencia::cor(Cor::RESET));
            }
        }
        blocoHabilidades.push_back("");

        blocoHabilidades.push_back("Passivas:");
        if (info->habilidadePassiva.empty() || info->habilidadePassiva.find("Nenhuma") != std::string::npos) {
            blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  Nenhuma" + Aparencia::cor(Cor::RESET));
        } else {
            bool temPassivas = derrotado && processarHabilidade(info->habilidadePassiva, false);
            if (!temPassivas) {
                blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  ???" + Aparencia::cor(Cor::RESET));
                blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  (Derrote o inimigo para descobrir as passivas)" + Aparencia::cor(Cor::RESET));
            }
        }
        
        Aparencia::imprimirBlocoCentralizado(blocoHabilidades);
        std::cout << "\n";
        Aparencia::imprimirLinhaDivisoria('-');
        std::cout << "\n";

        // Drops
        imprimirSecaoBase("DROPS", true, [&]() {
            bool temDrops = false;
            std::vector<std::string> blocoDrops;
            for (const auto& drop : info->drops) {
                if (bestiario.jaColetouDrop(nomeSelecionado, drop)) {
                    blocoDrops.push_back("  - " + drop);
                    temDrops = true;
                }
            }
            if (!temDrops) {
                blocoDrops.push_back(Aparencia::cor(Cor::CINZA) + "  ???" + Aparencia::cor(Cor::RESET));
                blocoDrops.push_back(Aparencia::cor(Cor::CINZA) + "  (Colete drops para desbloquear)" + Aparencia::cor(Cor::RESET));
            }
            Aparencia::imprimirBlocoCentralizado(blocoDrops);
        }, "");

        Aparencia::exibirPrompt("[0] Retornar a lista\n\nEscolha: ");
        std::string escolha;
        std::cin >> escolha;
        if (escolha == "0") break;
    }
}
