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
        float solIntensidade = 0;
        int solR = 0, solG = 0, solB = 0;
        float dirLuzX = 0, dirLuzY = 0;
    };

    inline bool verificarOclusao(float startX, float startY, float endX, float endY, const std::vector<std::string>* matrizDoMapa) {
        if (!matrizDoMapa) return false;
        int x0 = (int)startX; int y0 = (int)startY;
        int x1 = (int)endX; int y1 = (int)endY;
        int dx_line = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy_line = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx_line + dy_line, e2;
        while (true) {
            if (x0 == x1 && y0 == y1) break;
            if (x0 != (int)startX || y0 != (int)startY) {
                if (y0 >= 0 && y0 < (int)matrizDoMapa->size() && x0 >= 0 && x0 < (int)(*matrizDoMapa)[0].size()) {
                    char c = (*matrizDoMapa)[y0][x0];
                    if (c != '.' && c != ' ' && c != '~' && c != '^' && c != 'P' && c != 'F' && c != 'B' && c != 'A' && c != 'Q' && c != 'M') {
                        return true;
                    }
                }
            }
            e2 = 2 * err;
            if (e2 >= dy_line) { err += dy_line; x0 += sx; }
            if (e2 <= dx_line) { err += dx_line; y0 += sy; }
        }
        return false;
    }

    struct EstadoClima { float solR, solG, solB, solIntensidade; int fogR, fogG, fogB; };
    inline EstadoClima misturarClima(const EstadoClima& a, const EstadoClima& b, float t) {
        return {
            a.solR + (b.solR - a.solR) * t, a.solG + (b.solG - a.solG) * t, a.solB + (b.solB - a.solB) * t,
            a.solIntensidade + (b.solIntensidade - a.solIntensidade) * t,
            (int)(a.fogR + (b.fogR - a.fogR) * t), (int)(a.fogG + (b.fogG - a.fogG) * t), (int)(a.fogB + (b.fogB - a.fogB) * t)
        };
    }

    inline InfoLuz calcularInfoLuz(float distancia, float profundidadeMaxima, int temaCeu,
                                   const std::vector<std::tuple<int, int, int>>& luzes,
                                   float hitX, float hitY, const std::vector<std::string>* matrizDoMapa = nullptr, float tempoAnimacao = 0.0f) {
        InfoLuz info;
        info.nevoaPercent = std::min(1.0f, (distancia / (profundidadeMaxima * 0.8f)) *
                                             (distancia / (profundidadeMaxima * 0.8f)));

        EstadoClima climaDinamico = {0,0,0,0, 0,0,0};
        if (temaCeu == 3) { // Static Indoors
            climaDinamico = { 80.0f, 80.0f, 95.0f, 0.4f, 5, 5, 10 }; // Dim, constant ambient light
        } else {
            // Dynamic Time of Day (120 seconds cycle)
            float t = std::fmod(tempoAnimacao, 120.0f) / 120.0f;
            EstadoClima nascerDoSol = { 255.0f, 180.0f, 100.0f, 0.8f, 200, 120, 80 };
            EstadoClima dia         = { 255.0f, 255.0f, 240.0f, 1.0f, 120, 180, 255 };
            EstadoClima porDoSol    = { 255.0f, 140.0f, 80.0f,  0.8f, 200, 100, 50 };
            EstadoClima noite       = { 80.0f,  100.0f, 255.0f, 0.4f, 10,  10,  30 };
            
            if (t < 0.1f)      climaDinamico = misturarClima(nascerDoSol, dia, t / 0.1f);
            else if (t < 0.4f) climaDinamico = dia;
            else if (t < 0.5f) climaDinamico = misturarClima(dia, porDoSol, (t - 0.4f) / 0.1f);
            else if (t < 0.6f) climaDinamico = misturarClima(porDoSol, noite, (t - 0.5f) / 0.1f);
            else if (t < 0.9f) climaDinamico = noite;
            else               climaDinamico = misturarClima(noite, nascerDoSol, (t - 0.9f) / 0.1f);
        }

        info.fogR = climaDinamico.fogR;
        info.fogG = climaDinamico.fogG;
        info.fogB = climaDinamico.fogB;

        float globalRotation = tempoAnimacao * 0.05f;
        float dirLuzX = 0.0f, dirLuzY = 0.0f;
        if (temaCeu != 3) {
            float t = std::fmod(tempoAnimacao, 120.0f) / 120.0f;
            if (t > 0.55f && t < 0.95f) { // Noite (Moon domina)
                dirLuzX = -std::cos(globalRotation);
                dirLuzY = -std::sin(globalRotation);
            } else { // Dia (Sun domina)
                dirLuzX = std::cos(globalRotation);
                dirLuzY = std::sin(globalRotation);
            }
        }
        info.dirLuzX = dirLuzX;
        info.dirLuzY = dirLuzY;

        if (!luzes.empty()) {
            for (const auto& l : luzes) {
                float lx = std::get<0>(l) + 0.5f;
                float ly = std::get<1>(l) + 0.5f;
                int tipo = std::get<2>(l);
                float dx = hitX - lx;
                float dy = hitY - ly;
                float distLuzSq = dx * dx + dy * dy;
                if (distLuzSq < 4.0f) {
                    if (matrizDoMapa != nullptr) {
                        if (verificarOclusao(hitX, hitY, lx, ly, matrizDoMapa)) continue;
                    }

                    float distLuz = std::sqrt(distLuzSq);
                    float intensidade = 1.0f - distLuz / 2.0f;
                    if (intensidade > 0) {
                        intensidade = std::max(0.0f, intensidade);
                        intensidade *= intensidade;
                        if (tipo == 0) { info.luzR += 220 * intensidade; info.luzG += 120 * intensidade; info.luzB += 30 * intensidade; } // Orange Fire
                        else if (tipo == 1) { info.luzR += 220 * intensidade; info.luzG += 220 * intensidade; info.luzB += 255 * intensidade; } // Magical White Portal
                        else if (tipo == 2) { info.luzR += 160 * intensidade; info.luzG += 40 * intensidade; info.luzB += 40 * intensidade; } // Soft Red Enemy
                        else if (tipo == 3) { info.luzR += 150 * intensidade; info.luzG += 130 * intensidade; info.luzB += 60 * intensidade; } // Soft Yellow NPC
                    }
                }
            }
        }
        
        // Direcional Light (Sun/Moon)
        if (matrizDoMapa != nullptr) {
            bool inShadow = false;
            if (temaCeu != 3) {
                // Determine sun/moon direction dynamically based on time of day
                inShadow = verificarOclusao(hitX, hitY, hitX + dirLuzX * 2.5f, hitY + dirLuzY * 2.5f, matrizDoMapa);
            }
            if (!inShadow) {
                info.solR = climaDinamico.solR;
                info.solG = climaDinamico.solG;
                info.solB = climaDinamico.solB;
                info.solIntensidade = climaDinamico.solIntensidade;
            } else {
                // Shadowed ambient light (scaled down)
                info.solR = climaDinamico.solR * 0.5f;
                info.solG = climaDinamico.solG * 0.5f;
                info.solB = climaDinamico.solB * 0.5f;
                info.solIntensidade = climaDinamico.solIntensidade * 0.5f;
            }
        } else {
            info.solR = climaDinamico.solR;
            info.solG = climaDinamico.solG;
            info.solB = climaDinamico.solB;
            info.solIntensidade = climaDinamico.solIntensidade;
        }
        
        return info;
    }

    inline Pixel3D aplicarLuzPrecalculada(int r, int g, int b, const InfoLuz& info, bool escurecer = false, bool isParede = false, float nx = 0.0f, float ny = 0.0f) {
        float lR = info.luzR;
        float lG = info.luzG;
        float lB = info.luzB;
        
        // Direcional Sun Light Mix
        float solFactor = info.solIntensidade;
        if (isParede && info.solIntensidade > 0) {
            float NdotL = 1.0f;
            if (info.dirLuzX != 0.0f || info.dirLuzY != 0.0f) {
                NdotL = std::max(0.0f, nx * info.dirLuzX + ny * info.dirLuzY);
            } else {
                NdotL = std::max(0.0f, nx);
            }
            solFactor *= (0.2f + 0.8f * NdotL); // Ambient 20%, Directional 80%
        }
        
        float baseSolR = r * (info.solR / 255.0f) * solFactor;
        float baseSolG = g * (info.solG / 255.0f) * solFactor;
        float baseSolB = b * (info.solB / 255.0f) * solFactor;

        // Point lights: Emissive (pure light) + Reflected (based on texture)
        // Scaled down to prevent severe over-exposure/clipping which caused flat white blobs ("manchas")
        float emissiveR = lR * 0.25f;
        float emissiveG = lG * 0.25f;
        float emissiveB = lB * 0.25f;
        
        float reflectedR = (r * lR) / 200.0f;
        float reflectedG = (g * lG) / 200.0f;
        float reflectedB = (b * lB) / 200.0f;

        float finalR = baseSolR + emissiveR + reflectedR;
        float finalG = baseSolG + emissiveG + reflectedG;
        float finalB = baseSolB + emissiveB + reflectedB;

        // Limite minimo de luz ambiente para nao ficar no breu absoluto
        finalR = std::max(finalR, r * 0.12f);
        finalG = std::max(finalG, g * 0.12f);
        finalB = std::max(finalB, b * 0.12f);

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
                                float hitX, float hitY, bool escurecer = false, const std::vector<std::string>* matrizDoMapa = nullptr,
                                bool isParede = false, float nx = 0.0f, float ny = 0.0f, float tempoAnimacao = 0.0f) {
        InfoLuz info = calcularInfoLuz(distancia, profundidadeMaxima, temaCeu, luzes, hitX, hitY, matrizDoMapa, tempoAnimacao);
        return aplicarLuzPrecalculada(r, g, b, info, escurecer, isParede, nx, ny);
    }

}
