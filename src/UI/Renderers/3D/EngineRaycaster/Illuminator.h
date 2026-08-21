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
        if (!mapMatrix || mapMatrix->empty()) return false;
        
        float dx = endX - startX;
        float dy = endY - startY;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist < 0.01f) return false;
        
        float stepSize = 0.2f;
        float stepX = (dx / dist) * stepSize;
        float stepY = (dy / dist) * stepSize;
        int numSteps = (int)(dist / stepSize);
        
        float cx = startX;
        float cy = startY;
        int startX_int = (int)startX;
        int startY_int = (int)startY;

        for (int i = 0; i <= numSteps; i++) {
            int cx_int = (int)cx;
            int cy_int = (int)cy;
            
            if (cx_int != startX_int || cy_int != startY_int) {
                if (cy_int >= 0 && cy_int < (int)mapMatrix->size() && cx_int >= 0 && cx_int < (int)(*mapMatrix)[0].size()) {
                    char c = (*mapMatrix)[cy_int][cx_int];
                    if (c != '.' && c != ' ' && c != '~' && c != '^' && c != 'P' && c != 'F' && c != 'B' && c != 'A' && c != 'Q' && c != 'M' && 
                        c != 'T' && c != 'G' && c != 'O' && c != 'S' && c != 'C' && c != 'I' && c != 'Y' && c != 'Z' && c != 'V' && c != 'W' && c != 'N') {
                        return true;
                    }
                }
            }
            cx += stepX;
            cy += stepY;
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
    static thread_local int shadowFrameMap[512][512] = {0};
    static thread_local bool shadowBoolMap[512][512] = {0};
    static thread_local int currentShadowFrame = 1;
    static thread_local float lastSunAngle = -1.0f;

    inline InfoLight calculateInfoLight(float distance, float depthMaximum, int themeSky,
                                   const std::vector<std::tuple<int, int, int>>& lights,
                                   float hitX, float hitY, const std::vector<std::string>* mapMatrix = nullptr, float timeAnimation = 0.0f) {
        InfoLight info;
        const float invMaxDepth = 1.0f / (depthMaximum * 0.8f);
        const float normDist = distance * invMaxDepth;
        info.fogPercentage = std::min(1.0f, normDist * normDist);

        StateClimate climateDynamic = {0,0,0,0, 0,0,0};
        float t = 0.0f;
        if (themeSky == 3) {
            // [PT-BR] Ambiente interno estatico: iluminacao constante e suave
            // [EN-US] Static indoor environment: constant, dim ambient light
            climateDynamic = { 80.0f, 80.0f, 95.0f, 0.4f, 5, 5, 10 };
        } else {
            // [PT-BR] Ciclo dinamico de horario do dia (120 segundos)
            // [EN-US] Dynamic time of day cycle (120 seconds)
            t = std::fmod(timeAnimation, 120.0f) * (1.0f / 120.0f);
            StateClimate sunBirth = { 255.0f, 180.0f, 100.0f, 0.8f, 200, 120, 80 };
            StateClimate day      = { 255.0f, 255.0f, 240.0f, 1.0f, 120, 180, 255 };
            StateClimate sunSet   = { 255.0f, 140.0f, 80.0f,  0.8f, 200, 100, 50 };
            StateClimate night    = { 80.0f,  100.0f, 255.0f, 0.4f, 10,  10,  30 };
            
            if (t < 0.1f)      climateDynamic = mixClimate(sunBirth, day, t * 10.0f);
            else if (t < 0.4f) climateDynamic = day;
            else if (t < 0.5f) climateDynamic = mixClimate(day, sunSet, (t - 0.4f) * 10.0f);
            else if (t < 0.6f) climateDynamic = mixClimate(sunSet, night, (t - 0.5f) * 10.0f);
            else if (t < 0.9f) climateDynamic = night;
            else               climateDynamic = mixClimate(night, sunBirth, (t - 0.9f) * 10.0f);
        }

        info.fogR = climateDynamic.fogR;
        info.fogG = climateDynamic.fogG;
        info.fogB = climateDynamic.fogB;

        float globalRotation = timeAnimation * 0.05f;
        float sayLightX = 0.0f, sayLightY = 0.0f;
        if (themeSky != 3) {
            if (t > 0.55f && t < 0.95f) {
                // [PT-BR] Noite: a lua domina a direcao da luz
                // [EN-US] Night: moon dominates light direction
                sayLightX = -std::cos(globalRotation);
                sayLightY = -std::sin(globalRotation);
            } else {
                // [PT-BR] Dia: o sol domina a direcao da luz
                // [EN-US] Day: sun dominates light direction
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
                        if (type == 0) { info.lightR += 220 * intensity; info.lightG += 120 * intensity; info.lightB += 30 * intensity; }
                        else if (type == 1) { info.lightR += 220 * intensity; info.lightG += 220 * intensity; info.lightB += 255 * intensity; }
                        else if (type == 2) { info.lightR += 160 * intensity; info.lightG += 40 * intensity; info.lightB += 40 * intensity; }
                        else if (type == 3) { info.lightR += 150 * intensity; info.lightG += 130 * intensity; info.lightB += 60 * intensity; }
                    }
                }
            }
        }
        
        // [PT-BR] Calculo de luz direcional e sombras do sol/lua
        // [EN-US] Directional light and shadow calculation for sun/moon
        if (mapMatrix != nullptr) {
            bool inShadow = false;
            if (themeSky != 3) {
                if (sayLightX != lastSunAngle) {
                    lastSunAngle = sayLightX;
                    currentShadowFrame++;
                }
                
                int mx = (int)(hitX * 4.0f);
                int my = (int)(hitY * 4.0f);
                
                if (mx >= 0 && mx < 512 && my >= 0 && my < 512) {
                    if (shadowFrameMap[my][mx] != currentShadowFrame) {
                        float originX = (mx + 0.5f) / 4.0f;
                        float originY = (my + 0.5f) / 4.0f;
                        inShadow = checkOcclusion(originX, originY, originX + sayLightX * 2.5f, originY + sayLightY * 2.5f, mapMatrix);
                        shadowBoolMap[my][mx] = inShadow;
                        shadowFrameMap[my][mx] = currentShadowFrame;
                    } else {
                        inShadow = shadowBoolMap[my][mx];
                    }
                } else {
                    inShadow = checkOcclusion(hitX, hitY, hitX + sayLightX * 2.5f, hitY + sayLightY * 2.5f, mapMatrix);
                }
            }
            if (!inShadow) {
                info.sunR = climateDynamic.sunR;
                info.sunG = climateDynamic.sunG;
                info.sunB = climateDynamic.sunB;
                info.sunIntensity = climateDynamic.sunIntensity;
            } else {
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
        
        // [PT-BR] Mistura de luz solar direcional
        // [EN-US] Directional sunlight blend
        float sunFactor = info.sunIntensity;
        if (isWall && info.sunIntensity > 0) {
            float NdotL = 1.0f;
            if (info.sayLightX != 0.0f || info.sayLightY != 0.0f) {
                NdotL = std::max(0.0f, nx * info.sayLightX + ny * info.sayLightY);
            } else {
                NdotL = std::max(0.0f, nx);
            }
            sunFactor *= (0.2f + 0.8f * NdotL);
        }
        
        float baseSunR = r * (info.sunR / 255.0f) * sunFactor;
        float baseSunG = g * (info.sunG / 255.0f) * sunFactor;
        float baseSunB = b * (info.sunB / 255.0f) * sunFactor;

        // [PT-BR] Luzes pontuais: componente emissiva e refletida
        // [EN-US] Point lights: emissive and reflected components
        float emissiveR = lR * 0.25f;
        float emissiveG = lG * 0.25f;
        float emissiveB = lB * 0.25f;
        
        float reflectedR = (r * lR) / 200.0f;
        float reflectedG = (g * lG) / 200.0f;
        float reflectedB = (b * lB) / 200.0f;

        float endR = baseSunR + emissiveR + reflectedR;
        float endG = baseSunG + emissiveG + reflectedG;
        float endB = baseSunB + emissiveB + reflectedB;

        // [PT-BR] Limite minimo de luz ambiente para evitar escuridao total
        // [EN-US] Minimum ambient light threshold to prevent total darkness
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
