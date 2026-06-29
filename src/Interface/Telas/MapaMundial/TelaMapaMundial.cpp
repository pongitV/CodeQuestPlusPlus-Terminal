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

ProximaTransicaoMapa TelaMapaMundial::exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino) {
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
        "   ║               ║ PonteReino ╠═══════╣║ [R]%         ║   ",
        "   ║               ╚═══╦═══╝                       ║   ",
        "   ║                   ║║                          ║   ",
        "   ║               ╔═══╩═════╗                     ║   ",
        "   ║               ║ Reino ║ [C]%                ║   ",
        "   ║               ╚═════════╝                     ║   ",
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

                bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);

                auto formatarProgresso = [](std::string& str, const std::string& placeholder, int valor, bool descoberto) {
                    size_t pos = str.find(placeholder + "%");
                    if (pos != std::string::npos) {
                        if (!descoberto) {
                            str.replace(pos, placeholder.length() + 1, Aparencia::cor(Cor::CINZA) + "??? " + Aparencia::cor(Cor::RESET));
                        } else {
                            std::string p = std::to_string(valor);
                            if (p.length() < 3) p = std::string(3 - p.length(), ' ') + p;
                            std::string cor = (valor >= 100) ? Aparencia::cor(Cor::CIANO) : (valor > 0) ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::CINZA);
                            str.replace(pos, placeholder.length() + 1, cor + p + "%" + Aparencia::cor(Cor::RESET));
                        }
                    }
                };

                formatarProgresso(linha, "[V]", progressoVila, true); // Vila sempre fica descoberta
                formatarProgresso(linha, "[F]", progressoFloresta, mapasDescobertos);
                formatarProgresso(linha, "[R]", progressoPonteReino, mapasDescobertos);
                formatarProgresso(linha, "[C]", progressoReino, mapasDescobertos);

                std::string palavra = "";
                if (localAtual == LocalizacaoMapa::VilaInicial) palavra = "VILA";
                else if (localAtual == LocalizacaoMapa::Floresta) palavra = "FLORESTA";
                else if (localAtual == LocalizacaoMapa::PonteReino) palavra = "Ponte do Reino";
                else if (localAtual == LocalizacaoMapa::Reino) palavra = "Reino";
                
                if (!palavra.empty()) {
                    size_t pos = linha.find(palavra);
                    if (pos != std::string::npos) {
                        linha.replace(pos, palavra.length(), Aparencia::cor(Cor::VERDE) + palavra + Aparencia::cor(Cor::RESET));
                    }
                }

                if (!mapasDescobertos) {
                    auto substituirExato = [&](const std::string& alvo, const std::string& substituto) {
                        size_t p = linha.find(alvo);
                        if (p != std::string::npos) {
                            linha.replace(p, alvo.length(), Aparencia::cor(Cor::CINZA) + substituto + Aparencia::cor(Cor::RESET));
                        }
                    };

                    substituirExato("╔══════════╗", "  ╔═════╗   ");
                    substituirExato("║ FLORESTA ║", "  ║ ??? ║   ");
                    substituirExato("╚══════════╝", "  ╚═════╝   ");

                    substituirExato("╔═══════╗", "  ╔═════╗");
                    substituirExato("║ PonteReino ╠", "  ║ ??? ╠");
                    substituirExato("╚═══════╝", "  ╚═════╝");

                    substituirExato("╔═════════╗", "  ╔═════╗  ");
                    substituirExato("║ Reino ║", "  ║ ??? ║  ");
                    substituirExato("╚═════════╝", "  ╚═════╝  ");
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
            
            bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);
            
            if (mapasDescobertos) opcoes.push_back("Floresta");
            else opcoes.push_back(Aparencia::cor(Cor::CINZA) + "???" + Aparencia::cor(Cor::RESET));
            
            if (mapasDescobertos) opcoes.push_back("Ponte do Reino");
            else opcoes.push_back(Aparencia::cor(Cor::CINZA) + "???" + Aparencia::cor(Cor::RESET));

            if (mapasDescobertos) opcoes.push_back("Reino");
            else opcoes.push_back(Aparencia::cor(Cor::CINZA) + "???" + Aparencia::cor(Cor::RESET));
            
            opcoes.push_back("Cancelar Viagem");
            return opcoes;
        },
        // process choice
        [&](int id) {
            if (id == -1 || id == 4) {
                destinoEscolhido = ProximaTransicaoMapa::Nenhuma;
                return false;
            }

            bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);

            if (id == 1 && !mapasDescobertos) {
                msgExtra = "Local ainda nao descoberto!";
                return true;
            }
            if (id == 2 && !mapasDescobertos) {
                msgExtra = "Local ainda nao descoberto!";
                return true;
            }
            if (id == 3 && !mapasDescobertos) {
                msgExtra = "Local ainda nao descoberto!";
                return true;
            }

            ProximaTransicaoMapa destinoCandidato = (id == 0) ? ProximaTransicaoMapa::Vila : 
                                                    (id == 1) ? ProximaTransicaoMapa::Floresta : 
                                                    (id == 2) ? ProximaTransicaoMapa::PonteReino : 
                                                                ProximaTransicaoMapa::Reino;
            LocalizacaoMapa locCandidata = (id == 0) ? LocalizacaoMapa::VilaInicial : 
                                           (id == 1) ? LocalizacaoMapa::Floresta : 
                                           (id == 2) ? LocalizacaoMapa::PonteReino : 
                                                       LocalizacaoMapa::Reino;

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
