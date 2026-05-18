#include "TelaRacas.h"
#include "TelaRacasLayouts.h"
#include "TelaBestiario.h"
#include "../../Sistemas/SistemaPersonagem.h"
#include "../../Sistemas/SistemaBestiario.h"
#include "../../Utilidades/Aparencia.h"
#include "../../Utilidades/ControleDeInput.h"
#include <iostream>
#include <vector>

void TelaRacas::exibir(SistemaPersonagem* jogador) {
    bool continuar = true;
    
    std::vector<std::string> racasJogaveis = {"Dwarf", "Elfo", "Humano", "Ork"};
    
    do {
        Aparencia::limparTela();
        Aparencia::exibirLogoAscii(ArtesRacas::logoRacas, 66, Cor::AMARELO, "RACAS DO MUNDO", false);
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Selecione uma raca para ver seus detalhes:");
        std::cout << "\n";

        std::vector<std::string> racasMonstros;
        auto inimigos = SistemaBestiario::instancia().obterInimigosOrdenadosPorDificuldade();
        for (const auto& ini : inimigos) {
            if (SistemaBestiario::instancia().estaDescoberto(ini)) {
                racasMonstros.push_back(ini);
            }
        }

        std::vector<std::string> menuOpcoes;
        for (const auto& r : racasJogaveis) menuOpcoes.push_back(r + " (Jogavel)");
        for (const auto& r : racasMonstros) menuOpcoes.push_back(r + " (Monstro)");
        menuOpcoes.push_back("VOLTAR");

        int escolha = ControleDeInput::lerSelecaoMenuComSetas(menuOpcoes, true);

        if (escolha == static_cast<int>(menuOpcoes.size()) - 1) {
            continuar = false;
        } else if (escolha >= 0 && escolha < static_cast<int>(racasJogaveis.size())) {
            exibirFichaRacaJogavel(jogador, racasJogaveis[escolha]);
        } else {
            int indiceMonstro = escolha - static_cast<int>(racasJogaveis.size());
            TelaBestiario::exibirFicha(jogador, racasMonstros[indiceMonstro], true); // true = Exibir com logo das Raças
        }
    } while (continuar);
}

void TelaRacas::exibirFichaRacaJogavel(SistemaPersonagem* jogador, const std::string& nomeRaca) {
    Aparencia::limparTela();
    Aparencia::exibirLogoAscii(ArtesRacas::logoRacas, 66, Cor::AMARELO, "FICHA DA RACA");
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Raca: " + nomeRaca);
    std::cout << "\n\n";
    
    std::vector<std::string> info;
    if (nomeRaca == "Dwarf") {
        info = {
            "Os Dwarfs (Anoes) sao seres robustos e resistentes,",
            "conhecidos por sua forca desproporcional e maestria com a forja.",
            "Eles habitam montanhas e cavernas profundas."
        };
    } else if (nomeRaca == "Elfo") {
        info = {
            "Os Elfos sao agilissimos e profundamente conectados a magia,",
            "capazes de desferir ataques rapidos e mortais.",
            "Geralmente sao encontrados em florestas ancestrais."
        };
    } else if (nomeRaca == "Humano") {
        info = {
            "Os Humanos sao extremamente versateis e adaptaveis.",
            "Embora nao possuam atributos tao extremos quanto outras racas,",
            "sao capazes de dominar qualquer classe ou arma."
        };
    } else if (nomeRaca == "Ork") {
        info = {
            "Os Orks sao criaturas massivas e movidas pelo furor da batalha.",
            "Eles possuem uma vitalidade e forca inigualaveis,",
            "podendo sobreviver a ferimentos que matariam outras especies."
        };
    }
    
    Aparencia::imprimirBlocoCentralizado(info);
    std::cout << "\n";
    ControleDeInput::aguardarEnter();
}
