#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "FrameOverlayUtils.h"
#include "MenuRaycasterLayout.h"
#include "../../EngineRaycaster/RaycasterMundo.h"
#include "../../../../Core/Utilidades/Aparencia.h"

namespace MenuRaycasterUtils {

    inline float s_cicloCelestial = 0.0f;
    inline float s_velocidadeCelestial = 0.005f;
    inline int s_estrelasX = 0;
    inline int s_guerreiroX = 5;
    inline int s_magoX = -10;
    inline int s_arqueiroX = -25;
    inline int s_guerreiroPassos = 0;

    inline void incrementarCicloDia() {
        s_cicloCelestial += s_velocidadeCelestial;
        if (s_cicloCelestial >= 4.6f) s_cicloCelestial -= 4.6f;
        s_estrelasX = (s_estrelasX + 1) % 2000;
        s_guerreiroX += 1;
        s_magoX += 1;
        s_arqueiroX += 1;
        int largura = Aparencia::obterLarguraTerminal();
        if (largura > 0) {
            if (s_guerreiroX > largura) s_guerreiroX = -45;
            if (s_magoX > largura) s_magoX = -45;
            if (s_arqueiroX > largura) s_arqueiroX = -45;
        }
        s_guerreiroPassos++;
    }

    inline void aplicarCicloDiaNoite(std::vector<std::string>& frame) {
        if (frame.empty()) return;

        int larguraTela = Aparencia::obterLarguraTerminal();
        if (larguraTela <= 0) larguraTela = 120;

        int totalLinhas = (int)frame.size();
        int alturaCeu = totalLinhas;

        float FOV = 1.6f;

        long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float animPulsacao = (globalMs % 100000) / 1000.0f;

        float fadeBase = std::cos((s_cicloCelestial - 1.15f) * (6.2831853f / 4.6f));
        float fadeCeu = (fadeBase * 4.0f + 1.0f) * 0.5f;
        fadeCeu = std::max(0.0f, std::min(1.0f, fadeCeu));

        float posSol = -1.15f + s_cicloCelestial;
        float posLua = -1.15f + (s_cicloCelestial - 2.3f);

        for (int y = 0; y < totalLinhas; ++y) {
            std::string novaLinha;
            novaLinha.reserve(larguraTela * 28);

            for (int cellX = 0; cellX < larguraTela; ++cellX) {
                float colAng = ((float)cellX / (float)larguraTela - 0.5f) * FOV;

                float raioAnguloMenu = std::fmod(colAng + 6.2831853f, 6.2831853f);

                float anguloCeuSun = std::fmod(colAng - posSol + 6.2831853f, 6.2831853f);
                float anguloCeuMoon = std::fmod(colAng - posLua + 6.2831853f, 6.2831853f);

                Pixel3D pxDia   = RaycasterMundo::obterPixelTeto(2, raioAnguloMenu, anguloCeuSun, y, alturaCeu * 2, animPulsacao);
                Pixel3D pxNoite = RaycasterMundo::obterPixelTeto(1, raioAnguloMenu, anguloCeuMoon, y, alturaCeu * 2, animPulsacao);

                if (pxNoite.hasFg && pxNoite.ch != ' ') {
                    pxNoite.r = pxNoite.fgR;
                    pxNoite.g = pxNoite.fgG;
                    pxNoite.b = pxNoite.fgB;
                }

                int r = (int)((float)pxDia.r * fadeCeu + (float)pxNoite.r * (1.0f - fadeCeu));
                int g = (int)((float)pxDia.g * fadeCeu + (float)pxNoite.g * (1.0f - fadeCeu));
                int b = (int)((float)pxDia.b * fadeCeu + (float)pxNoite.b * (1.0f - fadeCeu));

                if (y >= totalLinhas * 2 / 3) {
                    float campoY = (float)(y - totalLinhas * 2 / 3) / (float)(totalLinhas / 3);
                    r = 25 + (int)(campoY * 35);
                    g = 90 + (int)(campoY * 50 + (cellX % 3) * 3);
                    b = 15 + (int)(campoY * 25);

                    unsigned int fX = (unsigned int)(cellX * 7 + 13) * 374761393U;
                    unsigned int fY = (unsigned int)(y * 11 + 7) * 668265263U;
                    unsigned int fHash = fX + fY;
                    fHash = (fHash ^ (fHash >> 13)) * 1274126177U;
                    int flor = fHash % 1000;
                    if (flor < 6) {
                        r = 235; g = 40; b = 40;
                    } else if (flor < 11) {
                        r = 255; g = 220; b = 50;
                    } else if (flor < 15) {
                        r = 240; g = 240; b = 255;
                    }
                    if (campoY < 0.08f) {
                        float blend = campoY / 0.08f;
                        r = (int)((float)r * blend + 60.0f * (1.0f - blend));
                        g = (int)((float)g * blend + 120.0f * (1.0f - blend));
                        b = (int)((float)b * blend + 80.0f * (1.0f - blend));
                    }
                }

                r = std::max(0, std::min(255, r));
                g = std::max(0, std::min(255, g));
                b = std::max(0, std::min(255, b));

                novaLinha += "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m ";
                novaLinha += "\033[0m";
            }
            frame[y] = std::move(novaLinha);
        }
    }

    inline void desenharCastelo() {
        if (s_fundo3DMenu.empty()) return;
        int altura = (int)s_fundo3DMenu.size();
        int yBase = altura * 2 / 3 + 4 - 18;
        if (yBase < 0) yBase = 0;

        int largura = Aparencia::obterLarguraTerminal();
        int xCentro = std::max(0, (largura - 47) / 2);
        pintarSpriteNoFrame(yBase, xCentro, ArtesRaycaster::casteloMenu, 110, 115, 125);
    }

    inline void animarGuerreiro() {
        if (s_fundo3DMenu.empty()) return;
        int altura = (int)s_fundo3DMenu.size();
        int yBase = altura * 2 / 3 + 4;
        if (yBase + 4 > altura) return;

        pintarSpriteNoFrame(yBase, s_guerreiroX, ArtesRaycaster::guerreiroArte, 190, 195, 210);
    }

    inline void animarMago() {
        if (s_fundo3DMenu.empty()) return;
        int altura = (int)s_fundo3DMenu.size();
        int yBase = altura * 2 / 3 + 4;
        if (yBase + 4 > altura) return;

        pintarSpriteNoFrame(yBase, s_magoX, ArtesRaycaster::magoArte, 120, 100, 210);
    }

    inline void animarArqueiro() {
        if (s_fundo3DMenu.empty()) return;
        int altura = (int)s_fundo3DMenu.size();
        int yBase = altura * 2 / 3 + 4;
        if (yBase + 4 > altura) return;

        pintarSpriteNoFrame(yBase, s_arqueiroX, ArtesRaycaster::arqueiroArte, 100, 180, 100);
    }

    inline void sobreporLogoCodeQuest() {
        if (s_fundo3DMenu.empty()) return;
        int larguraConsole = Aparencia::obterLarguraTerminal();
        if (larguraConsole <= 0) return;
        std::string texto = "CodeQuest";
        std::string mais = "++";
        int totalWidth = Aparencia::obterComprimentoVisual(texto) + Aparencia::obterComprimentoVisual(mais);
        int margemLogo = std::max(0, (larguraConsole - totalWidth) / 2);
        sobreporNoFrame(1, margemLogo, texto, 255, 255, 255);
        sobreporNoFrame(1, margemLogo + (int)texto.length(), mais, 255, 165, 0);
    }

    inline void exibirFundo3D(std::ostream& out) {
        aplicarCicloDiaNoite(s_fundo3DMenu);
        desenharCastelo();
        animarGuerreiro();
        animarMago();
        animarArqueiro();
        sobreporLogoCodeQuest();
        out << "\033[H";
        for (size_t y = 0; y < s_fundo3DMenu.size(); ++y) {
            out << s_fundo3DMenu[y];
            if (y < s_fundo3DMenu.size() - 1) out << "\n";
        }
    }

}
