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
#include "RaycasterHUD.h"
#include "RaycasterRenderer.h"
#include <map>
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../Mundo/Vila/Mapa1VilaLayout.h"
#include "../Mundo/Floresta/Mapa2FlorestaLayout.h"
#include "../Mundo/Reino/Mapa3ReinoLayout.h"

using namespace std;

float Raycaster::sensibilidadeX = 0.0008f; // Original: 0.002f (40%)
float Raycaster::sensibilidadeY = 0.048f;  // Original: 0.08f (60%)

#ifdef _WIN32
struct MouseHider {
    bool isHidden;
    MouseHider() : isHidden(false) {}
    void hide() {
        if (!isHidden) {
            BYTE ANDmaskCursor[] = { 0xFF };
            BYTE XORmaskCursor[] = { 0x00 };
            HCURSOR hCursor1 = CreateCursor(NULL, 0, 0, 1, 1, ANDmaskCursor, XORmaskCursor);
            SetSystemCursor(hCursor1, 32512); // OCR_NORMAL
            HCURSOR hCursor2 = CreateCursor(NULL, 0, 0, 1, 1, ANDmaskCursor, XORmaskCursor);
            SetSystemCursor(hCursor2, 32513); // OCR_IBEAM
            isHidden = true;
        }
    }
    void show() {
        if (isHidden) {
            SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
            isHidden = false;
        }
    }
    ~MouseHider() {
        show();
    }
};
#endif

char Raycaster::iniciarExploracao3D(const vector<string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const string& tituloMapa, Personagem* jogador, int& outHitX, int& outHitY, int tipoAnimacaoEntrada) {
    outHitX = -1;
    outHitY = -1;
    if (matrizDoMapa.empty() || !jogador) return 0;

    int larguraMapa = matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();

    bool temaFloresta = RaycasterMundo::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterMundo::obterTemaCeu(tituloMapa);

    // Obtem a resolucao dinamica do terminal para preencher a tela inteira
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();

    // Fallback de seguranca caso nao consiga ler a resolucao
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;
    
    map<char, SpriteCache> cacheSprites;
    RaycasterInimigos::inicializarSprites(cacheSprites);
    RaycasterNPCs::inicializarSprites(cacheSprites);



    float profundidadeMaxima = 150.0f;  // Profundidade infinita/maxima do mapa
    float velocidadeMovimento = 5.0f;


    Aparencia::limparTela();
    cout << "\033[?25l"; // Oculta o cursor piscante

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    auto tempoInicio = chrono::system_clock::now();
    float bobbingTime = 0.0f;
    float bobbingAmplitude = 0.0f;
    float pitchOffset = 0.0f;

    int ALTURA_INTERNA = ALTURA_TELA * 2;
    vector<string> tela3D(LARGURA_TELA * ALTURA_INTERNA, " ");
    vector<string> tela(LARGURA_TELA * ALTURA_TELA, " ");

    auto downsampleTela = [&]() {
        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
        std::vector<std::thread> threads;
        int chunk = ALTURA_TELA / numThreads;
        if (chunk == 0) chunk = 1;

        for (unsigned int t = 0; t < numThreads; ++t) {
            int startY = t * chunk;
            int endY = (t == numThreads - 1) ? ALTURA_TELA : (t + 1) * chunk;
            if (startY >= ALTURA_TELA) break;

            threads.emplace_back([&, startY, endY]() {
                for (int y = startY; y < endY; y++) {
                    for (int x = 0; x < LARGURA_TELA; x++) {
                        string top = tela3D[(y * 2) * LARGURA_TELA + x];
                        string bot = tela3D[(y * 2 + 1) * LARGURA_TELA + x];
                        
                        auto getBg = [](const string& s) {
                            size_t pos = s.find("\033[48;2;");
                            if (pos != string::npos) {
                                size_t end = s.find('m', pos);
                                if (end != string::npos) return s.substr(pos, end - pos + 1);
                            }
                            return string("\033[48;2;0;0;0m");
                        };

                        auto getChar = [](const string& s) {
                            size_t firstM = s.find('m');
                            if (firstM != string::npos && firstM + 1 < s.size()) {
                                char c = s[firstM + 1];
                                if (c != '\033' && c != ' ') return c;
                                if (c == '\033') {
                                    size_t secondM = s.find('m', firstM + 1);
                                    if (secondM != string::npos && secondM + 1 < s.size()) {
                                        c = s[secondM + 1];
                                        if (c != '\033' && c != ' ') return c;
                                    }
                                }
                            }
                            return ' ';
                        };

                        char topC = getChar(top);
                        char botC = getChar(bot);
                        
                        if (topC == ' ' && botC == ' ') {
                            string bgTop = getBg(top);
                            string bgBot = getBg(bot);
                            if (bgTop.size() > 3) bgTop[2] = '3'; // Transforma 48 (bg) em 38 (fg) trocando o '4' pelo '3' no indice 2
                            tela[y * LARGURA_TELA + x] = bgBot + bgTop + "\xE2\x96\x80\033[0m"; // ▀ (Half-Block superior)
                        } else if (topC != ' ') {
                            tela[y * LARGURA_TELA + x] = top;
                        } else {
                            tela[y * LARGURA_TELA + x] = bot;
                        }
                    }
                }
            });
        }
        for (auto& th : threads) th.join();
    };

    auto animarOlho = [&](bool abrindo, const vector<string>& frameBase) {
        int maxPassos = 8; 
        for (int passo = 0; passo <= maxPassos; passo++) {
            int p = abrindo ? passo : (maxPassos - passo);
            float aberturaPercent = (float)p / maxPassos;
            
            string bufferFrame = "\033[?25l\033[H";
            bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15);
            int centroY = ALTURA_TELA / 2;
            int centroX = LARGURA_TELA / 2;
            
            for (int y = 0; y < ALTURA_TELA; y++) {
                for (int x = 0; x < LARGURA_TELA; x++) {
                    if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break;
                    
                    float dx = std::abs(x - centroX);
                    float rx = dx / (float)centroX; 
                    
                    float heightAtX = -1.0f;
                    if (aberturaPercent > 0.0f) {
                        heightAtX = (ALTURA_TELA * 0.6f * (1.0f - rx*rx) + ALTURA_TELA * 0.5f) * aberturaPercent; 
                    }
                    
                    if (std::abs(y - centroY) > heightAtX) {
                        bufferFrame += "\033[40m \033[0m";
                    } else {
                        bufferFrame += frameBase[y * LARGURA_TELA + x];
                    }
                }
                if (y < ALTURA_TELA - 1) bufferFrame += "\n";
            }
            cout << bufferFrame << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    
    
    auto animarPortaAbrindo = [&]() {
        int maxPassos = 11;
        for (int passo = 0; passo <= maxPassos; passo++) {
            float percent = (float)passo / maxPassos;
            string bufferFrame = "\033[?25l\033[H";
            bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15);
            
            int portaLeftOffset = (int)(LARGURA_TELA * percent); 
            
            for (int y = 0; y < ALTURA_TELA; y++) {
                for (int x = 0; x < LARGURA_TELA; x++) {
                    if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break;
                    
                    if (x < LARGURA_TELA - portaLeftOffset) {
                        bufferFrame += "\033[48;2;10;10;10m \033[0m";
                    } else {
                        bufferFrame += tela[y * LARGURA_TELA + x];
                    }
                }
                if (y < ALTURA_TELA - 1) bufferFrame += "\n";
            }
            cout << bufferFrame << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(22));
        }
    };

    auto animarBanner3D = [&](const string& titulo) {
        vector<string> banner;
        string upper = titulo;
        for(char& c : upper) c = toupper((unsigned char)c);
        
        if (upper.find("VILA") != string::npos) banner = Mapa1VilaLayouts::obterLogoVila();
        else if (upper.find("INICIO") != string::npos) banner = Mapa1VilaLayouts::obterLogoSpawn();
        else if (upper.find("FLORESTA") != string::npos) banner = Mapa2FlorestaLayouts::obterLogoFloresta();
        else if (upper.find("REINO") != string::npos) banner = Mapa3ReinoLayouts::obterLogoReino();
        else {
            banner = {
                "==================================",
                "   " + titulo,
                "=================================="
            };
        }
        
        for (auto& l : banner) {
            l = Aparencia::removerCoresANSI(l);
        }
        
        int bannerWidth = 0;
        for (const auto& l : banner) {
            int w = Aparencia::obterComprimentoVisual(l);
            if (w > bannerWidth) bannerWidth = w;
        }
        
        int startX = (LARGURA_TELA - bannerWidth) / 2;
        if (startX < 0) startX = 0;
        
        int maxPassos = 10;
        for (int passo = 0; passo <= maxPassos; passo++) {
            std::vector<std::string> frameAtual = tela;
            int bannerY = (passo * (ALTURA_TELA / 4)) / maxPassos; 
            
            for (size_t i = 0; i < banner.size(); i++) {
                int drawY = bannerY + i;
                if (drawY < ALTURA_TELA) {
                    int curX = startX;
                    string line = banner[i];
                    for (size_t j = 0; j < line.length(); ) {
                        int len = 1;
                        unsigned char c = line[j];
                        if ((c & 0x80) == 0) len = 1;
                        else if ((c & 0xE0) == 0xC0) len = 2;
                        else if ((c & 0xF0) == 0xE0) len = 3;
                        else if ((c & 0xF8) == 0xF0) len = 4;
                        
                        string charStr = line.substr(j, len);
                        if (charStr != " " && curX >= 0 && curX < LARGURA_TELA) {
                            frameAtual[drawY * LARGURA_TELA + curX] = "\033[1;37m" + charStr + "\033[0m"; // White
                        }
                        curX++;
                        j += len;
                    }
                }
            }
            
            string bufferFrame = "\033[?25l\033[H";
            bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15);
            for (int y = 0; y < ALTURA_TELA; y++) {
                for (int x = 0; x < LARGURA_TELA; x++) {
                    if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break;
                    bufferFrame += frameAtual[y * LARGURA_TELA + x];
                }
                if (y < ALTURA_TELA - 1) bufferFrame += "\n";
            }
            std::cout << bufferFrame << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, profundidadeMaxima, 0.0f, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);
    downsampleTela();
    RaycasterHUD::desenhar(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);
    if (tipoAnimacaoEntrada == 1) {
        animarOlho(true, tela);
    } else if (tipoAnimacaoEntrada == 2) {
        animarPortaAbrindo();
        animarBanner3D(tituloMapa);
    }

#ifdef _WIN32
    // Aguarda o jogador soltar a tecla 'V' antes de iniciar o loop para nao fechar no mesmo instante
    while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif


    bool primeiraIteracaoMouse = true;
    bool rodando = true;
#ifdef _WIN32
    MouseHider mouseHider;
#endif
    while (rodando) {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float tempoDelta = elapsedTime.count();
        
        chrono::duration<float> diffInicio = tp2 - tempoInicio;
        float tempoAbsoluto = diffInicio.count();

        // Limitador de delta para nao "pular" paredes ou quebrar o mapa se a thread travar
        if (tempoDelta > 0.1f) tempoDelta = 0.1f;

        float oldPlayerX = jogadorX;
        float oldPlayerY = jogadorY;
        int oldCellX = (int)jogadorX;
        int oldCellY = (int)jogadorY;
        bool isMoving = false;

#ifdef _WIN32
        // --- CONTROLES ASSINCRONOS E MOUSE ---
        HWND hwnd = GetConsoleWindow();
        if (hwnd && GetForegroundWindow() == hwnd) {
            mouseHider.hide(); // Oculta 100% o cursor do mouse
            RECT rect;
            GetWindowRect(hwnd, &rect);
            int centerX = rect.left + (rect.right - rect.left) / 2;
            int centerY = rect.top + (rect.bottom - rect.top) / 2; 
            
            if (primeiraIteracaoMouse) {
                SetCursorPos(centerX, centerY);
                primeiraIteracaoMouse = false;
            } else {
                POINT p;
                if (GetCursorPos(&p)) {
                    int deltaX = p.x - centerX;
                    int deltaY = p.y - centerY;
                    
                    if (deltaX != 0 || deltaY != 0) {
                        anguloVisao += deltaX * sensibilidadeX; // Yaw (Esquerda/Direita)
                        pitchOffset -= deltaY * sensibilidadeY;  // Pitch corrigido: multiplicador drasticamente menor pois mexe com as linhas do console
                        
                        // Limitar o angulo de olhar para cima/baixo
                        float maxPitch = ALTURA_TELA * 0.7f;
                        if (pitchOffset > maxPitch) pitchOffset = maxPitch;
                        if (pitchOffset < -maxPitch) pitchOffset = -maxPitch;
                        
                        SetCursorPos(centerX, centerY); 
                    }
                }
            }
        } else {
            mouseHider.show(); // Mostra se a janela perder o foco
        }

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            ControleDeInput::limparBuffer(); // Previne buffer sujo ("dead input") ao abrir o menu
            TelaPause::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }

        if (GetAsyncKeyState('V') & 0x8000) {
            mouseHider.show();
            rodando = false;
            while (GetAsyncKeyState('V') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if (GetAsyncKeyState('I') & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            ControleDeInput::limparBuffer();
            InventarioCombate::gerenciarInventario(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('C') & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            ControleDeInput::limparBuffer();
            TelaAtributos::gerenciarFichaDoJogador(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('B') & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            ControleDeInput::limparBuffer();
            TelaDiario::exibir(jogador);
            Aparencia::limparTela();
            tp1 = chrono::system_clock::now();
        }
        if (GetAsyncKeyState('M') & 0x8000) {
            primeiraIteracaoMouse = true;
            mouseHider.show();
            while (GetAsyncKeyState('M') & 0x8000) std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ControleDeInput::limparBuffer();
            Aparencia::limparTela();
            return 'M';
        }

        // Movimento e Strafing (Com sistema de Sliding)
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (GetAsyncKeyState('W') & 0x8000) {
            isMoving = true;
            moveX += cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            moveY += sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            isMoving = true;
            moveX -= cosf(anguloVisao) * velocidadeMovimento * tempoDelta;
            moveY -= sinf(anguloVisao) * velocidadeMovimento * tempoDelta;
        }
        if (GetAsyncKeyState('A') & 0x8000) { // Strafe Esquerda (-90 graus)
            isMoving = true;
            moveX += cosf(anguloVisao - 1.5708f) * velocidadeMovimento * tempoDelta;
            moveY += sinf(anguloVisao - 1.5708f) * velocidadeMovimento * tempoDelta;
        }
        if (GetAsyncKeyState('D') & 0x8000) { // Strafe Direita (+90 graus)
            isMoving = true;
            moveX += cosf(anguloVisao + 1.5708f) * velocidadeMovimento * tempoDelta;
            moveY += sinf(anguloVisao + 1.5708f) * velocidadeMovimento * tempoDelta;
        }

        if (isMoving) {
            float novoX = jogadorX + moveX;
            float novoY = jogadorY + moveY;
            
            if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)jogadorX, (int)novoY, matrizDoMapa)) jogadorY = novoY;
            }
            if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
                if (RaycasterMundo::isWalkable((int)novoX, (int)jogadorY, matrizDoMapa)) jogadorX = novoX;
            }
        }

        // Efeito de Head Bobbing (Balanco da Camera)
        if (isMoving) {
            bobbingTime += tempoDelta * 12.0f;
            bobbingAmplitude += tempoDelta * 5.0f; // Aumenta a forca do passo
            if (bobbingAmplitude > 1.0f) bobbingAmplitude = 1.0f;
        } else {
            bobbingAmplitude -= tempoDelta * 5.0f; // Suaviza a parada em 0.2 segundos
            if (bobbingAmplitude < 0.0f) {
                bobbingAmplitude = 0.0f;
                bobbingTime = 0.0f;
            } else {
                bobbingTime += tempoDelta * 12.0f;
            }
        }
        int bobbingOffset = (int)(sinf(bobbingTime) * bobbingAmplitude * (ALTURA_TELA * 0.02f));


        // Verifica se o jogador pisou em um trigger (Inimigo ou Teleporte) para acionar a transicao de mapa/combate
        int newCellX = (int)jogadorX;
        int newCellY = (int)jogadorY;
        if (newCellX != oldCellX || newCellY != oldCellY) {
            char cell = matrizDoMapa[newCellY][newCellX];
            bool isLabel = RaycasterMundo::isMapLabel(newCellX, newCellY, matrizDoMapa);
            if (!isLabel && (RaycasterMundo::isTeleport(cell) || RaycasterMundo::isEntity(cell))) {
                outHitX = newCellX;
                outHitY = newCellY;
                jogadorX = oldPlayerX; // Retorna para a exata posicao anterior flutuante
                jogadorY = oldPlayerY;
                rodando = false; // Sai do loop 3D e devolve o controle pro mapa top-down processar o evento!
            }
        }
#else
        // Fallback previne loop infinito caso compilado fora do windows
        rodando = false; 
#endif

        // --- RENDERIZACAO RAYCASTING (3D) ---
        float horizonteInterno = (ALTURA_INTERNA / 2.0f) + (bobbingOffset * 2) + (pitchOffset * 2.0f);
        int offsetGeral = (bobbingOffset * 2) + (int)(pitchOffset * 2.0f);
        

    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, horizonteInterno, offsetGeral, profundidadeMaxima, tempoAbsoluto, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);
        downsampleTela();

        // --- RENDERIZACAO HUD E OVERLAYS (2D) ---
        RaycasterHUD::desenhar(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);

        // Envia o frame processado para o terminal de uma vez de forma linear (Zero Flickering!)
        string bufferFrame = "\033[?25l\033[H"; 
        bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15); 
        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // Pula o ultimo pixel para nao bugar o scroll automatico do terminal
                bufferFrame += tela[y * LARGURA_TELA + x];
            }
            if (y < ALTURA_TELA - 1) bufferFrame += "\n"; // Nao quebra linha na ultima fileira da tela
        }

        cout << bufferFrame << flush; // O flush empurra a string toda pro Windows de uma so vez

        // Frame Pacing dinâmico para cravar ~60 FPS reais
        auto frameEnd = chrono::system_clock::now();
        auto frameDuration = chrono::duration_cast<chrono::milliseconds>(frameEnd - tp2).count();
        int sleepTime = 16 - static_cast<int>(frameDuration);
        if (sleepTime > 0) {
            this_thread::sleep_for(chrono::milliseconds(sleepTime));
        }
    }

    // Ao apertar ESC, o loop morre, limpa o console e o controle volta para o jogo top-down padrao
    ControleDeInput::limparBuffer();
    // So limpa a tela se o fechamento foi manual (ESC). Se bateu em entidade, preserva a visao 3D como fundo pro Popup!
    if (outHitX == -1 && outHitY == -1) {
        animarOlho(false, tela);
        Aparencia::limparTela();
    }
    return 0;
}

void Raycaster::piscarTelaCor(Cor cor, int duracaoMs) {
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;

    std::string colorCode = Aparencia::bgRGB(255, 50, 50);
    if (cor == Cor::BRANCO) colorCode = Aparencia::bgRGB(255, 255, 255);
    else if (cor == Cor::VERDE) colorCode = Aparencia::bgRGB(50, 255, 50);

    std::string resetCode = "\033[0m";
    std::string blankLine = colorCode + std::string(LARGURA_TELA, ' ') + resetCode;

    std::string bufferFrame = "\033[?25l\033[H";
    for (int y = 0; y < ALTURA_TELA; y++) {
        if (y == ALTURA_TELA - 1) {
            bufferFrame += blankLine.substr(0, blankLine.length() - resetCode.length() - 1) + resetCode; 
        } else {
            bufferFrame += blankLine + "\n";
        }
    }
    std::cout << bufferFrame << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(duracaoMs));
}

std::vector<std::string> Raycaster::desenharQuadroEstatico3D(const std::vector<std::string>& matrizDoMapa, float jogadorX, float jogadorY, float anguloVisao, const std::string& tituloMapa, Personagem* jogador, int alturaOverride) {
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = (alturaOverride > 0) ? alturaOverride : Aparencia::obterAlturaTerminal();
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 40;

    bool temaFloresta = RaycasterMundo::isTemaFloresta(tituloMapa);
    int temaCeu = RaycasterMundo::obterTemaCeu(tituloMapa);

    std::map<char, SpriteCache> cacheSprites;
    RaycasterInimigos::inicializarSprites(cacheSprites);
    RaycasterNPCs::inicializarSprites(cacheSprites);

    int ALTURA_INTERNA = ALTURA_TELA * 2;
    std::vector<std::string> tela3D(LARGURA_TELA * ALTURA_INTERNA, " ");
    std::vector<std::string> tela(LARGURA_TELA * ALTURA_TELA, " ");

    RaycasterRenderer::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, 150.0f, 0.0f, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);

    for (int y = 0; y < ALTURA_TELA; y++) {
        for (int x = 0; x < LARGURA_TELA; x++) {
            std::string top = tela3D[(y * 2) * LARGURA_TELA + x];
            std::string bot = tela3D[(y * 2 + 1) * LARGURA_TELA + x];

            auto getBg = [](const std::string& s) {
                size_t pos = s.find("\033[48;2;");
                if (pos != std::string::npos) {
                    size_t end = s.find('m', pos);
                    if (end != std::string::npos) return s.substr(pos, end - pos + 1);
                }
                return std::string("\033[48;2;0;0;0m");
            };

            auto getChar = [](const std::string& s) {
                size_t firstM = s.find('m');
                if (firstM != std::string::npos && firstM + 1 < s.size()) {
                    char c = s[firstM + 1];
                    if (c != '\033' && c != ' ') return c;
                    if (c == '\033') {
                        size_t secondM = s.find('m', firstM + 1);
                        if (secondM != std::string::npos && secondM + 1 < s.size()) {
                            c = s[secondM + 1];
                            if (c != '\033' && c != ' ') return c;
                        }
                    }
                }
                return ' ';
            };

            char topC = getChar(top);
            char botC = getChar(bot);

            if (topC == ' ' && botC == ' ') {
                std::string bgTop = getBg(top);
                std::string bgBot = getBg(bot);
                if (bgTop.size() > 3) bgTop[2] = '3';
                tela[y * LARGURA_TELA + x] = bgBot + bgTop + "\xE2\x96\x80\033[0m";
            } else if (topC != ' ') {
                tela[y * LARGURA_TELA + x] = top;
            } else {
                tela[y * LARGURA_TELA + x] = bot;
            }
        }
    }

    return tela;
}
