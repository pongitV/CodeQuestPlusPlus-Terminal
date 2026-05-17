
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
#include "../Utilidades/ControleDeInput.h"
#include "TelaBase.h"
#include "TelaBestiarioLayouts.h"

static void exibirCabecalho(int largura, const std::string& tituloSecundario = "", bool animar = false) {
    Aparencia::limparTela();
    Aparencia::exibirLogoAscii(ArtesBestiario::logoBestiario, 101, Cor::VERDE, tituloSecundario, animar);
}

void TelaBestiario::exibirLista(SistemaPersonagem* jogadorAtual) {
    if (jogadorAtual == nullptr) return;

    int largura = Aparencia::obterLarguraTerminal();
    SistemaBestiario& bestiario = SistemaBestiario::instancia();
    const auto& inimigos = bestiario.obterInimigosOrdenadosPorDificuldade();

    std::vector<std::string> descobertos;
    std::copy_if(inimigos.begin(), inimigos.end(), std::back_inserter(descobertos),
                 [&](const std::string& nome) { return bestiario.estaDescoberto(nome); });

    static auto ultimoAcesso = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animarEntrada = TelaBase::deveAnimarEntradaDaTela(ultimoAcesso, 300);

    if (descobertos.empty()) {
        exibirCabecalho(largura, "", animarEntrada);
        Aparencia::imprimirCentralizado("Nenhum inimigo descoberto ainda.");
        Aparencia::imprimirCentralizado("Explore e combata para desbloquear entries.");
        std::cout << "\n";
        Aparencia::exibirPrompt("[0] Voltar\n\nEscolha: ");
        std::string escolha = ControleDeInput::lerEntradaProtegida();
        return;
    }

    constexpr int quantidadeMaximaPorPagina = 10;
    int totalDescobertos = static_cast<int>(descobertos.size());
    int totalDePaginas = std::max(1, (totalDescobertos + quantidadeMaximaPorPagina - 1) / quantidadeMaximaPorPagina);
    int paginaAtual = 0;

    while (true) {
        exibirCabecalho(largura, "", animarEntrada);
        animarEntrada = false;

        Aparencia::imprimirCentralizado("Encontrados: " + std::to_string(totalDescobertos) + "/" + std::to_string(inimigos.size()));
        std::cout << "\n";

        int indiceInicial = paginaAtual * quantidadeMaximaPorPagina;
        int indiceFinal = std::min(indiceInicial + quantidadeMaximaPorPagina, totalDescobertos);

        Aparencia::imprimirCentralizado("═══ INIMIGOS ═══");
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
        std::string entradaDigitadaPeloJogador = ControleDeInput::lerEntradaProtegida();

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
    int vezesDerrotado = bestiario.obterQuantidadeDerrotas(nomeSelecionado); // Voce precisara implementar este metodo em SistemaBestiario!

    auto imprimirSecaoBase = [largura](const std::string& tituloDaSecao, bool deveExibirConteudo, const std::function<void()>& funcaoParaExibirConteudo, const std::string& textoCasoOculto) {
        Aparencia::imprimirCentralizado("═══ " + tituloDaSecao + " ═══");
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
            if (vezesDerrotado > 0) {
                Aparencia::imprimirCentralizado("Derrotado: " + std::to_string(vezesDerrotado) + " vezes", Aparencia::cor(Cor::AMARELO));
            }
            std::cout << "\n";
            Aparencia::imprimirCentralizadoMultilinha(info->aparencia);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Lore: " + info->lore);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("Fato Curioso: " + info->fatoCurioso);
        }, "(Nunca viu este inimigo em combate)");

        // Atributos
        imprimirSecaoBase("ATRIBUTOS", derrotado, [&]() {
            std::vector<std::string> atributosLimpos = info->atributosTexto;
            for (auto& linha : atributosLimpos) {
                size_t pos = 0;
                while ((pos = linha.find('|', pos)) != std::string::npos) {
                    linha.replace(pos, 1, "║");
                    pos += 3;
                }
            }
            Aparencia::imprimirBlocoCentralizado(atributosLimpos);
        }, "(Derrote o inimigo para descobrir os atributos)");

        // Habilidades
        Aparencia::imprimirCentralizado("═══ HABILIDADES ═══");
        std::cout << "\n";
        
        std::vector<std::string> blocoHabilidades;

        auto processarHabilidade = [&](const std::string& hab) {
            std::string habLimpa = hab;
            size_t posPipe = 0;
            while ((posPipe = habLimpa.find('|', posPipe)) != std::string::npos) {
                habLimpa.replace(posPipe, 1, "║");
                posPipe += 3;
            }
            blocoHabilidades.push_back("  - " + habLimpa);
        };

        blocoHabilidades.push_back("Ativas:");
        if (info->habilidadesAtivas.empty() || info->habilidadesAtivas[0].find("Nenhuma") != std::string::npos) {
            blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  Nenhuma" + Aparencia::cor(Cor::RESET));
        } else {
            for (const auto& hab : info->habilidadesAtivas) {
                processarHabilidade(hab);
            }
        }
        blocoHabilidades.push_back("");

        blocoHabilidades.push_back("Passivas:");
        if (info->habilidadePassiva.empty() || info->habilidadePassiva.find("Nenhuma") != std::string::npos) {
            blocoHabilidades.push_back(Aparencia::cor(Cor::CINZA) + "  Nenhuma" + Aparencia::cor(Cor::RESET));
        } else {
            processarHabilidade(info->habilidadePassiva);
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
        std::string escolha = ControleDeInput::lerEntradaProtegida();
        if (escolha == "0") break;
    }
}
