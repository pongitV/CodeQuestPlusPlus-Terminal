#include "Raycaster.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Combate/TelaCombate.h"
#include "../../../Sistemas/Inventario/InventarioCombate.h"
#include "../../TelasBase/Atributos/TelaAtributos.h"
#include "../../TelasBase/Diario/TelaDiario.h"
#include "../../TelasBase/Pause/TelaPause.h"
#include "RaycasterSprites.h"
#include "RaycasterInimigos.h"
#include "RaycasterNPCs.h"
#include "RaycasterMundo.h"
#include "RaycasterHUD.h"
#include "RaycasterRenderizador.h"
#include "../../../Core/Controladores/Debug.h"
#include "RaycasterControles.h"
#include <map>
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <string_view>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../../../Mapas/Vila/Mapa1VilaLayout.h"
#include "../../../Mapas/Floresta/Mapa2FlorestaLayout.h"
#include "../../../Mapas/Reino/Mapa3PonteReinoLayout.h"
#include "../../../Mapas/Reino/Mapa4ReinoLayout.h"

using namespace std;

float Raycaster::sensibilidadeX = 0.0008f; // Original: 0.002f (40%)
float Raycaster::sensibilidadeY = 0.048f;  // Original: 0.08f (60%)

std::string RaycasterQuadro::s_ultimoQuadroRenderizado;

void RaycasterQuadro::restaurarUltimoQuadro() {
    if (s_ultimoQuadroRenderizado.empty()) return;
    std::cout << "\033[0m" << s_ultimoQuadroRenderizado << std::flush;
}

static inline char* writeByteFast(char* p, uint8_t val) {
    if (val >= 100) {
        *p++ = '0' + (val / 100);
        *p++ = '0' + ((val / 10) % 10);
        *p++ = '0' + (val % 10);
    } else if (val >= 10) {
        *p++ = '0' + (val / 10);
        *p++ = '0' + (val % 10);
    } else {
        *p++ = '0' + val;
    }
    return p;
}

static inline char* writeAnsiColorFast(char* p, int type, uint8_t r, uint8_t g, uint8_t b) {
    *p++ = '\033';
    *p++ = '[';
    if (type == 38) {
        *p++ = '3';
        *p++ = '8';
    } else {
        *p++ = '4';
        *p++ = '8';
    }
    *p++ = ';';
    *p++ = '2';
    *p++ = ';';
    p = writeByteFast(p, r);
    *p++ = ';';
    p = writeByteFast(p, g);
    *p++ = ';';
    p = writeByteFast(p, b);
    *p++ = 'm';
    return p;
}

static inline void writeAnsiPixel(std::string& s, const Pixel3D& px) {
    char buf[64];
    char* p = buf;
    p = writeAnsiColorFast(p, 48, px.r, px.g, px.b);
    if (px.hasFg) {
        p = writeAnsiColorFast(p, 38, px.fgR, px.fgG, px.fgB);
    }
    *p++ = px.ch;
    s.assign(buf, p - buf);
}

static void downsampleTelaBuffer(const vector<Pixel3D>& tela3D, vector<string>& tela, int LARGURA_TELA, int ALTURA_TELA) {
    for (int y = 0; y < ALTURA_TELA; y++) {
        for (int x = 0; x < LARGURA_TELA; x++) {
            const Pixel3D& top = tela3D[(y * 2) * LARGURA_TELA + x];
            const Pixel3D& bot = tela3D[(y * 2 + 1) * LARGURA_TELA + x];
            
            std::string& combined = tela[y * LARGURA_TELA + x];
            
            if (top.ch == ' ' && bot.ch == ' ') {
                char buf[64];
                char* p = buf;
                p = writeAnsiColorFast(p, 48, bot.r, bot.g, bot.b);
                p = writeAnsiColorFast(p, 38, top.r, top.g, top.b);
                *p++ = '\xE2';
                *p++ = '\x96';
                *p++ = '\x80';
                combined.assign(buf, p - buf);
            } else if (top.ch != ' ') {
                writeAnsiPixel(combined, top);
            } else {
                writeAnsiPixel(combined, bot);
            }
        }
    }
}

char Raycaster::iniciarExploracao3D(const vector<string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const string& tituloMapa, Personagem* jogador, int& outHitX, int& outHitY, int tipoAnimacaoEntrada) {
    outHitX = -1;
    outHitY = -1;
    if (matrizDoMapa.empty() || !jogador) return 0;

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

    auto tp1 = chrono::steady_clock::now();
    auto tp2 = chrono::steady_clock::now();
    static auto tempoInicio = chrono::steady_clock::now();
    float bobbingTime = 0.0f;
    float bobbingAmplitude = 0.0f;
    float pitchOffset = 0.0f;
    int bobbingOffset = 0;

    int ALTURA_INTERNA = ALTURA_TELA * 2;
    vector<Pixel3D> tela3D(LARGURA_TELA * ALTURA_INTERNA);
    vector<string> tela(LARGURA_TELA * ALTURA_TELA, " ");

    auto downsampleTela = [&]() {
        downsampleTelaBuffer(tela3D, tela, LARGURA_TELA, ALTURA_TELA);
    };

    auto animarOlho = [&](bool abrindo, const vector<string>& frameBase) {
        int maxPassos = 8; 
        for (int passo = 0; passo <= maxPassos; passo++) {
            int p = abrindo ? passo : (maxPassos - passo);
            float aberturaPercent = (float)p / maxPassos;
            
            string bufferFrame = "\033[?2026h\033[?25l\033[H";
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
            cout << bufferFrame << "\033[?2026l" << flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    };

    
    
    auto animarPortaAbrindo = [&]() {
        int maxPassos = 11;
        for (int passo = 0; passo <= maxPassos; passo++) {
            float percent = (float)passo / maxPassos;
            string bufferFrame = "\033[?2026h\033[?25l\033[H";
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
            cout << bufferFrame << "\033[?2026l" << flush;
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
        else if (upper.find("PONTE DO REINO") != string::npos || upper.find("PONTE") != string::npos) banner = Mapa3PonteReinoLayouts::obterLogoPonteReino();
        else if (upper.find("REINO") != string::npos) banner = Mapa4ReinoLayouts::obterLogoReino();
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
        
        int bannerY = 2;
        
        // Decompoe cada linha do banner em caracteres visuais individuais
        vector<vector<string>> bannerChars(banner.size());
        for (size_t i = 0; i < banner.size(); i++) {
            for (size_t j = 0; j < banner[i].length(); ) {
                unsigned char chr = banner[i][j];
                int len = 1;
                if ((chr & 0x80) == 0) len = 1;
                else if ((chr & 0xE0) == 0xC0) len = 2;
                else if ((chr & 0xF0) == 0xE0) len = 3;
                else if ((chr & 0xF8) == 0xF0) len = 4;
                bannerChars[i].push_back(banner[i].substr(j, len));
                j += len;
            }
        }
        
        // Renderiza a cena base uma unica vez (sem banner)
        {
            string cenaInicial;
            cenaInicial.reserve(LARGURA_TELA * ALTURA_TELA * 15);
            cenaInicial += "\033[?2026h\033[?25l\033[H";
            for (int y = 0; y < ALTURA_TELA; y++) {
                for (int x = 0; x < LARGURA_TELA; x++) {
                    if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break;
                    cenaInicial += tela[y * LARGURA_TELA + x];
                }
                if (y < ALTURA_TELA - 1) cenaInicial += "\n";
            }
            std::cout << cenaInicial << "\033[?2026l" << std::flush;
        }
        
        // Fade-in: atualiza apenas as linhas do banner via posicionamento de cursor
        int maxPassos = 15;
        for (int passo = 0; passo <= maxPassos; passo++) {
            float opacity = (float)passo / maxPassos;
            int c = (int)(255 * opacity);
            string corBanner = "\033[38;2;" + to_string(c) + ";" + to_string(c) + ";" + to_string(c) + "m";
            
            std::ostringstream buffer;
            for (size_t i = 0; i < banner.size(); i++) {
                int drawY = bannerY + i;
                if (drawY >= ALTURA_TELA) continue;
                
                buffer << "\033[" << (drawY + 1) << ";1H";
                for (int x = 0; x < LARGURA_TELA; x++) {
                    int bannerCol = x - startX;
                    if (bannerCol >= 0 && bannerCol < (int)bannerChars[i].size() && bannerChars[i][bannerCol] != " ") {
                        buffer << corBanner << bannerChars[i][bannerCol] << "\033[0m";
                    } else {
                        buffer << tela[drawY * LARGURA_TELA + x];
                    }
                }
                buffer << "\033[K";
            }
            std::cout << buffer.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    int temaAtivoInicial = temaCeu;
    if (temaCeu == 1 || temaCeu == 2) {
        long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        float anguloGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
        if (anguloGlobal > 1.5707f && anguloGlobal < 4.7123f) {
            temaAtivoInicial = 1; 
        } else {
            temaAtivoInicial = 2; 
        }
    }
    
    auto tpAgora = std::chrono::steady_clock::now();
    std::chrono::duration<float> diffInicioInicial = tpAgora - tempoInicio;
    float tempoAbsolutoInicial = diffInicioInicial.count();

    RaycasterRenderizador::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, profundidadeMaxima, tempoAbsolutoInicial, matrizDoMapa, tituloMapa, temaFloresta, temaAtivoInicial, cacheSprites);
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
        tp2 = chrono::steady_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float tempoDelta = elapsedTime.count();
        
        chrono::duration<float> diffInicio = tp2 - tempoInicio;
        float tempoAbsoluto = diffInicio.count();

        // Limitador de delta para nao "pular" paredes ou quebrar o mapa se a thread travar
        if (tempoDelta > 0.1f) tempoDelta = 0.1f;

        if (jogador->obterVida() <= 0 || jogador->obterVoltarProMenu()) {
            rodando = false;
            break;
        }

        bool isMoving = false;
        char acaoRetorno = RaycasterControles::processarInputEControles(
            jogador,
            jogadorX,
            jogadorY,
            anguloVisao,
            pitchOffset,
            tempoDelta,
            velocidadeMovimento,
            matrizDoMapa,
            ALTURA_TELA,
            sensibilidadeX,
            sensibilidadeY,
            primeiraIteracaoMouse,
            outHitX,
            outHitY,
            rodando,
            tp1,
#ifdef _WIN32
            mouseHider,
#endif
            isMoving,
            bobbingTime,
            bobbingAmplitude,
            bobbingOffset
        );
        if (acaoRetorno != '\0') {
            return acaoRetorno;
        }

        // --- RENDERIZACAO RAYCASTING (3D) ---
        float horizonteInterno = (ALTURA_INTERNA / 2.0f) + (bobbingOffset * 2) + (pitchOffset * 2.0f);
        int offsetGeral = (bobbingOffset * 2) + (int)(pitchOffset * 2.0f);
        

        // --- CICLO DIA/NOITE GLOBAL ---
        int temaAtivo = temaCeu;
        if (temaCeu == 1 || temaCeu == 2) {
            long long globalMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            float anguloGlobal = ((globalMs % 60000) / 60000.0f) * 6.2831853f;
            if (anguloGlobal > 1.5707f && anguloGlobal < 4.7123f) {
                temaAtivo = 1; 
            } else {
                temaAtivo = 2; 
            }
        }

        RaycasterRenderizador::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, horizonteInterno, offsetGeral, profundidadeMaxima, tempoAbsoluto, matrizDoMapa, tituloMapa, temaFloresta, temaAtivo, cacheSprites);
        // --- LIMPA A TELA HUD ---
        for (int i = 0; i < LARGURA_TELA * ALTURA_TELA; i++) {
            tela[i] = " ";
        }

        // --- RENDERIZACAO HUD E OVERLAYS (2D) ---
        RaycasterHUD::desenhar(tela, LARGURA_TELA, ALTURA_TELA, jogadorX, jogadorY, anguloVisao, matrizDoMapa, tituloMapa, temaFloresta, jogador);

        // Envia o frame processado para o terminal de uma vez de forma linear (Zero Flickering!)
        string bufferFrame = "\033[?2026h\033[?25l\033[H"; 
        bufferFrame.reserve(LARGURA_TELA * ALTURA_TELA * 15); 

        int curBgR = -1, curBgG = -1, curBgB = -1;
        int curFgR = -1, curFgG = -1, curFgB = -1;

        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // Pula o ultimo pixel
                
                const string& hudStr = tela[y * LARGURA_TELA + x];
                if (hudStr != " ") {
                    bufferFrame += hudStr;
                    curBgR = -1; curBgG = -1; curBgB = -1;
                    curFgR = -1; curFgG = -1; curFgB = -1;
                } else {
                    const Pixel3D& top = tela3D[(y * 2) * LARGURA_TELA + x];
                    const Pixel3D& bot = tela3D[(y * 2 + 1) * LARGURA_TELA + x];
                    
                    int bgR, bgG, bgB;
                    int fgR=0, fgG=0, fgB=0;
                    bool hasFg = false;
                    char ch = ' ';
                    bool isHalfBlock = false;
                    
                    if (top.ch == ' ' && bot.ch == ' ') {
                        bgR = bot.r; bgG = bot.g; bgB = bot.b;
                        fgR = top.r; fgG = top.g; fgB = top.b;
                        hasFg = true;
                        isHalfBlock = true;
                    } else if (top.ch != ' ') {
                        bgR = top.r; bgG = top.g; bgB = top.b;
                        if (top.hasFg) { fgR = top.fgR; fgG = top.fgG; fgB = top.fgB; hasFg = true; }
                        ch = top.ch;
                    } else {
                        bgR = bot.r; bgG = bot.g; bgB = bot.b;
                        if (bot.hasFg) { fgR = bot.fgR; fgG = bot.fgG; fgB = bot.fgB; hasFg = true; }
                        ch = bot.ch;
                    }
                    
                    char buf[64];
                    char* p = buf;
                    
                    if (bgR != curBgR || bgG != curBgG || bgB != curBgB) {
                        p = writeAnsiColorFast(p, 48, bgR, bgG, bgB);
                        curBgR = bgR; curBgG = bgG; curBgB = bgB;
                    }
                    if (hasFg) {
                        if (fgR != curFgR || fgG != curFgG || fgB != curFgB) {
                            p = writeAnsiColorFast(p, 38, fgR, fgG, fgB);
                            curFgR = fgR; curFgG = fgG; curFgB = fgB;
                        }
                    }
                    
                    if (isHalfBlock) {
                        *p++ = '\xE2';
                        *p++ = '\x96';
                        *p++ = '\x80';
                    } else {
                        *p++ = ch;
                    }
                    
                    bufferFrame.append(buf, p - buf);
                }
            }
            if (y < ALTURA_TELA - 1) bufferFrame += "\n";
        }
        bufferFrame += "\033[0m\033[?2026l"; // Reset final das cores ao terminar o frame e desliga atualizacao sincronizada
        s_ultimoQuadroRenderizado = bufferFrame; // <--- SALVA O QUADRO FORMATADO EM ANSI
        cout << bufferFrame << flush;

        // Frame Pacing dinâmico para cravar ~60 FPS reais
        auto frameEnd = chrono::steady_clock::now();
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
    (void)jogador;
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
    std::vector<Pixel3D> tela3D(LARGURA_TELA * ALTURA_INTERNA);
    std::vector<std::string> tela(LARGURA_TELA * ALTURA_TELA, " ");

    RaycasterRenderizador::renderizar3D(tela3D, LARGURA_TELA, ALTURA_INTERNA, jogadorX, jogadorY, anguloVisao, (ALTURA_INTERNA / 2.0f), 0, 150.0f, 0.0f, matrizDoMapa, tituloMapa, temaFloresta, temaCeu, cacheSprites);

    downsampleTelaBuffer(tela3D, tela, LARGURA_TELA, ALTURA_TELA);

    return tela;
}
