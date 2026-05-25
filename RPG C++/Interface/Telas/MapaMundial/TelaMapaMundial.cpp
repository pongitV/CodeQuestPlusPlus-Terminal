#include "TelaMapaMundial.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Mundo/MapaInteracao.h"
#include "../TelaBase.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"

ProximaTransicaoMapa TelaMapaMundial::exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoReino) {
    static const std::vector<std::string> arteMapa = {
        "   ╔═══════════════════════════════════════════════╗   ",
        "   ║                                               ║   ",
        "   ║                            ╔══════╗           ║   ",
        "   ║                            ║ VILA ║ [V]%      ║   ",
        "   ║                            ╚══════╝           ║   ",
        "   ║                               ║║              ║   ",
        "   ║                          ╔══════════╗         ║   ",
        "   ║                          ║ FLORESTA ║ [F]%    ║   ",
        "   ║                          ╚══════════╝         ║   ",
        "   ║                               ║║              ║   ",
        "   ║               ╔═══════╗       ║║              ║   ",
        "   ║               ║ REINO ╠═══════╣║ [R]%         ║   ",
        "   ║               ╚═══════╝                       ║   ",
        "   ║                                               ║   ",
        "   ╚═══════════════════════════════════════════════╝   "
    };

    ProximaTransicaoMapa destinoEscolhido = ProximaTransicaoMapa::Nenhuma;
    std::string msgExtra = "";

    TelaBase::executarLoopPadrao(
        "MAPA MUNDIAL - VIAGEM RAPIDA",
        Cor::BRANCO,
        // render content
        [&]() {
            int larguraTerminal = Aparencia::obterLarguraTerminal();
            int larguraMapa = Aparencia::obterComprimentoVisual(arteMapa[0]);
            int espacos = std::max(0, (larguraTerminal - larguraMapa) / 2);
            std::string margem(espacos, ' ');

            std::cout << "\n";
            for (int y = 0; y < static_cast<int>(arteMapa.size()); ++y) {
                std::string linha = arteMapa[y];

                auto formatarProgresso = [](std::string& str, const std::string& placeholder, int valor) {
                    size_t pos = str.find(placeholder);
                    if (pos != std::string::npos) {
                        std::string p = std::to_string(valor);
                        if (p.length() < 3) p = std::string(3 - p.length(), ' ') + p;
                        std::string cor = (valor >= 100) ? Aparencia::cor(Cor::CIANO) : (valor > 0) ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA);
                        str.replace(pos, placeholder.length(), cor + p + Aparencia::cor(Cor::RESET));
                    }
                };

                formatarProgresso(linha, "[V]", progressoVila);
                formatarProgresso(linha, "[F]", progressoFloresta);
                formatarProgresso(linha, "[R]", progressoReino);

                std::string palavra;
                if (localAtual == LocalizacaoMapa::VilaInicial) palavra = "VILA";
                else if (localAtual == LocalizacaoMapa::Floresta) palavra = "FLORESTA";
                else if (localAtual == LocalizacaoMapa::Reino) palavra = "REINO";
                
                size_t pos = linha.find(palavra);
                if (pos != std::string::npos) {
                    linha.replace(pos, palavra.length(), Aparencia::cor(Cor::VERDE) + palavra + Aparencia::cor(Cor::RESET));
                }
                
                std::cout << margem << linha << "\n";
            }
            std::cout << "\n";
            if (!msgExtra.empty()) {
                Aparencia::imprimirCentralizado(msgExtra, Aparencia::cor(Cor::AMARELO));
                msgExtra = "";
            } else {
                Aparencia::imprimirCentralizado("Selecione um destino para Viagem Rapida.");
            }
            std::cout << "\n";
        },
        // build menu
        [&]() {
            std::vector<std::string> opcoes;
            opcoes.push_back("Vila Inicial");
            if (Progressao::instancia().obterFlag(Flags::Visitou_Floresta)) opcoes.push_back("Floresta");
            if (Progressao::instancia().obterFlag(Flags::Visitou_Reino)) opcoes.push_back("Reino");
            opcoes.push_back("Cancelar Viagem");
            return opcoes;
        },
        // process choice
        [&](int id) {
            std::vector<std::pair<std::string, ProximaTransicaoMapa>> destinos;
            destinos.push_back({"Vila Inicial", ProximaTransicaoMapa::Vila});
            if (Progressao::instancia().obterFlag(Flags::Visitou_Floresta)) destinos.push_back({"Floresta", ProximaTransicaoMapa::Floresta});
            if (Progressao::instancia().obterFlag(Flags::Visitou_Reino)) destinos.push_back({"Reino", ProximaTransicaoMapa::Reino});

            if (id == -1 || id >= static_cast<int>(destinos.size())) {
                destinoEscolhido = ProximaTransicaoMapa::Nenhuma;
                return false;
            }

            ProximaTransicaoMapa destinoCandidato = destinos[id].second;
            LocalizacaoMapa locCandidata = (destinoCandidato == ProximaTransicaoMapa::Vila) ? LocalizacaoMapa::VilaInicial : (destinoCandidato == ProximaTransicaoMapa::Floresta) ? LocalizacaoMapa::Floresta : LocalizacaoMapa::Reino;

            if (locCandidata == localAtual) {
                msgExtra = "Voce ja esta neste local!";
                return true;
            }
            destinoEscolhido = destinoCandidato;
            return false;
        }
    );

    return destinoEscolhido;
}
