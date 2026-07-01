#include "RaycasterHUD.h"
#include "RaycasterMundo.h"
#include "../../../Visoes/TelasBase/Combate/TelaCombate.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Mundo/ControleMapa.h"
#include <cmath>

using namespace std;

void RaycasterHUD::desenhar(vector<string>& tela, int larguraTela, int alturaTela, float jogadorX, float jogadorY, float anguloVisao, const vector<string>& matrizDoMapa, const string& tituloMapa, bool temaFloresta, Personagem* jogador) {
    desenharBarraStatus(tela, larguraTela, alturaTela, jogador);
    desenharControles(tela, larguraTela, alturaTela);
}

void RaycasterHUD::desenharMinimapa(vector<string>& tela, int LARGURA_TELA, int ALTURA_TELA, float jogadorX, float jogadorY, float anguloVisao, const vector<string>& matrizDoMapa, const string& tituloMapa, bool /*temaFloresta*/, char iconeDoJogador, const string& corJogadorAnsi) {
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    // HUD: Mini-mapa sobreposto no topo-esquerdo
    int larguraMiniMapa = 31; // Aumentado a largura para visao panoramica horizontal
    int alturaMiniMapa = 15;  // Mantem a altura original
    int offsetX = 2;
    int offsetY = 1; // Topo

    // 1. Desenha a borda do mini-mapa
    string corBorda = "\033[38;2;255;255;255m"; // Branco
    string bgMini = "\033[48;2;25;25;25m"; // Fundo pseudo-transparente cinza escuro
    for (int y = offsetY - 1; y <= offsetY + alturaMiniMapa; y++) {
        for (int x = offsetX - 1; x <= offsetX + larguraMiniMapa; x++) {
            if (x >= 0 && x < LARGURA_TELA && y >= 0 && y < ALTURA_TELA) {
                string prefixo = bgMini + corBorda;
                if (y == offsetY - 1 && x == offsetX - 1) tela[y * LARGURA_TELA + x] = prefixo + "╔\033[0m";
                else if (y == offsetY - 1 && x == offsetX + larguraMiniMapa) tela[y * LARGURA_TELA + x] = prefixo + "╗\033[0m";
                else if (y == offsetY + alturaMiniMapa && x == offsetX - 1) tela[y * LARGURA_TELA + x] = prefixo + "╚\033[0m";
                else if (y == offsetY + alturaMiniMapa && x == offsetX + larguraMiniMapa) tela[y * LARGURA_TELA + x] = prefixo + "╝\033[0m";
                else if (y == offsetY - 1 || y == offsetY + alturaMiniMapa) tela[y * LARGURA_TELA + x] = prefixo + "═\033[0m";
                else if (x == offsetX - 1 || x == offsetX + larguraMiniMapa) tela[y * LARGURA_TELA + x] = prefixo + "║\033[0m";
            }
        }
    }

    // 2. Desenha o conteudo do mini-mapa SEM rotacao
    float anguloNorm = fmod(anguloVisao, 2.0f * 3.14159f);
    if (anguloNorm < 0) anguloNorm += 2.0f * 3.14159f;
    
    string direcaoArrow = "►"; // 0 radianos aponta para o Leste (+X)
    int dirX = 1, dirY = 0;
    if (anguloNorm >= 0.785f && anguloNorm < 2.356f) { direcaoArrow = "▼"; dirX = 0; dirY = 1; }
    else if (anguloNorm >= 2.356f && anguloNorm < 3.926f) { direcaoArrow = "◄"; dirX = -1; dirY = 0; }
    else if (anguloNorm >= 3.926f && anguloNorm < 5.497f) { direcaoArrow = "▲"; dirX = 0; dirY = -1; }

    for (int my = 0; my < alturaMiniMapa; my++) {
        for (int mx = 0; mx < larguraMiniMapa; mx++) {
            int mapX = (int)jogadorX + (mx - larguraMiniMapa/2);
            int mapY = (int)jogadorY + (my - alturaMiniMapa/2);
            
            int screenY = my + offsetY;
            int screenX = mx + offsetX;
            
            if (mapX >= 0 && mapX < larguraMapa && mapY >= 0 && mapY < alturaMapa) {
                char c = matrizDoMapa[mapY][mapX];
                
                // bool isLabel = RaycasterMundo::isMapLabel(mapX, mapY, matrizDoMapa);
                
                if (mx == larguraMiniMapa/2 && my == alturaMiniMapa/2) {
                    tela[screenY * LARGURA_TELA + screenX] = bgMini + corJogadorAnsi + string(1, iconeDoJogador) + "\033[0m"; // Jogador
                } else if (mx == larguraMiniMapa/2 + dirX && my == alturaMiniMapa/2 + dirY) {
                    tela[screenY * LARGURA_TELA + screenX] = bgMini + "\033[1;38;2;255;255;255m" + direcaoArrow + "\033[0m"; // Indicador Visao Branco
                } else {
                    tela[screenY * LARGURA_TELA + screenX] = bgMini + ControleMapa::formatarCelula(c, mapX, mapY, tituloMapa, matrizDoMapa, true) + "\033[0m";
                }
            } else {
                if (screenY >= 0 && screenY < ALTURA_TELA && screenX >= 0 && screenX < LARGURA_TELA) {
                    tela[screenY * LARGURA_TELA + screenX] = bgMini + " \033[0m"; // Preenche o espaco fora dos limites do mapa
                }
            }
        }
    }
}

void RaycasterHUD::desenharBarraStatus(vector<string>& tela, int LARGURA_TELA, int ALTURA_TELA, Personagem* jogador) {
    vector<string> linhasHUD = TelaCombate::obterLinhasBarraDeStatusDoJogador(jogador, Cor::RESET, -1, 0, false);
    int hudHeight = linhasHUD.size();
    int maxHudWidth = 0;
    for (const string& linha : linhasHUD) {
        int len = Aparencia::obterComprimentoVisual(linha);
        if (len > maxHudWidth) maxHudWidth = len;
    }
    
    int boxWidth = maxHudWidth + 1; 
    int boxHeight = hudHeight + 2;
    int hudOffsetX = (LARGURA_TELA - maxHudWidth) / 2; 
    if (hudOffsetX < 0) hudOffsetX = 0;
    int hudOffsetY = ALTURA_TELA - boxHeight - 1; 

    string corHudBorda = "\033[38;2;255;255;255m"; 
    string bgHud = "\033[48;2;25;25;25m"; 

    if (hudOffsetY >= 0 && hudOffsetY < ALTURA_TELA) {
        string pixelTopo = bgHud + corHudBorda + "╔";
        for(int k=0; k < boxWidth - 2; ++k) pixelTopo += "═";
        pixelTopo += "╗\033[0m";
        tela[hudOffsetY * LARGURA_TELA + hudOffsetX] = pixelTopo;
        for(int x = 1; x < boxWidth; ++x) {
            if (hudOffsetX + x < LARGURA_TELA) tela[hudOffsetY * LARGURA_TELA + hudOffsetX + x] = "";
        }
    }

    if (hudOffsetY + boxHeight - 1 >= 0 && hudOffsetY + boxHeight - 1 < ALTURA_TELA) {
        string pixelBase = bgHud + corHudBorda + "╚";
        for(int k=0; k < boxWidth - 2; ++k) pixelBase += "═";
        pixelBase += "╝\033[0m";
        tela[(hudOffsetY + boxHeight - 1) * LARGURA_TELA + hudOffsetX] = pixelBase;
        for(int x = 1; x < boxWidth; ++x) {
            if (hudOffsetX + x < LARGURA_TELA) tela[(hudOffsetY + boxHeight - 1) * LARGURA_TELA + hudOffsetX + x] = "";
        }
    }

    for (size_t i = 0; i < linhasHUD.size(); ++i) {
        string linha = linhasHUD[i];
        size_t pos = 0;
        while ((pos = linha.find("\033[0m", pos)) != string::npos) {
            linha.replace(pos, 4, "\033[0m" + bgHud);
            pos += 4 + bgHud.length(); 
        }
        
        size_t posEsq = linha.find("║");
        if (posEsq != string::npos) linha.replace(posEsq, 3, bgHud + corHudBorda + "║" + bgHud);
        else linha = bgHud + linha;

        int len = Aparencia::obterComprimentoVisual(linha);
        int pad = maxHudWidth - len;
        if (pad > 0) linha += string(pad, ' ');

        linha += corHudBorda + "║\033[0m";

        int y = hudOffsetY + 1 + i;
        if (y >= 0 && y < ALTURA_TELA) {
            tela[y * LARGURA_TELA + hudOffsetX] = linha;
            for(int x = 1; x < boxWidth; ++x) {
                if (hudOffsetX + x < LARGURA_TELA) tela[y * LARGURA_TELA + hudOffsetX + x] = "";
            }
        }
    }
}

void RaycasterHUD::desenharControles(vector<string>& tela, int LARGURA_TELA, int ALTURA_TELA) {
    string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    int startCtrlX = (LARGURA_TELA - static_cast<int>(textoDeControles.length())) / 2;
    int linhaControles = ALTURA_TELA - 1;
    
    for (int x = 0; x < LARGURA_TELA; x++) {
        tela[linhaControles * LARGURA_TELA + x] = "\033[48;2;20;20;20m \033[0m"; 
    }
    if (startCtrlX > 0) {
        for (size_t i = 0; i < textoDeControles.length(); ++i) {
            tela[linhaControles * LARGURA_TELA + startCtrlX + i] = "\033[38;2;150;150;150m\033[48;2;20;20;20m" + string(1, textoDeControles[i]) + "\033[0m";
        }
    }
}
