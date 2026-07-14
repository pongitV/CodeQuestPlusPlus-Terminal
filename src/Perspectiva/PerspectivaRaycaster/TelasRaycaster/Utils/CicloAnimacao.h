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
#include "../../../../Sistemas/ConfiguracaoTerminal/ControleInputOutput/ControleDeInput.h"

namespace MenuRaycasterUtils {

    inline float s_cicloCelestial = 0.0f;
    inline float s_velocidadeCelestial = 0.005f;
    inline int s_estrelasX = 0;
    inline int s_guerreiroPassos = 0;

    inline int s_estadoCena = 0;
    inline int s_combatTick = 0;
    inline int s_guerreiroX = -10;
    inline int s_magoX = -25;
    inline int s_arqueiroX = -40;
    inline int s_goblin1X = 0;
    inline int s_goblin2X = 0;
    inline int s_orkX = 0;
    inline bool s_goblin1Vivo = false;
    inline bool s_goblin2Vivo = false;
    inline bool s_orkVivo = false;
    inline int s_goblin1HitTimer = 0;
    inline int s_goblin2HitTimer = 0;
    inline int s_orkHitTimer = 0;
    inline int s_warriorOffset = 0;
    inline int s_archerProjX = 0;
    inline int s_laserTimer = 0;
    inline int s_healTimer = 0;

    inline void incrementarCicloDia() {
        s_cicloCelestial += s_velocidadeCelestial;
        if (s_cicloCelestial >= 4.6f) s_cicloCelestial -= 4.6f;
        s_estrelasX = (s_estrelasX + 1) % 2000;
        
        int largura = Aparencia::obterLarguraTerminal();
        if (largura <= 0) largura = 120;
        int centro = largura / 2;
        
        if (s_estadoCena == 0) {
            if (s_combatTick == 0) {
                s_guerreiroX = -40; s_magoX = -55; s_arqueiroX = -70;
                s_goblin1X = largura + 40; s_goblin2X = largura + 55; s_orkX = largura + 70;
                s_goblin1Vivo = true; s_goblin2Vivo = true; s_orkVivo = true;
                s_warriorOffset = 0; s_archerProjX = 0; s_laserTimer = 0; s_healTimer = 0;
            }
            s_combatTick++;
            s_guerreiroX += 2; s_magoX += 2; s_arqueiroX += 2;
            s_goblin1X -= 2; s_goblin2X -= 2; s_orkX -= 2;
            
            if (s_guerreiroX >= centro - 5) {
                s_guerreiroX = centro - 5; s_magoX = centro - 20; s_arqueiroX = centro - 35;
                s_goblin1X = centro + 5; s_goblin2X = centro + 20; s_orkX = centro + 35;
                s_estadoCena = 1; 
                s_combatTick = 0;
            }
        } 
        else if (s_estadoCena == 1) {
            s_combatTick++;
            int t = s_combatTick;
            
            if (t < 30) { 
                if (t == 5) { s_warriorOffset = s_goblin1X - s_guerreiroX - 10; s_goblin1HitTimer = 5; } 
                if (t == 10) { s_warriorOffset = 0; }
                if (t == 20) { s_warriorOffset = s_goblin1X - s_guerreiroX - 10; s_goblin1HitTimer = 5; } 
                if (t == 25) { s_warriorOffset = 0; s_goblin1Vivo = false; } 
            }
            else if (t < 60) { 
                if (t == 35) { s_archerProjX = s_arqueiroX + 10; } 
                if (t > 35 && t < 40) s_archerProjX = s_arqueiroX + 10 + (s_goblin2X - s_arqueiroX - 10) * (t - 35) / 5;
                if (t == 40) { s_archerProjX = 0; s_goblin2HitTimer = 5; } 
                
                if (t == 50) { s_archerProjX = s_arqueiroX + 10; } 
                if (t > 50 && t < 55) s_archerProjX = s_arqueiroX + 10 + (s_goblin2X - s_arqueiroX - 10) * (t - 50) / 5;
                if (t == 55) { s_archerProjX = 0; s_goblin2HitTimer = 5; s_goblin2Vivo = false; } 
            }
            else if (t < 90) { 
                if (t == 65) s_healTimer = 20;
            }
            else if (t < 120) { 
                if (t == 95) { s_warriorOffset = s_orkX - s_guerreiroX - 10; s_orkHitTimer = 5; } 
                if (t == 100) { s_warriorOffset = 0; }
                if (t == 105) { s_archerProjX = s_arqueiroX + 10; }
                if (t > 105 && t < 110) s_archerProjX = s_arqueiroX + 10 + (s_orkX - s_arqueiroX - 10) * (t - 105) / 5;
                if (t == 110) { s_archerProjX = 0; s_orkHitTimer = 5; } 
            }
            else if (t < 150) { 
                if (t == 125) { s_laserTimer = 15; s_orkHitTimer = 15; } 
                if (t == 140) { s_orkVivo = false; } 
            }
            else {
                s_estadoCena = 2; 
            }
        }
        else if (s_estadoCena == 2) {
            s_guerreiroX += 2; s_magoX += 2; s_arqueiroX += 2;
            if (s_arqueiroX > largura + 10) {
                s_estadoCena = 0; 
                s_combatTick = 0;
            }
        }

        if (s_goblin1HitTimer > 0) s_goblin1HitTimer--;
        if (s_goblin2HitTimer > 0) s_goblin2HitTimer--;
        if (s_orkHitTimer > 0) s_orkHitTimer--;
        if (s_laserTimer > 0) s_laserTimer--;
        if (s_healTimer > 0) s_healTimer--;

        s_guerreiroPassos++;
    }

    inline void aplicarCicloDiaNoite(std::vector<std::string>& frame) {
        if (frame.empty()) return;

        int larguraTela = Aparencia::obterLarguraTerminal();
        if (larguraTela <= 0) larguraTela = 120;

        int totalLinhas = (int)frame.size();
        int alturaCeu = totalLinhas;

        float FOV = 1.6f;

        static float s_tempoMenuAnimacao = 30.0f; // Comeca de dia

        int deltaX = 0;
        if (ControleDeInput::lerEstadoArrastoHorizontalMouse(deltaX)) {
            if (deltaX != 0) {
                s_tempoMenuAnimacao += (float)deltaX * 0.4f; 
            }
        } else {
            s_tempoMenuAnimacao += 0.05f; // Auto-passar do tempo lento
        }

        while (s_tempoMenuAnimacao < 0.0f) s_tempoMenuAnimacao += 120.0f;
        while (s_tempoMenuAnimacao > 120.0f) s_tempoMenuAnimacao -= 120.0f;

        for (int y = 0; y < totalLinhas; ++y) {
            std::string novaLinha;
            novaLinha.reserve(larguraTela * 28);

            for (int cellX = 0; cellX < larguraTela; ++cellX) {
                float colAng = ((float)cellX / (float)larguraTela - 0.5f) * FOV;
                
                Pixel3D pxCeu = RaycasterMundo::obterPixelTeto(0, colAng, colAng, y, alturaCeu * 2, s_tempoMenuAnimacao, true);

                int r = pxCeu.r;
                int g = pxCeu.g;
                int b = pxCeu.b;

                if (y >= totalLinhas * 2 / 3) {
                    float campoY = (float)(y - totalLinhas * 2 / 3) / (float)(totalLinhas / 3);
                    
                    // Grass texture with smooth organic variation
                    float noise = std::sin(cellX * 0.15f + y * 0.1f) + std::sin(y * 0.1f - cellX * 0.05f);
                    float detail = std::sin(cellX * 0.8f) * std::sin(y * 0.8f);
                    
                    int baseR = 25 + (int)(campoY * 35);
                    int baseG = 80 + (int)(campoY * 55);
                    int baseB = 15 + (int)(campoY * 25);
                    
                    r = baseR + (int)(noise * 5) + (int)(detail * 4);
                    g = baseG + (int)(noise * 8) + (int)(detail * 6);
                    b = baseB + (int)(noise * 5) + (int)(detail * 4);

                    // Flower distribution (scattered evenly)
                    unsigned int fX = (unsigned int)(cellX * 7 + 13) * 374761393U;
                    unsigned int fY = (unsigned int)(y * 11 + 7) * 668265263U;
                    unsigned int fHash = fX + fY;
                    fHash = (fHash ^ (fHash >> 13)) * 1274126177U;
                    int flor = fHash % 1000;
                    
                    if (flor < 8) {
                        r = 235; g = 40; b = 40; // Red flowers
                    } else if (flor < 16) {
                        r = 255; g = 220; b = 50; // Yellow flowers
                    } else if (flor < 22) {
                        r = 240; g = 240; b = 255; // White flowers
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

                novaLinha += "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
                
                if (y < totalLinhas * 2 / 3 && pxCeu.ch != ' ') {
                    if (pxCeu.hasFg) {
                        novaLinha += "\033[38;2;" + std::to_string(pxCeu.fgR) + ";" + std::to_string(pxCeu.fgG) + ";" + std::to_string(pxCeu.fgB) + "m";
                    } else {
                        novaLinha += "\033[38;2;255;255;255m";
                    }
                    novaLinha += pxCeu.ch;
                } else {
                    novaLinha += " ";
                }
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
        pintarHeroiNoFrame(yBase, xCentro, ArtesRaycaster::casteloMenu, 110, 115, 125);
    }

    inline void desenharCenaBatalha() {
        if (s_fundo3DMenu.empty()) return;
        int altura = (int)s_fundo3DMenu.size();
        int yBase = altura * 2 / 3 + 4;
        if (yBase + 4 > altura) return;

        if (s_orkVivo) {
            pintarInimigoNoFrame(yBase - 1, s_orkX, ArtesRaycaster::orkArte, 50, 180, 50, s_orkHitTimer > 0);
        }
        if (s_goblin2Vivo) {
            pintarInimigoNoFrame(yBase, s_goblin2X, ArtesRaycaster::goblinArte, 40, 160, 40, s_goblin2HitTimer > 0);
        }
        if (s_goblin1Vivo) {
            pintarInimigoNoFrame(yBase, s_goblin1X, ArtesRaycaster::goblinArte, 40, 160, 40, s_goblin1HitTimer > 0);
        }

        pintarHeroiNoFrame(yBase, s_arqueiroX, ArtesRaycaster::arqueiroArte, 100, 180, 100);
        pintarHeroiNoFrame(yBase, s_magoX, ArtesRaycaster::magoArte, 120, 100, 210);
        
        if (s_warriorOffset > 0) {
            pintarHeroiNoFrame(yBase, s_guerreiroX + s_warriorOffset, ArtesRaycaster::guerreiroAtaqueArte, 190, 195, 210);
        } else {
            pintarHeroiNoFrame(yBase, s_guerreiroX, ArtesRaycaster::guerreiroArte, 190, 195, 210);
        }

        if (s_archerProjX > 0) {
            std::string trail = (s_combatTick % 2 == 0) ? "~~" : " -";
            pintarEfeitoNoFrame(yBase + 2, s_archerProjX, trail + " \\", 200, 200, 200);
            pintarEfeitoNoFrame(yBase + 3, s_archerProjX, trail + "===>", 255, 255, 255);
            pintarEfeitoNoFrame(yBase + 4, s_archerProjX, trail + " /", 200, 200, 200);
        }

        if (s_healTimer > 0) {
            pintarEfeitoNoFrame(yBase - 1 - (s_healTimer % 3), s_guerreiroX + 2, "+", 50, 255, 50);
            pintarEfeitoNoFrame(yBase - 2 - (s_healTimer % 4), s_guerreiroX + 6, "+", 50, 255, 50);
            pintarEfeitoNoFrame(yBase - 1 - (s_healTimer % 3), s_arqueiroX + 2, "+", 50, 255, 50);
            pintarEfeitoNoFrame(yBase - 2 - (s_healTimer % 4), s_arqueiroX + 6, "+", 50, 255, 50);
            pintarEfeitoNoFrame(yBase - 1 - (s_healTimer % 3), s_magoX + 2, "+", 50, 255, 50);
            pintarEfeitoNoFrame(yBase - 2 - (s_healTimer % 4), s_magoX + 6, "+", 50, 255, 50);
        }

        if (s_laserTimer > 0) {
            int laserStartY = yBase + 3;
            int laserStartX = s_magoX + 8;
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
                pintarEfeitoNoFrame(laserStartY - 1, laserStartX, laserStr1, 0, 255, 255); 
                pintarEfeitoNoFrame(laserStartY, laserStartX, laserStr2, 200, 255, 255); 
                pintarEfeitoNoFrame(laserStartY + 1, laserStartX, laserStr3, 0, 255, 255);
            }
        }
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
        desenharCenaBatalha();
        sobreporLogoCodeQuest();
        out << "\033[H";
        for (size_t y = 0; y < s_fundo3DMenu.size(); ++y) {
            out << s_fundo3DMenu[y];
            if (y < s_fundo3DMenu.size() - 1) out << "\n";
        }
    }

}
