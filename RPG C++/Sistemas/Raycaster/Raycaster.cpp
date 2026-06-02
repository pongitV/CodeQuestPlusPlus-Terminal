#include "Raycaster.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Interface/Telas/Combate/TelaCombate.h"
#include "../Inventario/InventarioCombate.h"
#include "../../Interface/Telas/Atributos/TelaAtributos.h"
#include "../../Interface/Telas/Diario/TelaDiario.h"
#include "../../Interface/Telas/Pause/TelaPause.h"
#include "RaycasterSprites.h"
#include "RaycasterInimigos.h"
#include "RaycasterNPCs.h"
#include "RaycasterMundo.h"
#include <map>
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

char Raycaster::iniciarExploracao3D(const vector<string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const string& tituloMapa, Personagem* jogador) {
    if (matrizDoMapa.empty() || !jogador) return 0;

    int larguraMapa = matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();
    
    string corJogadorAnsi = Aparencia::cor(jogador->obterCorJogador());

    bool temaFloresta = RaycasterMundo::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterMundo::obterTemaCeu(tituloMapa);

    // Obtem a resolucao dinamica do terminal para preencher a tela inteira
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();

    // Fallback de seguranca caso nao consiga ler a resolucao
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;
    
    std::vector<std::string> texturaCeu(ALTURA_TELA);
    for (int y = 0; y < ALTURA_TELA; ++y) {
        texturaCeu[y] = RaycasterMundo::obterPixelTeto(temaCeu, y, ALTURA_TELA);
    }

    map<char, SpriteCache> cacheSprites;
    RaycasterInimigos::inicializarSprites(cacheSprites);
    RaycasterNPCs::inicializarSprites(cacheSprites);

    float campoVisao = 3.14159f / 4.0f; // FOV 45 graus
    float profundidadeMaxima = 20.0f;   // Profundidade da renderizacao e da neblina
    float velocidadeMovimento = 5.0f;
    float velocidadeRotacao = 2.0f;

    Aparencia::limparTela();
    cout << "\033[?25l"; // Oculta o cursor piscante

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    vector<string> tela(LARGURA_TELA * ALTURA_TELA, " ");

#ifdef _WIN32
    // Aguarda o jogador soltar a tecla 'V' antes de iniciar o loop para nao fechar no mesmo instante
    while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif

    bool rodando = true;
    while (rodando) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float tempoDelta = elapsedTime.count();

        // Limitador de delta para nao "pular" paredes ou quebrar o mapa se a thread travar
        if (tempoDelta > 0.1f) tempoDelta = 0.1f;

        int oldCellX = (int)jogadorX;
        int oldCellY = (int)jogadorY;

#ifdef _WIN32
        // --- CONTROLES ASSINCRONOS (TEMPO REAL) ---
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            TelaPause::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }

        if (GetAsyncKeyState('V') & 0x8000) {
            rodando = false;
            while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (GetAsyncKeyState('I') & 0x8000) {
            InventarioCombate::gerenciarInventario(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('C') & 0x8000) {
            TelaAtributos::gerenciarFichaDoJogador(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('B') & 0x8000) {
            TelaDiario::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('M') & 0x8000) {
            while (GetAsyncKeyState('M') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ControleDeInput::limparBuffer();
            Aparencia::limparTela();
            return 'M';
        }

        // Rotacao
        if (GetAsyncKeyState('A') & 0x8000) anguloVisao -= velocidadeRotacao * tempoDelta;
        if (GetAsyncKeyState('D') & 0x8000) anguloVisao += velocidadeRotacao * tempoDelta;

        // Movimento (Com sistema de Sliding - permite deslizar na parede ao andar em diagonal)
        if (GetAsyncKeyState('W') & 0x8000) {
            float novoX = jogadorX + cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            float novoY = jogadorY + sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
            
            if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
                if (RaycasterMundo::isWalkable(matrizDoMapa[(int)novoY][(int)jogadorX])) jogadorY = novoY;
            }
            if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
                if (RaycasterMundo::isWalkable(matrizDoMapa[(int)jogadorY][(int)novoX])) jogadorX = novoX;
            }
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            float novoX = jogadorX - cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            float novoY = jogadorY - sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
            
            if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
                if (RaycasterMundo::isWalkable(matrizDoMapa[(int)novoY][(int)jogadorX])) jogadorY = novoY;
            }
            if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
                if (RaycasterMundo::isWalkable(matrizDoMapa[(int)jogadorY][(int)novoX])) jogadorX = novoX;
            }
        }

        // Verifica se o jogador pisou em um trigger (Inimigo ou Teleporte) para acionar a transicao de mapa/combate
        int newCellX = (int)jogadorX;
        int newCellY = (int)jogadorY;
        if (newCellX != oldCellX || newCellY != oldCellY) {
            char cell = matrizDoMapa[newCellY][newCellX];
            if (RaycasterMundo::isTeleport(cell) || RaycasterMundo::isEntity(cell)) {
                rodando = false; // Sai do loop 3D e devolve o controle pro mapa top-down processar o evento!
            }
        }
#else
        // Fallback previne loop infinito caso compilado fora do windows
        rodando = false; 
#endif

        // --- RENDERIZACAO RAYCASTING ---
        for (int x = 0; x < LARGURA_TELA; x++) {
            float raioAngulo = (anguloVisao - campoVisao / 2.0f) + ((float)x / (float)LARGURA_TELA) * campoVisao;
            float distanciaAteParede = 0.0f;
            bool bateuNaParede = false;

            float olhoX = cosf(raioAngulo);
            float olhoY = sinf(raioAngulo);
            
            float distEntidade = -1.0f;
            char charEntidade = ' ';
            float texXEntidade = 0.0f;
            char charParede = '#';

            while (!bateuNaParede && distanciaAteParede < profundidadeMaxima) {
                distanciaAteParede += 0.1f;

                int testeX = (int)(jogadorX + olhoX * distanciaAteParede);
                int testeY = (int)(jogadorY + olhoY * distanciaAteParede);
                if (testeX < 0 || testeX >= larguraMapa || testeY < 0 || testeY >= alturaMapa) {
                    bateuNaParede = true;
                    distanciaAteParede = profundidadeMaxima;
                } else {
                    char c = matrizDoMapa[testeY][testeX];
                    // Para o raio ao encontrar paredes ou Inimigos/NPCs (Ignora marcadores de teleporte como ^Saida)
                    if (c != '.' && c != ' ') {
                        bool isWordAfterTeleport = false;
                        
                        // Checa pra tras se a letra atual faz parte de uma palavra como ^Saida ou ^Loja
                        for (int tx = testeX - 1; tx >= std::max(0, testeX - 10); tx--) {
                            char check = matrizDoMapa[testeY][tx];
                            if (check == '^') { isWordAfterTeleport = true; break; }
                            if (check == ' ' || check == '.') break; 
                        }
                        
                        if (!isWordAfterTeleport) {
                            bool isTeleport = (c == '^');
                            bool isEntity = RaycasterMundo::isEntity(c);
                            
                            if (isEntity || isTeleport) {
                                if (distEntidade < 0) { // Salva o primeiro inimigo/portal atingido e atravessa ele!
                                    char spriteChar = RaycasterMundo::obterSpriteChar(c, tituloMapa);

                                    float centerX = testeX + 0.5f;
                                    float centerY = testeY + 0.5f;

                                    if (spriteChar == 'H') {
                                        centerX = 54.0f;
                                        centerY = 28.5f;
                                    }

                                    float dx = centerX - jogadorX;
                                    float dy = centerY - jogadorY;
                                    
                                    float anguloProCentro = atan2(dy, dx);
                                    float diffAngulo = raioAngulo - anguloProCentro;
                                    
                                    while (diffAngulo < -3.14159f) diffAngulo += 2.0f * 3.14159f;
                                    while (diffAngulo > 3.14159f) diffAngulo -= 2.0f * 3.14159f;
                                    
                                    float distProCentro = sqrt(dx*dx + dy*dy);
                                    float larguraSpriteRadianos = atan2(0.45f, distProCentro); // Sprite de 0.9 unidades (90% do bloco)
                                    if (spriteChar == 'H') larguraSpriteRadianos = atan2(2.5f, distProCentro); // Mahoraga é Gigante (Ocupa 5 blocos)
                                    
                                    if (fabs(diffAngulo) <= larguraSpriteRadianos) {
                                        distEntidade = distProCentro;
                                        charEntidade = spriteChar;
                                        texXEntidade = (diffAngulo / larguraSpriteRadianos) * 0.5f + 0.5f; // Mapeia a textura de 0.0 a 1.0
                                    }
                                }
                            } else {
                            bateuNaParede = true;
                            charParede = c;
                        }
                    }
                }
            }
            } // Fecha o loop while(!bateuNaParede)

            int teto = (float)(ALTURA_TELA / 2.0) - ALTURA_TELA / ((float)distanciaAteParede);
            int chao = ALTURA_TELA - teto;

        string pixelChao = RaycasterMundo::obterPixelChao(temaFloresta);

            for (int y = 0; y < ALTURA_TELA; y++) {
                if (y < teto) tela[y * LARGURA_TELA + x] = texturaCeu[y];
                else if (y >= teto && y <= chao) tela[y * LARGURA_TELA + x] = RaycasterMundo::obterPixelParede(temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao);
                else tela[y * LARGURA_TELA + x] = pixelChao;
            }

            // --- DESENHA A ARTE DO INIMIGO POR CIMA DO MUNDO ---
            if (distEntidade > 0.0f) {
                int tetoEnt = (float)(ALTURA_TELA / 2.0) - ALTURA_TELA / ((float)distEntidade);
                int chaoEnt = ALTURA_TELA - tetoEnt;
                int altEnt = chaoEnt - tetoEnt;
                
                if (altEnt > 0 && cacheSprites.count(charEntidade)) {
                    auto& sc = cacheSprites[charEntidade];
                    int spriteX = (int)(texXEntidade * sc.width);
                    if (spriteX < 0) spriteX = 0;
                    if (spriteX >= sc.width) spriteX = sc.width - 1;
                    
                    for (int y = tetoEnt; y <= chaoEnt; y++) {
                        if (y >= 0 && y < ALTURA_TELA) {
                            int spriteY = ((y - tetoEnt) * sc.height) / altEnt;
                            if (spriteY >= 0 && spriteY < sc.height && spriteX < (int)sc.pixels[spriteY].size()) {
                                string p = sc.pixels[spriteY][spriteX];
                                if (p != "") tela[y * LARGURA_TELA + x] = p; // Sobrepoe o pixel se nao for transparente
                            }
                        }
                    }
                }
            }
        }

        // HUD: Mini-mapa sobreposto no topo-esquerdo
        int larguraMiniMapa = 31; // Aumentado a largura para visao panoramica horizontal
        int alturaMiniMapa = 15;  // Mantem a altura original
        int offsetX = 2;
        int offsetY = 1; // Topo

        // 1. Desenha a borda do mini-mapa
        string corBorda = "\033[38;2;150;150;150m"; // Cinza claro
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
        char iconeDoJogador = jogador->obterIconeJogador();
        
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
                    
                    bool isWordAfterTeleport = RaycasterMundo::isWordAfterTeleport(mapX, mapY, matrizDoMapa);
                    bool isEntityMini = RaycasterMundo::isEntity(c) && !isWordAfterTeleport;
                    bool isTeleportMini = RaycasterMundo::isTeleport(c);
                    bool isParedeMini = (c != '.' && c != ' ' && !(c >= 'a' && c <= 'z') && !isEntityMini && !isTeleportMini && !isWordAfterTeleport);

                    if (mx == larguraMiniMapa/2 && my == alturaMiniMapa/2) {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + corJogadorAnsi + string(1, iconeDoJogador) + "\033[0m"; // Jogador
                    } else if (mx == larguraMiniMapa/2 + dirX && my == alturaMiniMapa/2 + dirY) {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + "\033[1;38;2;255;255;255m" + direcaoArrow + "\033[0m"; // Indicador Visao Branco
                    } else if (isParedeMini) {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + (temaFloresta ? "\033[38;2;101;67;33m" + string(1, c) + "\033[0m" : "\033[38;2;200;200;200m" + string(1, c) + "\033[0m");
                    } else if (isTeleportMini) {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + "\033[38;2;255;255;50m^\033[0m"; // Mostra os teleportes em amarelo brilhante
                    } else if (isEntityMini) {
                        string corMinimapaEnt = RaycasterMundo::obterCorMinimapaEntidade(c, tituloMapa);
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + corMinimapaEnt + string(1, c) + "\033[0m";
                    } else {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + "\033[38;2;50;50;50m.\033[0m";
                    }
                } else {
                    if (screenY >= 0 && screenY < ALTURA_TELA && screenX >= 0 && screenX < LARGURA_TELA) {
                        tela[screenY * LARGURA_TELA + screenX] = bgMini + " \033[0m"; // Preenche o espaco fora dos limites do mapa
                    }
                }
            }
        }

        // 3. Importar HUD do Combate
        vector<string> linhasHUD = TelaCombate::obterLinhasBarraDeStatusDoJogador(jogador, Cor::RESET, -1, 0, false);
        int hudHeight = linhasHUD.size();
        int maxHudWidth = 0;
        for (const string& linha : linhasHUD) {
            int len = Aparencia::obterComprimentoVisual(linha);
            if (len > maxHudWidth) maxHudWidth = len;
        }
        
        int boxWidth = maxHudWidth + 1; // +1 para a borda direita
        int boxHeight = hudHeight + 2;
        int hudOffsetX = (LARGURA_TELA - maxHudWidth) / 2; // Posiciona no centro horizontal exato
        if (hudOffsetX < 0) hudOffsetX = 0;
        int hudOffsetY = ALTURA_TELA - boxHeight - 1; // Subiu 1 linha para acomodar os controles na base

        string corHudBorda = "\033[38;2;150;150;150m"; // Cinza claro
        string bgHud = "\033[48;2;25;25;25m"; // Fundo pseudo-transparente cinza escuro

        // 1. Injeta o Topo do HUD
        if (hudOffsetY >= 0 && hudOffsetY < ALTURA_TELA) {
            string pixelTopo = bgHud + corHudBorda + "╔";
            for(int k=0; k < boxWidth - 2; ++k) pixelTopo += "═";
            pixelTopo += "╗\033[0m";
            tela[hudOffsetY * LARGURA_TELA + hudOffsetX] = pixelTopo;
            
            // Esvazia as proximas celulas da tela pois a nossa string já ocupa o espaco delas
            for(int x = 1; x < boxWidth; ++x) {
                if (hudOffsetX + x < LARGURA_TELA) tela[hudOffsetY * LARGURA_TELA + hudOffsetX + x] = "";
            }
        }

        // 2. Injeta a Base do HUD
        if (hudOffsetY + boxHeight - 1 >= 0 && hudOffsetY + boxHeight - 1 < ALTURA_TELA) {
            string pixelBase = bgHud + corHudBorda + "╚";
            for(int k=0; k < boxWidth - 2; ++k) pixelBase += "═";
            pixelBase += "╝\033[0m";
            tela[(hudOffsetY + boxHeight - 1) * LARGURA_TELA + hudOffsetX] = pixelBase;
            
            for(int x = 1; x < boxWidth; ++x) {
                if (hudOffsetX + x < LARGURA_TELA) tela[(hudOffsetY + boxHeight - 1) * LARGURA_TELA + hudOffsetX + x] = "";
            }
        }

        // 3. Injeta o Meio (Linhas do HUD texturizadas)
        for (size_t i = 0; i < linhasHUD.size(); ++i) {
            string linha = linhasHUD[i];
            
            // Intercepta os resets para manter o fundo cinza
            size_t pos = 0;
            while ((pos = linha.find("\033[0m", pos)) != string::npos) {
                linha.replace(pos, 4, "\033[0m" + bgHud);
                pos += 4 + bgHud.length(); 
            }
            
            // Colore a borda lateral esquerda original "║" e adiciona cor de fundo
            size_t posEsq = linha.find("║");
            if (posEsq != string::npos) {
                linha.replace(posEsq, 3, bgHud + corHudBorda + "║" + bgHud);
            } else {
                linha = bgHud + linha;
            }

            // Preenche o fundo com espacos ate atingir a largura maxima
            int len = Aparencia::obterComprimentoVisual(linha);
            int pad = maxHudWidth - len;
            if (pad > 0) linha += string(pad, ' ');

            // Adiciona a borda direita "║" finalizando a linha no formato caixa
            linha += corHudBorda + "║\033[0m";

            int y = hudOffsetY + 1 + i;
            if (y >= 0 && y < ALTURA_TELA) {
                tela[y * LARGURA_TELA + hudOffsetX] = linha;
                for(int x = 1; x < boxWidth; ++x) {
                    if (hudOffsetX + x < LARGURA_TELA) tela[y * LARGURA_TELA + hudOffsetX + x] = "";
                }
            }
        }
        
        // 4. Controles no inferior (abaixo do HUD)
        string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
        int startCtrlX = (LARGURA_TELA - textoDeControles.length()) / 2;
        int linhaControles = ALTURA_TELA - 1;
        for (int x = 0; x < LARGURA_TELA; x++) {
            tela[linhaControles * LARGURA_TELA + x] = "\033[48;2;20;20;20m \033[0m"; // Fundo cinza escuro na ultima linha
        }
        if (startCtrlX > 0) {
            for (size_t i = 0; i < textoDeControles.length(); ++i) {
                tela[linhaControles * LARGURA_TELA + startCtrlX + i] = "\033[38;2;150;150;150m\033[48;2;20;20;20m" + string(1, textoDeControles[i]) + "\033[0m";
            }
        }

        // Envia o frame processado para o terminal de uma vez de forma linear (Zero Flickering!)
        string bufferFrame = "\033[H"; 
        bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15); 
        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // Pula o ultimo pixel para nao bugar o scroll automatico do terminal
                bufferFrame += tela[y * LARGURA_TELA + x];
            }
            if (y < ALTURA_TELA - 1) bufferFrame += "\n"; // Nao quebra linha na ultima fileira da tela
        }

        cout << bufferFrame << flush; // O flush empurra a string toda pro Windows de uma so vez

        // Limita o jogo a ~60 FPS. Evita que o console engasgue com o excesso de dados e estabiliza a fisica de andar
        this_thread::sleep_for(chrono::milliseconds(16));
    }

    // Ao apertar ESC, o loop morre, limpa o console e o controle volta para o jogo top-down padrao
    ControleDeInput::limparBuffer();
    Aparencia::limparTela();
    return 0;
}
