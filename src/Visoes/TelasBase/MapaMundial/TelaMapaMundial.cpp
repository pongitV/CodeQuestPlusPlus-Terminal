#include "TelaMapaMundial.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Mundo/MapaInteracao.h"
#include "../TelaBase.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include <thread>
#include <chrono>

ProximaTransicaoMapa TelaMapaMundial::exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino) {
    ProximaTransicaoMapa destinoEscolhido = ProximaTransicaoMapa::Nenhuma;
    std::string msgExtra = "";
    bool executando = true;

    int idAtual = (localAtual == LocalizacaoMapa::VilaInicial) ? 0 :
                  (localAtual == LocalizacaoMapa::Floresta) ? 1 :
                  (localAtual == LocalizacaoMapa::PonteReino) ? 2 : 3;

    int idSelecionado = idAtual;
    bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);

    bool piscaAmarelo = true;
    auto ultimoBlink = std::chrono::steady_clock::now();
    bool precisaRenderizar = true;

    while (executando) {
        if (precisaRenderizar) {
            Aparencia::limparTela();
            
            std::cout << "\n";
            Aparencia::imprimirCentralizado("MAPA MUNDIAL - VIAGEM RAPIDA", Aparencia::cor(Cor::NEGRITO, Cor::BRANCO));
            std::cout << "\n\n";

            auto getProg = [](int val, bool disc) {
                if (!disc) return Aparencia::cor(Cor::CINZA) + "??? " + Aparencia::cor(Cor::RESET);
                std::string p = std::to_string(val);
                if (p.length() < 3) p = std::string(3 - p.length(), ' ') + p;
                std::string c = (val >= 100) ? Aparencia::cor(Cor::CIANO) : (val > 0) ? Aparencia::cor(Cor::AMARELO) : Aparencia::cor(Cor::BRANCO);
                return c + p + "%" + Aparencia::cor(Cor::RESET);
            };

            auto getColor = [&](int id, bool disc) {
                if (id == idSelecionado) return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO);
                if (!disc) return Aparencia::cor(Cor::CINZA);
                return Aparencia::cor(Cor::BRANCO);
            };

            auto getText = [&](std::string txt, int id, bool disc) {
                if (!disc) {
                    if (txt.length() == 6) txt = std::string(" ???  ");
                    else if (txt.length() == 10) txt = std::string("   ???    ");
                    else if (txt.length() == 7) txt = std::string("  ???  ");
                    else if (txt.length() == 9) txt = std::string("   ???   ");
                }
                
                std::string res = txt;
                if (id == idSelecionado && piscaAmarelo) {
                    res.front() = '>';
                    res.back() = '<';
                }

                if (id == idSelecionado) {
                    if (piscaAmarelo) {
                        res = Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + res;
                    } else {
                        if (id == idAtual) res = Aparencia::cor(Cor::NEGRITO, Cor::VERDE) + res;
                        else res = Aparencia::cor(Cor::NEGRITO, Cor::BRANCO) + res;
                    }
                } else if (id == idAtual) {
                    res = Aparencia::cor(Cor::NEGRITO, Cor::VERDE) + res;
                } else {
                    res = Aparencia::cor(Cor::NEGRITO, Cor::BRANCO) + res;
                }

                res += getColor(id, disc);
                return res;
            };

            auto getMargemEsq = [&](int id) {
                return std::string("   ") + Aparencia::cor(Cor::BRANCO) + "║" + Aparencia::cor(Cor::RESET);
            };

            std::string margemDireita = Aparencia::cor(Cor::BRANCO) + "║" + Aparencia::cor(Cor::RESET) + "   ";
            
            std::vector<std::string> linhas;
            linhas.push_back(Aparencia::cor(Cor::BRANCO) + "   ╔═══════════════════════════════════════════════╗   " + Aparencia::cor(Cor::RESET));
            linhas.push_back(getMargemEsq(-1) + std::string(47, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(28, ' ') + getColor(0, true) + "╔══════╗" + Aparencia::cor(Cor::RESET) + std::string(11, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(0) + std::string(28, ' ') + getColor(0, true) + "║" + getText(" VILA ", 0, true) + "║" + Aparencia::cor(Cor::RESET) + " " + getProg(progressoVila, true) + std::string(6, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(28, ' ') + getColor(0, true) + "╚══════╝" + Aparencia::cor(Cor::RESET) + std::string(11, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(31, ' ') + getColor(0, true) + "║║" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "╔══════════╗" : "  ╔═════╗   ") + Aparencia::cor(Cor::RESET) + std::string(9, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "║" : "  ║") + getText(mapasDescobertos ? " FLORESTA " : " ??? ", 1, mapasDescobertos) + (mapasDescobertos ? "║" : "║   ") + Aparencia::cor(Cor::RESET) + " " + getProg(progressoFloresta, mapasDescobertos) + std::string(4, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "╚══════════╝" : "  ╚═════╝   ") + Aparencia::cor(Cor::RESET) + std::string(9, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(31, ' ') + getColor(1, mapasDescobertos) + "║║" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "╔═══════╗" : "  ╔═════╗") + Aparencia::cor(Cor::RESET) + std::string(7, ' ') + getColor(1, mapasDescobertos) + "║║" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(2) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "║" : "  ║") + getText(mapasDescobertos ? " Ponte " : " ??? ", 2, mapasDescobertos) + (mapasDescobertos ? "╠" : "╠") + Aparencia::cor(Cor::RESET) + getColor(1, mapasDescobertos) + "═══════╣║ " + Aparencia::cor(Cor::RESET) + getProg(progressoPonteReino, mapasDescobertos) + std::string(9, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "╚═══╦═══╝" : "  ╚═════╝") + Aparencia::cor(Cor::RESET) + std::string(23, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(19, ' ') + getColor(2, mapasDescobertos) + "║║" + Aparencia::cor(Cor::RESET) + std::string(26, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "╔═══╩═════╗" : "  ╔═════╗  ") + Aparencia::cor(Cor::RESET) + std::string(21, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(3) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "║" : "  ║") + getText(mapasDescobertos ? " Reino   " : " ???   ", 3, mapasDescobertos) + (mapasDescobertos ? "║" : "║") + Aparencia::cor(Cor::RESET) + " " + getProg(progressoReino, mapasDescobertos) + std::string(16, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "╚═════════╝" : "  ╚═════╝  ") + Aparencia::cor(Cor::RESET) + std::string(21, ' ') + margemDireita);
            
            linhas.push_back(getMargemEsq(-1) + std::string(47, ' ') + margemDireita);
            linhas.push_back(Aparencia::cor(Cor::BRANCO) + "   ╚═══════════════════════════════════════════════╝   " + Aparencia::cor(Cor::RESET));

            int larguraTerminal = Aparencia::obterLarguraTerminal();
            int espacosMargem = std::max(0, (larguraTerminal - 55) / 2);
            std::string margemCentral(espacosMargem, ' ');

            for (const auto& l : linhas) {
                std::cout << margemCentral << l << "\n";
            }

            std::cout << "\n";
            if (!msgExtra.empty()) {
                Aparencia::imprimirCentralizado(msgExtra, Aparencia::cor(Cor::AMARELO));
                msgExtra = "";
            } else {
                Aparencia::imprimirCentralizado("Utilize [W][S] ou [Setas] para selecionar um destino.", Aparencia::cor(Cor::CINZA));
                Aparencia::imprimirCentralizado("[ENTER] Viajar | [Q]/[M] Cancelar", Aparencia::cor(Cor::CINZA));
            }
            std::cout << "\n";
            
            precisaRenderizar = false;
        }

        if (ControleDeInput::teclaPressionada()) {
            char tecla = ControleDeInput::lerTecla();
            if (tecla == '\033') { // Setas POSIX
                char proxTecla = ControleDeInput::lerTecla();
                if (proxTecla == '[') {
                    proxTecla = ControleDeInput::lerTecla();
                    if (proxTecla == 'A') tecla = 'W';
                    else if (proxTecla == 'B') tecla = 'S';
                }
            } else if (tecla == -32 || tecla == 0) { // Setas Windows
                char proxTecla = ControleDeInput::lerTecla();
                if (proxTecla == 72) tecla = 'W';
                else if (proxTecla == 80) tecla = 'S';
            } else {
                tecla = toupper(tecla);
            }

            if (tecla == 'W') {
                if (idSelecionado > 0) idSelecionado--;
                precisaRenderizar = true;
                piscaAmarelo = true;
                ultimoBlink = std::chrono::steady_clock::now();
            } else if (tecla == 'S') {
                if (idSelecionado < 3) idSelecionado++;
                precisaRenderizar = true;
                piscaAmarelo = true;
                ultimoBlink = std::chrono::steady_clock::now();
            } else if (tecla == 'Q' || tecla == 'M') {
                destinoEscolhido = ProximaTransicaoMapa::Nenhuma;
                executando = false;
            } else if (tecla == '\n' || tecla == '\r') {
                if (idSelecionado > 0 && !mapasDescobertos) {
                    msgExtra = "Local ainda nao descoberto!";
                    precisaRenderizar = true;
                } else if (idSelecionado == idAtual) {
                    msgExtra = "Voce ja esta neste local!";
                    precisaRenderizar = true;
                } else {
                    destinoEscolhido = (idSelecionado == 0) ? ProximaTransicaoMapa::Vila : 
                                       (idSelecionado == 1) ? ProximaTransicaoMapa::Floresta : 
                                       (idSelecionado == 2) ? ProximaTransicaoMapa::PonteReino : 
                                                              ProximaTransicaoMapa::Reino;
                    executando = false;
                }
            }
        } else {
            auto agora = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoBlink).count() >= 500) {
                piscaAmarelo = !piscaAmarelo;
                ultimoBlink = agora;
                precisaRenderizar = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    return destinoEscolhido;
}
