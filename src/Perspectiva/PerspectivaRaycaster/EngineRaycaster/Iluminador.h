#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include "RaycasterSprites.h"

namespace Iluminador {

    inline Pixel3D aplicarNevoa(int r, int g, int b, float distancia, float profundidadeMaxima,
                                int temaCeu, const std::vector<std::tuple<int, int, int>>& luzes,
                                float hitX, float hitY, bool escurecer = false) {
        float nevoaPercent = std::min(1.0f, (distancia / (profundidadeMaxima * 0.8f)) *
                                             (distancia / (profundidadeMaxima * 0.8f)));

        int fogR = 0, fogG = 0, fogB = 0;
        if (temaCeu == 1) { fogR = 5; fogG = 5; fogB = 15; }
        else if (temaCeu == 2) { fogR = 10; fogG = 60; fogB = 150; }
        else { fogR = 0; fogG = 0; fogB = 0; }

        float luzR = 0, luzG = 0, luzB = 0;
        for (const auto& l : luzes) {
            int lx = std::get<0>(l), ly = std::get<1>(l), tipo = std::get<2>(l);
            float dx = hitX - lx, dy = hitY - ly;
            float distLuz = dx * dx + dy * dy;
            if (distLuz < 64.0f) {
                float intensidade = 1.0f - std::sqrt(distLuz) / 8.0f;
                intensidade = std::max(0.0f, intensidade);
                intensidade *= intensidade;
                if (tipo == 0) { luzR += 220 * intensidade; luzG += 120 * intensidade; luzB += 30 * intensidade; }
                else if (tipo == 1) { luzR += 180 * intensidade; luzG += 220 * intensidade; luzB += 255 * intensidade; }
            }
        }

        float finalR = r + luzR, finalG = g + luzG, finalB = b + luzB;
        float nf = nevoaPercent;
        finalR = finalR * (1.0f - nf) + fogR * nf;
        finalG = finalG * (1.0f - nf) + fogG * nf;
        finalB = finalB * (1.0f - nf) + fogB * nf;

        if (escurecer) { finalR *= 0.65f; finalG *= 0.65f; finalB *= 0.65f; }

        Pixel3D px;
        px.r = (uint8_t)std::min(255, std::max(0, (int)finalR));
        px.g = (uint8_t)std::min(255, std::max(0, (int)finalG));
        px.b = (uint8_t)std::min(255, std::max(0, (int)finalB));
        px.ch = ' ';
        return px;
    }

}
