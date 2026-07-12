#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include "RaycasterSprites.h"

namespace Iluminador {

    struct InfoLuz {
        float luzR = 0, luzG = 0, luzB = 0;
        float nevoaPercent = 0;
        int fogR = 0, fogG = 0, fogB = 0;
    };

    inline InfoLuz calcularInfoLuz(float distancia, float profundidadeMaxima, int temaCeu,
                                   const std::vector<std::tuple<int, int, int>>& luzes,
                                   float hitX, float hitY, const std::vector<std::string>* matrizDoMapa = nullptr) {
        InfoLuz info;
        info.nevoaPercent = std::min(1.0f, (distancia / (profundidadeMaxima * 0.8f)) *
                                             (distancia / (profundidadeMaxima * 0.8f)));

        if (temaCeu == 1) { info.fogR = 5; info.fogG = 5; info.fogB = 15; }
        else if (temaCeu == 2) { info.fogR = 10; info.fogG = 60; info.fogB = 150; }

        if (!luzes.empty()) {
            for (const auto& l : luzes) {
                int lx = std::get<0>(l), ly = std::get<1>(l), tipo = std::get<2>(l);
                float dx = hitX - lx;
                if (dx > 8.0f || dx < -8.0f) continue;
                float dy = hitY - ly;
                if (dy > 8.0f || dy < -8.0f) continue;
                float distLuz = dx * dx + dy * dy;
                if (distLuz < 64.0f) {
                    if (matrizDoMapa != nullptr) {
                        bool occluded = false;
                        int x0 = (int)hitX; int y0 = (int)hitY;
                        int x1 = lx; int y1 = ly;
                        int dx_line = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
                        int dy_line = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
                        int err = dx_line + dy_line, e2;
                        while (true) {
                            if (x0 == x1 && y0 == y1) break;
                            if (x0 != (int)hitX || y0 != (int)hitY) {
                                if (y0 >= 0 && y0 < (int)matrizDoMapa->size() && x0 >= 0 && x0 < (int)(*matrizDoMapa)[0].size()) {
                                    char c = (*matrizDoMapa)[y0][x0];
                                    if (c != '.' && c != ' ' && c != '~' && c != '^' && c != 'P' && c != 'F' && c != 'B' && c != 'A' && c != 'Q' && c != 'M') {
                                        occluded = true; break;
                                    }
                                }
                            }
                            e2 = 2 * err;
                            if (e2 >= dy_line) { err += dy_line; x0 += sx; }
                            if (e2 <= dx_line) { err += dx_line; y0 += sy; }
                        }
                        if (occluded) continue;
                    }

                    float intensidade = 1.0f - distLuz / 64.0f;
                    intensidade = std::max(0.0f, intensidade);
                    intensidade *= intensidade;
                    if (tipo == 0) { info.luzR += 220 * intensidade; info.luzG += 120 * intensidade; info.luzB += 30 * intensidade; }
                    else if (tipo == 1) { info.luzR += 180 * intensidade; info.luzG += 220 * intensidade; info.luzB += 255 * intensidade; }
                }
            }
        }
        return info;
    }

    inline Pixel3D aplicarLuzPrecalculada(int r, int g, int b, const InfoLuz& info, bool escurecer = false) {
        float lR = info.luzR;
        float lG = info.luzG;
        float lB = info.luzB;
        
        float finalR = r + (lR * 0.25f) + (r * lR) / 160.0f;
        float finalG = g + (lG * 0.25f) + (g * lG) / 160.0f;
        float finalB = b + (lB * 0.25f) + (b * lB) / 160.0f;

        float nf = info.nevoaPercent;
        finalR = finalR * (1.0f - nf) + info.fogR * nf;
        finalG = finalG * (1.0f - nf) + info.fogG * nf;
        finalB = finalB * (1.0f - nf) + info.fogB * nf;

        if (escurecer) { finalR *= 0.65f; finalG *= 0.65f; finalB *= 0.65f; }

        Pixel3D px;
        px.r = (uint8_t)std::min(255.0f, std::max(0.0f, finalR));
        px.g = (uint8_t)std::min(255.0f, std::max(0.0f, finalG));
        px.b = (uint8_t)std::min(255.0f, std::max(0.0f, finalB));
        px.ch = ' ';
        return px;
    }

    inline Pixel3D aplicarNevoa(int r, int g, int b, float distancia, float profundidadeMaxima,
                                int temaCeu, const std::vector<std::tuple<int, int, int>>& luzes,
                                float hitX, float hitY, bool escurecer = false, const std::vector<std::string>* matrizDoMapa = nullptr) {
        InfoLuz info = calcularInfoLuz(distancia, profundidadeMaxima, temaCeu, luzes, hitX, hitY, matrizDoMapa);
        return aplicarLuzPrecalculada(r, g, b, info, escurecer);
    }

}
