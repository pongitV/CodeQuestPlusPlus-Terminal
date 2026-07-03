#include "TelaMapaIDE.h"
#include "../../TelasBase/MapaMundial/TelaMapaMundial.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../TemaIDE.h"

int TelaMapaIDE::exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino, const std::string& msgExtra) {
    int idAtual = (localAtual == LocalizacaoMapa::VilaInicial) ? 0 :
                  (localAtual == LocalizacaoMapa::Floresta) ? 1 :
                  (localAtual == LocalizacaoMapa::PonteReino) ? 2 : 3;

    int idSelecionado = idAtual;
    bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);

    bool piscaAmarelo = true;
    auto ultimoBlink = std::chrono::steady_clock::now();
    bool precisaRenderizar = true;
    std::string msg = msgExtra;

    while (true) {
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

            auto getMargemEsq = [&](int) {
                return std::string("   ") + Aparencia::cor(Cor::BRANCO) + "\u2551" + Aparencia::cor(Cor::RESET);
            };

            std::string margemDireita = Aparencia::cor(Cor::BRANCO) + "\u2551" + Aparencia::cor(Cor::RESET) + "   ";

            std::vector<std::string> linhas;
            linhas.push_back(Aparencia::cor(Cor::BRANCO) + "   \u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557   " + Aparencia::cor(Cor::RESET));
            linhas.push_back(getMargemEsq(-1) + std::string(47, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(28, ' ') + getColor(0, true) + "\u2554\u2550\u2550\u2550\u2550\u2550\u2557" + Aparencia::cor(Cor::RESET) + std::string(11, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(0) + std::string(28, ' ') + getColor(0, true) + "\u2551" + getText(" VILA ", 0, true) + "\u2551" + Aparencia::cor(Cor::RESET) + " " + getProg(progressoVila, true) + std::string(6, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(28, ' ') + getColor(0, true) + "\u255A\u2550\u2550\u2550\u2550\u2550\u255D" + Aparencia::cor(Cor::RESET) + std::string(11, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(31, ' ') + getColor(0, true) + "\u2551\u2551" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557   ") + Aparencia::cor(Cor::RESET) + std::string(9, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "\u2551" : "  \u2551") + getText(mapasDescobertos ? " FLORESTA " : " ??? ", 1, mapasDescobertos) + (mapasDescobertos ? "\u2551" : "\u2551   ") + Aparencia::cor(Cor::RESET) + " " + getProg(progressoFloresta, mapasDescobertos) + std::string(4, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(26, ' ') + getColor(1, mapasDescobertos) + (mapasDescobertos ? "\u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D   ") + Aparencia::cor(Cor::RESET) + std::string(9, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(31, ' ') + getColor(1, mapasDescobertos) + "\u2551\u2551" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557") + Aparencia::cor(Cor::RESET) + std::string(7, ' ') + getColor(1, mapasDescobertos) + "\u2551\u2551" + Aparencia::cor(Cor::RESET) + std::string(14, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(2) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "\u2551" : "  \u2551") + getText(mapasDescobertos ? " Ponte " : " ??? ", 2, mapasDescobertos) + (mapasDescobertos ? "\u2560" : "\u2560") + Aparencia::cor(Cor::RESET) + getColor(1, mapasDescobertos) + "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2563\u2551 " + Aparencia::cor(Cor::RESET) + getProg(progressoPonteReino, mapasDescobertos) + std::string(9, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(2, mapasDescobertos) + (mapasDescobertos ? "\u255A\u2550\u2550\u2550\u2566\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D") + Aparencia::cor(Cor::RESET) + std::string(23, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(19, ' ') + getColor(2, mapasDescobertos) + "\u2551\u2551" + Aparencia::cor(Cor::RESET) + std::string(26, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "\u2554\u2550\u2550\u2550\u2569\u2550\u2550\u2550\u2550\u2550\u2557" : "  \u2554\u2550\u2550\u2550\u2550\u2557  ") + Aparencia::cor(Cor::RESET) + std::string(21, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(3) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "\u2551" : "  \u2551") + getText(mapasDescobertos ? " Reino   " : " ???   ", 3, mapasDescobertos) + (mapasDescobertos ? "\u2551" : "\u2551") + Aparencia::cor(Cor::RESET) + " " + getProg(progressoReino, mapasDescobertos) + std::string(16, ' ') + margemDireita);
            linhas.push_back(getMargemEsq(-1) + std::string(15, ' ') + getColor(3, mapasDescobertos) + (mapasDescobertos ? "\u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D" : "  \u255A\u2550\u2550\u2550\u2550\u255D  ") + Aparencia::cor(Cor::RESET) + std::string(21, ' ') + margemDireita);

            linhas.push_back(getMargemEsq(-1) + std::string(47, ' ') + margemDireita);
            linhas.push_back(Aparencia::cor(Cor::BRANCO) + "   \u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u255D   " + Aparencia::cor(Cor::RESET));

            int larguraTerminal = Aparencia::obterLarguraTerminal();
            int espacosMargem = std::max(0, (larguraTerminal - 55) / 2);
            std::string margemCentral(espacosMargem, ' ');

            for (const auto& l : linhas) {
                std::cout << margemCentral << l << "\n";
            }

            std::cout << "\n";
            if (!msg.empty()) {
                Aparencia::imprimirCentralizado(msg, Aparencia::cor(Cor::AMARELO));
                msg = "";
            } else {
                Aparencia::imprimirCentralizado("Utilize [W][S] ou [Setas] para selecionar um destino.", Aparencia::cor(Cor::CINZA));
                Aparencia::imprimirCentralizado("[ENTER] Viajar | [Q]/[M] Cancelar", Aparencia::cor(Cor::CINZA));
            }
            std::cout << "\n";

            precisaRenderizar = false;
        }

        if (ControleDeInput::teclaPressionada()) {
            char tecla = ControleDeInput::lerTecla();
            if (tecla == '\033') {
                char proxTecla = ControleDeInput::lerTecla();
                if (proxTecla == '[') {
                    proxTecla = ControleDeInput::lerTecla();
                    if (proxTecla == 'A') tecla = 'W';
                    else if (proxTecla == 'B') tecla = 'S';
                }
            } else if (tecla == -32 || tecla == 0) {
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
                return -1;
            } else if (tecla == '\n' || tecla == '\r') {
                return idSelecionado;
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
}
