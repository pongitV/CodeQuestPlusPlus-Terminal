#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "UI/Renderers/3D/RaycasterScreens/Utils/FrameOverlayUtils.h"
#include "UI/Renderers/3D/RaycasterScreens/Utils/MenuRaycasterLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterWorld.h"
#include "Core/Utils/Appearance.h"
#include "Core/Terminal/InputOutputControl/InputControl.h"

namespace MenuRaycasterUtils {

    inline float s_cycleHeavenly = 0.0f;
    inline float s_speedHeavenly = 0.005f;
    inline int s_starsX = 0;
    inline int s_warriorSteps = 0;

    inline int s_stateScene = 0;
    inline int s_combatTick = 0;
    inline int s_warriorX = -10;
    inline int s_wizardX = -25;
    inline int s_archerX = -40;
    inline int s_goblin1X = 0;
    inline int s_goblin2X = 0;
    inline int s_orkX = 0;
    inline bool s_goblin1Alive = false;
    inline bool s_goblin2Alive = false;
    inline bool s_orkAlive = false;
    inline int s_goblin1HitTimer = 0;
    inline int s_goblin2HitTimer = 0;
    inline int s_orkHitTimer = 0;
    inline int s_warriorOffset = 0;
    inline int s_archerProjectX = 0;
    inline int s_laserTimer = 0;
    inline int s_healTimer = 0;

    inline void increaseCycleDay() {
        s_cycleHeavenly += s_speedHeavenly;
        if (s_cycleHeavenly >= 4.6f) s_cycleHeavenly -= 4.6f;
        s_starsX = (s_starsX + 1) % 2000;
        
        int width = Appearance::getTerminalWidth();
        if (width <= 0) width = 120;
        int center = width / 2;
        
        if (s_stateScene == 0) {
            if (s_combatTick == 0) {
                s_warriorX = -40; s_wizardX = -55; s_archerX = -70;
                s_goblin1X = width + 40; s_goblin2X = width + 55; s_orkX = width + 70;
                s_goblin1Alive = true; s_goblin2Alive = true; s_orkAlive = true;
                s_warriorOffset = 0; s_archerProjectX = 0; s_laserTimer = 0; s_healTimer = 0;
            }
            s_combatTick++;
            s_warriorX += 2; s_wizardX += 2; s_archerX += 2;
            s_goblin1X -= 2; s_goblin2X -= 2; s_orkX -= 2;
            
            if (s_warriorX >= center - 5) {
                s_warriorX = center - 5; s_wizardX = center - 20; s_archerX = center - 35;
                s_goblin1X = center + 5; s_goblin2X = center + 20; s_orkX = center + 35;
                s_stateScene = 1; 
                s_combatTick = 0;
            }
        } 
        else if (s_stateScene == 1) {
            s_combatTick++;
            int t = s_combatTick;
            
            if (t < 30) { 
                if (t == 5) { s_warriorOffset = s_goblin1X - s_warriorX - 10; s_goblin1HitTimer = 5; } 
                if (t == 10) { s_warriorOffset = 0; }
                if (t == 20) { s_warriorOffset = s_goblin1X - s_warriorX - 10; s_goblin1HitTimer = 5; } 
                if (t == 25) { s_warriorOffset = 0; s_goblin1Alive = false; } 
            }
            else if (t < 60) { 
                if (t == 35) { s_archerProjectX = s_archerX + 10; } 
                if (t > 35 && t < 40) s_archerProjectX = s_archerX + 10 + (s_goblin2X - s_archerX - 10) * (t - 35) / 5;
                if (t == 40) { s_archerProjectX = 0; s_goblin2HitTimer = 5; } 
                
                if (t == 50) { s_archerProjectX = s_archerX + 10; } 
                if (t > 50 && t < 55) s_archerProjectX = s_archerX + 10 + (s_goblin2X - s_archerX - 10) * (t - 50) / 5;
                if (t == 55) { s_archerProjectX = 0; s_goblin2HitTimer = 5; s_goblin2Alive = false; } 
            }
            else if (t < 90) { 
                if (t == 65) s_healTimer = 20;
            }
            else if (t < 120) { 
                if (t == 95) { s_warriorOffset = s_orkX - s_warriorX - 10; s_orkHitTimer = 5; } 
                if (t == 100) { s_warriorOffset = 0; }
                if (t == 105) { s_archerProjectX = s_archerX + 10; }
                if (t > 105 && t < 110) s_archerProjectX = s_archerX + 10 + (s_orkX - s_archerX - 10) * (t - 105) / 5;
                if (t == 110) { s_archerProjectX = 0; s_orkHitTimer = 5; } 
            }
            else if (t < 150) { 
                if (t == 125) { s_laserTimer = 15; s_orkHitTimer = 15; } 
                if (t == 140) { s_orkAlive = false; } 
            }
            else {
                s_stateScene = 2; 
            }
        }
        else if (s_stateScene == 2) {
            s_warriorX += 2; s_wizardX += 2; s_archerX += 2;
            if (s_archerX > width + 10) {
                s_stateScene = 0; 
                s_combatTick = 0;
            }
        }

        if (s_goblin1HitTimer > 0) s_goblin1HitTimer--;
        if (s_goblin2HitTimer > 0) s_goblin2HitTimer--;
        if (s_orkHitTimer > 0) s_orkHitTimer--;
        if (s_laserTimer > 0) s_laserTimer--;
        if (s_healTimer > 0) s_healTimer--;

        s_warriorSteps++;
    }

    inline float s_sunOverrideAngle = -100.0f;
    inline float s_sunOverrideRatioY = -100.0f;
    inline float s_moonOverrideAngle = -100.0f;
    inline float s_moonOverrideRatioY = -100.0f;
    inline int s_draggingEntity = 0; // 0=none, 1=sun, 2=moon, 3=sky

    inline void applyCycleDayNight(std::vector<std::string>& frame) {
        if (frame.empty()) return;

        int widthScreen = Appearance::getTerminalWidth();
        if (widthScreen <= 0) widthScreen = 120;

        int totalLines = (int)frame.size();
        int heightSky = totalLines;
        float FOV = 1.6f;

        static float s_timeMenuAnimation = 30.0f; // Comeca de dia

        int mouseX = -1, mouseY = -1;
        bool isLeft = false, isRight = false;
        
        static int lastMouseX = -1;
        static bool lastLeft = false;

        InputControl::pollMouseState(mouseX, mouseY, isLeft, isRight);

        if (isLeft) {
            if (!lastLeft) {
                float colAng = ((float)mouseX / widthScreen - 0.5f) * FOV;
                float ratioY = (float)mouseY / heightSky;
                
                float t = std::fmod(s_timeMenuAnimation, 120.0f) / 120.0f;
                float diffAngleSun = colAng - (t - 0.25f) * 3.2f;
                float sunPhase = (t - 0.25f) * 2.0f * 3.14159f;
                float sunElevation = std::cos(sunPhase);
                float distYSun = ratioY - (0.5f - 0.2f * sunElevation);
                float distSun = std::sqrt(diffAngleSun * diffAngleSun * 6.0f + distYSun * distYSun);

                float diffAngleMoon = colAng - (t - 0.75f) * 3.2f;
                float moonPhase = (t - 0.75f) * 2.0f * 3.14159f;
                float moonElevation = std::cos(moonPhase);
                float distYMoon = ratioY - (0.5f - 0.2f * moonElevation);
                float distMoon = std::sqrt(diffAngleMoon * diffAngleMoon * 6.0f + distYMoon * distYMoon);

                if (distSun < 0.15f) s_draggingEntity = 1;
                else if (distMoon < 0.15f) s_draggingEntity = 2;
                else s_draggingEntity = 3;
                
                lastMouseX = mouseX;
            }
            
            if (s_draggingEntity == 1) {
                s_sunOverrideAngle = ((float)mouseX / widthScreen - 0.5f) * FOV;
                s_sunOverrideRatioY = (float)mouseY / heightSky;
                float target_t = (s_sunOverrideAngle / 3.2f) + 0.25f;
                s_timeMenuAnimation = target_t * 120.0f;
            } else if (s_draggingEntity == 2) {
                s_moonOverrideAngle = ((float)mouseX / widthScreen - 0.5f) * FOV;
                s_moonOverrideRatioY = (float)mouseY / heightSky;
                float target_t = (s_moonOverrideAngle / 3.2f) + 0.75f;
                s_timeMenuAnimation = target_t * 120.0f;
            } else if (s_draggingEntity == 3) {
                if (lastMouseX != -1) {
                    s_timeMenuAnimation += (mouseX - lastMouseX) * 0.4f;
                }
                lastMouseX = mouseX;
            }
        } else {
            s_draggingEntity = 0;
            s_sunOverrideAngle = -100.0f;
            s_sunOverrideRatioY = -100.0f;
            s_moonOverrideAngle = -100.0f;
            s_moonOverrideRatioY = -100.0f;
            lastMouseX = -1;
            s_timeMenuAnimation += 0.05f;
        }
        
        lastLeft = isLeft;

        while (s_timeMenuAnimation < 0.0f) s_timeMenuAnimation += 120.0f;
        while (s_timeMenuAnimation > 120.0f) s_timeMenuAnimation -= 120.0f;

        for (int y = 0; y < totalLines; ++y) {
            std::string newLine;
            newLine.reserve(widthScreen * 28);

            for (int cellX = 0; cellX < widthScreen; ++cellX) {
                float colAng = ((float)cellX / (float)widthScreen - 0.5f) * FOV;
                
                Pixel3D pxSky = RaycasterWorld::getPixelCeiling(0, colAng, colAng, y, heightSky * 2, s_timeMenuAnimation, true, s_sunOverrideAngle, s_sunOverrideRatioY, s_moonOverrideAngle, s_moonOverrideRatioY);

                int r = pxSky.r;
                int g = pxSky.g;
                int b = pxSky.b;

                if (y >= totalLines * 2 / 3) {
                    float fieldY = (float)(y - totalLines * 2 / 3) / (float)(totalLines / 3);
                    
                    // Textura de grama com variacao organica suave
                    float noise = std::sin(cellX * 0.15f + y * 0.1f) + std::sin(y * 0.1f - cellX * 0.05f);
                    float details = std::sin(cellX * 0.8f) * std::sin(y * 0.8f);
                    
                    int baseR = 25 + (int)(fieldY * 35);
                    int baseG = 80 + (int)(fieldY * 55);
                    int baseB = 15 + (int)(fieldY * 25);
                    
                    r = baseR + (int)(noise * 5) + (int)(details * 4);
                    g = baseG + (int)(noise * 8) + (int)(details * 6);
                    b = baseB + (int)(noise * 5) + (int)(details * 4);

                    // Flower distribution (scattered evenly)
                    unsigned int fX = (unsigned int)(cellX * 7 + 13) * 374761393U;
                    unsigned int fY = (unsigned int)(y * 11 + 7) * 668265263U;
                    unsigned int fHash = fX + fY;
                    fHash = (fHash ^ (fHash >> 13)) * 1274126177U;
                    int flower = fHash % 1000;
                    
                    if (flower < 8) {
                        r = 235; g = 40; b = 40; // Red flowers
                    } else if (flower < 16) {
                        r = 255; g = 220; b = 50; // Yellow flowers
                    } else if (flower < 22) {
                        r = 240; g = 240; b = 255; // White flowers
                    }
                    
                    if (fieldY < 0.08f) {
                        float blend = fieldY / 0.08f;
                        r = (int)((float)r * blend + 60.0f * (1.0f - blend));
                        g = (int)((float)g * blend + 120.0f * (1.0f - blend));
                        b = (int)((float)b * blend + 80.0f * (1.0f - blend));
                    }
                }

                r = std::max(0, std::min(255, r));
                g = std::max(0, std::min(255, g));
                b = std::max(0, std::min(255, b));

                newLine += "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
                
                if (y < totalLines * 2 / 3 && pxSky.ch != ' ') {
                    if (pxSky.hasFg) {
                        newLine += "\033[38;2;" + std::to_string(pxSky.fgR) + ";" + std::to_string(pxSky.fgG) + ";" + std::to_string(pxSky.fgB) + "m";
                    } else {
                        newLine += "\033[38;2;255;255;255m";
                    }
                    newLine += pxSky.ch;
                } else {
                    newLine += " ";
                }
                newLine += "\033[0m";
            }
            frame[y] = std::move(newLine);
        }
    }

    inline void drawCastle() {
        if (s_background3DMenu.empty()) return;
        int height = (int)s_background3DMenu.size();
        int yBase = height * 2 / 3 + 4 - 18;
        if (yBase < 0) yBase = 0;

        int width = Appearance::getTerminalWidth();
        int xCenter = std::max(0, (width - 47) / 2);
        paintHeroNoFrame(yBase, xCenter, ArtsRaycaster::castleMenu, 110, 115, 125);
    }

    inline void drawSceneBattle() {
        if (s_background3DMenu.empty()) return;
        int height = (int)s_background3DMenu.size();
        int yBase = height * 2 / 3 + 4;
        if (yBase + 4 > height) return;

        if (s_orkAlive) {
            paintEnemyNoFrame(yBase - 1, s_orkX, ArtsRaycaster::orkArt, 50, 180, 50, s_orkHitTimer > 0);
        }
        if (s_goblin2Alive) {
            paintEnemyNoFrame(yBase, s_goblin2X, ArtsRaycaster::goblinArt, 40, 160, 40, s_goblin2HitTimer > 0);
        }
        if (s_goblin1Alive) {
            paintEnemyNoFrame(yBase, s_goblin1X, ArtsRaycaster::goblinArt, 40, 160, 40, s_goblin1HitTimer > 0);
        }

        paintHeroNoFrame(yBase, s_archerX, ArtsRaycaster::archerArt, 100, 180, 100);
        paintHeroNoFrame(yBase, s_wizardX, ArtsRaycaster::wizardArt, 120, 100, 210);
        
        if (s_warriorOffset > 0) {
            paintHeroNoFrame(yBase, s_warriorX + s_warriorOffset, ArtsRaycaster::warriorAttackArt, 190, 195, 210);
        } else {
            paintHeroNoFrame(yBase, s_warriorX, ArtsRaycaster::warriorArt, 190, 195, 210);
        }

        if (s_archerProjectX > 0) {
            std::string trail = (s_combatTick % 2 == 0) ? "~~" : " -";
            paintEffectNoFrame(yBase + 2, s_archerProjectX, trail + " \\", 200, 200, 200);
            paintEffectNoFrame(yBase + 3, s_archerProjectX, trail + "===>", 255, 255, 255);
            paintEffectNoFrame(yBase + 4, s_archerProjectX, trail + " /", 200, 200, 200);
        }

        if (s_healTimer > 0) {
            paintEffectNoFrame(yBase - 1 - (s_healTimer % 3), s_warriorX + 2, "+", 50, 255, 50);
            paintEffectNoFrame(yBase - 2 - (s_healTimer % 4), s_warriorX + 6, "+", 50, 255, 50);
            paintEffectNoFrame(yBase - 1 - (s_healTimer % 3), s_archerX + 2, "+", 50, 255, 50);
            paintEffectNoFrame(yBase - 2 - (s_healTimer % 4), s_archerX + 6, "+", 50, 255, 50);
            paintEffectNoFrame(yBase - 1 - (s_healTimer % 3), s_wizardX + 2, "+", 50, 255, 50);
            paintEffectNoFrame(yBase - 2 - (s_healTimer % 4), s_wizardX + 6, "+", 50, 255, 50);
        }

        if (s_laserTimer > 0) {
            int laserStartY = yBase + 3;
            int laserStartX = s_wizardX + 8;
            int laserEndX = s_orkX + 2;
            if (laserEndX > laserStartX) {
                std::string laserStr1 = "";
                std::string laserStr2 = "";
                std::string laserStr3 = "";
                for (int i = 0; i < (laserEndX - laserStartX); ++i) {
                    if ((i + s_combatTick) % 4 == 0) {
                        laserStr1 += "\\"; laserStr2 += "="; laserStr3 += "/";
                    } else if ((i + s_combatTick) % 4 == 2) {
                        laserStr1 += "/"; laserStr2 += "="; laserStr3 += "\\";
                    } else {
                        laserStr1 += " "; laserStr2 += "="; laserStr3 += " ";
                    }
                }
                paintEffectNoFrame(laserStartY - 1, laserStartX, laserStr1, 0, 255, 255); 
                paintEffectNoFrame(laserStartY, laserStartX, laserStr2, 200, 255, 255); 
                paintEffectNoFrame(laserStartY + 1, laserStartX, laserStr3, 0, 255, 255);
            }
        }
    }

    inline void superimposeSoonCodeQuest() {
        if (s_background3DMenu.empty()) return;
        int widthConsole = Appearance::getTerminalWidth();
        if (widthConsole <= 0) return;
        std::string text = "CodeQuest";
        std::string more = "++";
        int totalWidth = Appearance::getVisualLength(text) + Appearance::getVisualLength(more);
        int marginSoon = std::max(0, (widthConsole - totalWidth) / 2);
        superimposeNoFrame(1, marginSoon, text, 255, 255, 255);
        superimposeNoFrame(1, marginSoon + (int)text.length(), more, 255, 165, 0);
    }

    inline void displayBackground3D(std::ostream& out) {
        applyCycleDayNight(s_background3DMenu);
        drawCastle();
        drawSceneBattle();
        superimposeSoonCodeQuest();
        out << "\033[H";
        for (size_t y = 0; y < s_background3DMenu.size(); ++y) {
            out << s_background3DMenu[y];
            if (y < s_background3DMenu.size() - 1) out << "\n";
        }
    }

}
