#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterSprites.h"

namespace Highlighter {

    struct InfoLight {
        float lightR = 0, lightG = 0, lightB = 0;
        float fogPercentage = 0;
        int fogR = 0, fogG = 0, fogB = 0;
        float sunIntensity = 0;
        int sunR = 0, sunG = 0, sunB = 0;
        float sayLightX = 0, sayLightY = 0;
    };

    inline bool checkOcclusion(float startX, float startY, float endX, float endY, const std::vector<std::string>* mapMatrix) {
        if (!mapMatrix) return false;
        int x0 = (int)startX; int y0 = (int)startY;
        int x1 = (int)endX; int y1 = (int)endY;
        int dx_line = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy_line = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx_line + dy_line, e2;
        while (true) {
            if (x0 == x1 && y0 == y1) break;
            if (x0 != (int)startX || y0 != (int)startY) {
                if (y0 >= 0 && y0 < (int)mapMatrix->size() && x0 >= 0 && x0 < (int)(*mapMatrix)[0].size()) {
                    char c = (*mapMatrix)[y0][x0];
                    if (c != '.' && c != ' ' && c != '~' && c != '^' && c != 'P' && c != 'F' && c != 'B' && c != 'A' && c != 'Q' && c != 'M' && 
                        c != 'T' && c != 'G' && c != 'O' && c != 'S' && c != 'C' && c != 'I' && c != 'Y' && c != 'Z' && c != 'V' && c != 'W' && c != 'N') {
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

    struct StateClimate { float sunR, sunG, sunB, sunIntensity; int fogR, fogG, fogB; };
    inline StateClimate mixClimate(const StateClimate& a, const StateClimate& b, float t) {
        return {
            a.sunR + (b.sunR - a.sunR) * t, a.sunG + (b.sunG - a.sunG) * t, a.sunB + (b.sunB - a.sunB) * t,
            a.sunIntensity + (b.sunIntensity - a.sunIntensity) * t,
            (int)(a.fogR + (b.fogR - a.fogR) * t), (int)(a.fogG + (b.fogG - a.fogG) * t), (int)(a.fogB + (b.fogB - a.fogB) * t)
        };
    }

    inline InfoLight calculateInfoLight(float distance, float depthMaximum, int themeSky,
                                   const std::vector<std::tuple<int, int, int>>& lights,
                                   float hitX, float hitY, const std::vector<std::string>* mapMatrix = nullptr, float timeAnimation = 0.0f) {
        InfoLight info;
        info.fogPercentage = std::min(1.0f, (distance / (depthMaximum * 0.8f)) *
                                             (distance / (depthMaximum * 0.8f)));

        StateClimate climateDynamic = {0,0,0,0, 0,0,0};
        if (themeSky == 3) { // Static Indoors
            climateDynamic = { 80.0f, 80.0f, 95.0f, 0.4f, 5, 5, 10 }; // Dim, constant ambient light
        } else {
            // Dynamic Time of Day (120 seconds cycle)
            float t = std::fmod(timeAnimation, 120.0f) / 120.0f;
            StateClimate bebornDoSun = { 255.0f, 180.0f, 100.0f, 0.8f, 200, 120, 80 };
            StateClimate day         = { 255.0f, 255.0f, 240.0f, 1.0f, 120, 180, 255 };
            StateClimate byDoSun    = { 255.0f, 140.0f, 80.0f,  0.8f, 200, 100, 50 };
            StateClimate night       = { 80.0f,  100.0f, 255.0f, 0.4f, 10,  10,  30 };
            
            if (t < 0.1f)      climateDynamic = mixClimate(bebornDoSun, day, t / 0.1f);
            else if (t < 0.4f) climateDynamic = day;
            else if (t < 0.5f) climateDynamic = mixClimate(day, byDoSun, (t - 0.4f) / 0.1f);
            else if (t < 0.6f) climateDynamic = mixClimate(byDoSun, night, (t - 0.5f) / 0.1f);
            else if (t < 0.9f) climateDynamic = night;
            else               climateDynamic = mixClimate(night, bebornDoSun, (t - 0.9f) / 0.1f);
        }

        info.fogR = climateDynamic.fogR;
        info.fogG = climateDynamic.fogG;
        info.fogB = climateDynamic.fogB;

        float globalRotation = timeAnimation * 0.05f;
        float sayLightX = 0.0f, sayLightY = 0.0f;
        if (themeSky != 3) {
            float t = std::fmod(timeAnimation, 120.0f) / 120.0f;
            if (t > 0.55f && t < 0.95f) { // Noite (Moon domina)
                sayLightX = -std::cos(globalRotation);
                sayLightY = -std::sin(globalRotation);
            } else { // Dia (Sun domina)
                sayLightX = std::cos(globalRotation);
                sayLightY = std::sin(globalRotation);
            }
        }
        info.sayLightX = sayLightX;
        info.sayLightY = sayLightY;

        if (!lights.empty()) {
            for (const auto& l : lights) {
                float lx = std::get<0>(l) + 0.5f;
                float ly = std::get<1>(l) + 0.5f;
                int type = std::get<2>(l);
                float dx = hitX - lx;
                float dy = hitY - ly;
                float distLightSq = dx * dx + dy * dy;
                if (distLightSq < 4.0f) {
                    if (mapMatrix != nullptr) {
                        if (checkOcclusion(hitX, hitY, lx, ly, mapMatrix)) continue;
                    }

                    float distLight = std::sqrt(distLightSq);
                    float intensity = 1.0f - distLight / 2.0f;
                    if (intensity > 0) {
                        intensity = std::max(0.0f, intensity);
                        intensity *= intensity;
                        if (type == 0) { info.lightR += 220 * intensity; info.lightG += 120 * intensity; info.lightB += 30 * intensity; } // Orange Fire
                        else if (type == 1) { info.lightR += 220 * intensity; info.lightG += 220 * intensity; info.lightB += 255 * intensity; } // Magical White Portal
                        else if (type == 2) { info.lightR += 160 * intensity; info.lightG += 40 * intensity; info.lightB += 40 * intensity; } // Soft Red Enemy
                        else if (type == 3) { info.lightR += 150 * intensity; info.lightG += 130 * intensity; info.lightB += 60 * intensity; } // Soft Yellow NPC
                    }
                }
            }
        }
        
        // Direcional Light (Sun/Moon)
        if (mapMatrix != nullptr) {
            bool inShadow = false;
            if (themeSky != 3) {
                // Determine sun/moon direction dynamically based on time of day
                inShadow = checkOcclusion(hitX, hitY, hitX + sayLightX * 2.5f, hitY + sayLightY * 2.5f, mapMatrix);
            }
            if (!inShadow) {
                info.sunR = climateDynamic.sunR;
                info.sunG = climateDynamic.sunG;
                info.sunB = climateDynamic.sunB;
                info.sunIntensity = climateDynamic.sunIntensity;
            } else {
                // Shadowed ambient light (scaled down)
                info.sunR = climateDynamic.sunR * 0.5f;
                info.sunG = climateDynamic.sunG * 0.5f;
                info.sunB = climateDynamic.sunB * 0.5f;
                info.sunIntensity = climateDynamic.sunIntensity * 0.5f;
            }
        } else {
            info.sunR = climateDynamic.sunR;
            info.sunG = climateDynamic.sunG;
            info.sunB = climateDynamic.sunB;
            info.sunIntensity = climateDynamic.sunIntensity;
        }
        
        return info;
    }

    inline Pixel3D applyLightPrecalculated(int r, int g, int b, const InfoLight& info, bool darken = false, bool isWall = false, float nx = 0.0f, float ny = 0.0f) {
        float lR = info.lightR;
        float lG = info.lightG;
        float lB = info.lightB;
        
        // Direcional Sun Light Mix
        float sunFactor = info.sunIntensity;
        if (isWall && info.sunIntensity > 0) {
            float NdotL = 1.0f;
            if (info.sayLightX != 0.0f || info.sayLightY != 0.0f) {
                NdotL = std::max(0.0f, nx * info.sayLightX + ny * info.sayLightY);
            } else {
                NdotL = std::max(0.0f, nx);
            }
            sunFactor *= (0.2f + 0.8f * NdotL); // Ambient 20%, Directional 80%
        }
        
        float baseSunR = r * (info.sunR / 255.0f) * sunFactor;
        float baseSunG = g * (info.sunG / 255.0f) * sunFactor;
        float baseSunB = b * (info.sunB / 255.0f) * sunFactor;

        // Point lights: Emissive (pure light) + Reflected (based on texture)
        // Scaled down to prevent severe over-exposure/clipping which caused flat white blobs ("manchas")
        float emissiveR = lR * 0.25f;
        float emissiveG = lG * 0.25f;
        float emissiveB = lB * 0.25f;
        
        float reflectedR = (r * lR) / 200.0f;
        float reflectedG = (g * lG) / 200.0f;
        float reflectedB = (b * lB) / 200.0f;

        float endR = baseSunR + emissiveR + reflectedR;
        float endG = baseSunG + emissiveG + reflectedG;
        float endB = baseSunB + emissiveB + reflectedB;

        // Limite minimo de luz ambiente para nao ficar no breu absoluto
        endR = std::max(endR, r * 0.12f);
        endG = std::max(endG, g * 0.12f);
        endB = std::max(endB, b * 0.12f);

        float nf = info.fogPercentage;
        endR = endR * (1.0f - nf) + info.fogR * nf;
        endG = endG * (1.0f - nf) + info.fogG * nf;
        endB = endB * (1.0f - nf) + info.fogB * nf;

        if (darken) { endR *= 0.65f; endG *= 0.65f; endB *= 0.65f; }

        Pixel3D px;
        px.r = (uint8_t)std::min(255.0f, std::max(0.0f, endR));
        px.g = (uint8_t)std::min(255.0f, std::max(0.0f, endG));
        px.b = (uint8_t)std::min(255.0f, std::max(0.0f, endB));
        px.ch = ' ';
        return px;
    }

    inline Pixel3D applyFog(int r, int g, int b, float distance, float depthMaximum,
                                int themeSky, const std::vector<std::tuple<int, int, int>>& lights,
                                float hitX, float hitY, bool darken = false, const std::vector<std::string>* mapMatrix = nullptr,
                                bool isWall = false, float nx = 0.0f, float ny = 0.0f, float timeAnimation = 0.0f) {
        InfoLight info = calculateInfoLight(distance, depthMaximum, themeSky, lights, hitX, hitY, mapMatrix, timeAnimation);
        return applyLightPrecalculated(r, g, b, info, darken, isWall, nx, ny);
    }

}
