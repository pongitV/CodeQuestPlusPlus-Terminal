#include "GerenciadorTexturas.h"
#include <cmath>
#include <algorithm>

bool GerenciadorTexturas::inicializado = false;
CorRGB GerenciadorTexturas::cache[256][4096];
float GerenciadorTexturas::tabelaSin[4096];

float GerenciadorTexturas::fastSin(float angle) {
    // angle in radians. Normalize to 0-2PI
    constexpr float TWO_PI = 2.0f * 3.14159265f;
    float a = std::fmod(angle, TWO_PI);
    if (a < 0) a += TWO_PI;
    int index = (int)((a / TWO_PI) * 4096.0f) % 4096;
    return tabelaSin[index];
}

float GerenciadorTexturas::fastCos(float angle) {
    return fastSin(angle + 1.57079632f); // angle + PI/2
}

void GerenciadorTexturas::inicializar() {
    if (inicializado) return;

    // Gerar tabelas trigonométricas
    for (int i = 0; i < 4096; i++) {
        tabelaSin[i] = std::sin((float)i / 4096.0f * 2.0f * 3.14159265f);
    }

    // Gerar Texturas
    for (int i = 1; i <= static_cast<int>(TexID::TetoIndoorsPadrao); i++) {
        gerar(static_cast<TexID>(i));
    }

    inicializado = true;
}

void GerenciadorTexturas::gerar(TexID id) {
    int index = static_cast<int>(id);
    for (int ty = 0; ty < 64; ty++) {
        for (int tx = 0; tx < 64; tx++) {
            uint8_t r = 0, g = 0, b = 0;

            switch (id) {
                case TexID::LabirintoMadeira: {
                    bool isWoodBase = (ty > 54);
                    bool isWoodenPillar = (tx % 32 < 4);
                    bool isWoodenFrameX = (tx % 16 < 2);
                    bool isWoodenFrameY = (ty % 16 < 2);
                    if (isWoodBase || isWoodenPillar || isWoodenFrameX || isWoodenFrameY) {
                        r = 70; g = 40; b = 20;
                    } else {
                        r = 235; g = 220; b = 190;
                    }
                    break;
                }
                case TexID::LabirintoArcoPilar: {
                    int shadow = (tx % 32 < 8 || tx % 32 > 24) ? -20 : 20;
                    r = std::clamp(130 + shadow, 0, 255);
                    g = std::clamp(130 + shadow, 0, 255);
                    b = std::clamp(120 + shadow, 0, 255);
                    bool isRachadura = ((tx * 13 + ty * 7) % 31) < 2 || ((tx * 5 + ty * 11) % 47) < 2;
                    if (isRachadura) { r = 40; g = 40; b = 40; }
                    break;
                }
                case TexID::LabirintoArcoFundo: {
                    bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
                    if (isJunta) {
                        r = 60; g = 60; b = 60;
                    } else {
                        bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                        if (hasGrain) { r = 100; g = 100; b = 100; } 
                        else { r = 115; g = 115; b = 115; }
                    }
                    break;
                }
                case TexID::MorganaMadeira: {
                    bool isTabua = (tx % 10 == 0);
                    if (isTabua) { r = 65; g = 35; b = 15; } 
                    else { 
                        bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                        if (hasGrain) { r = 90; g = 45; b = 20; } else { r = 75; g = 38; b = 18; }
                    }
                    if (ty >= 50 && ty <= 60) {
                        int posCogumeloX = ((ty * 13) % 64);
                        if (std::abs(tx - posCogumeloX) < 3) {
                            if (ty < 54) { 
                                r = 150; g = 30; b = 150; 
                                if ((tx + ty) % 2 == 0) { r = 200; g = 100; b = 255; }
                            } else { 
                                r = 180; g = 180; b = 150;
                            }
                        }
                    }
                    break;
                }
                case TexID::IgrejaVitral: {
                    bool isVitral = (tx % 32 >= 8 && tx % 32 <= 24 && ty >= 12 && ty <= 52);
                    if (isVitral) {
                        int padrao = (tx / 4 + ty / 4) % 4;
                        if (padrao == 0) { r = 230; g = 30; b = 30; }
                        else if (padrao == 1) { r = 30; g = 120; b = 230; }
                        else if (padrao == 2) { r = 230; g = 200; b = 20; }
                        else { r = 30; g = 200; b = 100; }
                    } else {
                        r = 210; g = 205; b = 195; 
                    }
                    break;
                }
                case TexID::PonteMadeira:
                case TexID::ReinoMadeira: {
                    bool isTabua = (tx % 8 == 0); 
                    if (isTabua) { r = 45; g = 25; b = 10; } 
                    else {
                        bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                        if (hasGrain) { r = 80; g = 50; b = 20; } 
                        else { r = 101; g = 67; b = 33; }
                    }
                    break;
                }
                case TexID::Alquimista: {
                    bool isPrateleira = (ty == 12 || ty == 28 || ty == 44);
                    if (isPrateleira) {
                        r = 80; g = 40; b = 15;
                    } else {
                        int vidroCol = (tx % 12);
                        if (vidroCol >= 3 && vidroCol <= 8 && ((ty % 16) > 4 && (ty % 16) < 12)) {
                            int corPocao = (tx / 12) % 3;
                            if (corPocao == 0) { r = 0; g = 220; b = 255; }
                            else if (corPocao == 1) { r = 255; g = 0; b = 128; }
                            else { r = 50; g = 255; b = 50; }
                        } else {
                            r = 40; g = 30; b = 20;
                        }
                    }
                    break;
                }
                case TexID::EntradaIgreja: {
                    if (tx < 8 || tx > 55) {
                        r = 210; g = 205; b = 195;
                    } else if (ty < 16) {
                        float arcX = (tx - 32.0f) / 24.0f;
                        float arcY = (ty - 16.0f) / 16.0f;
                        if (arcX*arcX + arcY*arcY > 1.0f) {
                            r = 190; g = 185; b = 175; 
                        } else {
                            int padrao = (tx / 4 + ty / 4) % 3;
                            if (padrao == 0) { r = 230; g = 30; b = 30; }
                            else if (padrao == 1) { r = 30; g = 120; b = 230; }
                            else { r = 230; g = 200; b = 20; }
                        }
                    } else {
                        if (tx >= 31 && tx <= 32) { r = 20; g = 10; b = 5; } 
                        else if (ty >= 38 && ty <= 42 && (tx == 28 || tx == 36)) { r = 218; g = 165; b = 32; }
                        else {
                            bool isVerticalLine = (tx % 4 == 0);
                            if (isVerticalLine) { r = 40; g = 20; b = 10; } 
                            else { r = 60; g = 30; b = 15; }
                        }
                    }
                    break;
                }
                case TexID::ManequimAnok: {
                    bool isVerticalJoint = (tx == 0 || tx == 63);
                    bool isHorizontalJoint = (ty % 16 == 0);
                    if (isVerticalJoint || isHorizontalJoint) {
                        r = 50; g = 30; b = 15;
                    } else {
                        int grain = (tx * 3 + ty * 7) % 8;
                        r = 110 - grain * 2; g = 75 - grain; b = 45;
                    }
                    float dx = (tx - 31.5f) / 4.0f; float dy = (ty - 10.5f) / 5.0f;
                    if (dx*dx + dy*dy <= 1.0f) { r = 222; g = 184; b = 135; }
                    else if (tx >= 30 && tx <= 33 && ty >= 13 && ty <= 15) { r = 202; g = 164; b = 115; }
                    else if (ty >= 16 && ty <= 45) {
                        int larguraVestido = (ty <= 22) ? 12 - (ty - 16) : ((ty <= 30) ? 6 + (ty - 22) / 2 : 10 + (ty - 30) / 2);
                        if (tx >= 32 - larguraVestido && tx <= 32 + larguraVestido) {
                            if (ty >= 28 && ty <= 30) { r = 218; g = 165; b = 32; } 
                            else if (ty >= 16 && ty <= 18 && tx >= 27 && tx <= 37) { r = 218; g = 165; b = 32; }
                            else {
                                int dobra = (tx % 6 < 3) ? 20 : 0;
                                r = std::clamp(160 + dobra - (ty - 16), 0, 255); g = 20; b = 40;
                            }
                        }
                    }
                    else if (tx >= 30 && tx <= 33 && ty > 45 && ty <= 56) { r = 80; g = 80; b = 80; }
                    else if (ty > 56 && ty <= 62) {
                        int largBase = (ty - 56) * 3;
                        if (tx >= 32 - largBase && tx <= 32 + largBase) { r = 50; g = 25; b = 10; }
                    }
                    break;
                }
                case TexID::Franchesco: {
                    bool isPrateleira = (ty == 16 || ty == 32 || ty == 48);
                    if (isPrateleira) { r = 60; g = 35; b = 15; } 
                    else if (ty > 16 && ty < 24 && (tx % 16 > 2 && tx % 16 < 14)) { 
                        r = 120; g = 40; b = 40; 
                        if (tx % 4 == 0) { r = 200; g = 180; b = 120; } 
                    } else if (ty > 32 && ty < 40 && (tx % 12 > 2 && tx % 12 < 10)) { 
                        r = 160; g = 140; b = 100; 
                        if (ty < 35) { r = 100; g = 80; b = 50; } 
                    } else if (ty > 48 && ty < 56 && (tx % 20 > 4 && tx % 20 < 16)) { 
                        r = 100; g = 60; b = 20; 
                        if (tx % 20 < 6 || tx % 20 > 14 || ty == 52) { r = 80; g = 80; b = 80; } 
                    } else {
                        bool isTabua = (tx % 8 == 0); 
                        if (isTabua) { r = 40; g = 20; b = 10; } else { r = 50; g = 30; b = 15; }
                    }
                    break;
                }
                case TexID::Bjorn: {
                    bool isTabua = (tx % 16 == 0); 
                    if (isTabua) { r = 35; g = 20; b = 10; } else { r = 45; g = 25; b = 15; }
                    if (tx >= 8 && tx <= 12 && ty >= 10 && ty <= 40) {
                        if (tx == 10 && ty >= 10 && ty <= 30) { r = 190; g = 195; b = 200; } 
                        else if (ty >= 30 && ty <= 32 && tx >= 8 && tx <= 12) { r = 150; g = 130; b = 40; } 
                        else if (tx == 10 && ty > 32 && ty <= 38) { r = 80; g = 40; b = 10; } 
                        else if (tx == 10 && ty > 38 && ty <= 40) { r = 150; g = 130; b = 40; }
                    } else if (tx >= 52 && tx <= 58 && ty >= 15 && ty <= 45) {
                        if (tx == 55 && ty >= 15 && ty <= 45) { r = 80; g = 50; b = 20; } 
                        else if (ty >= 18 && ty <= 26 && tx >= 52 && tx <= 58) {
                            if (tx < 55) { r = 180; g = 180; b = 185; } 
                        }
                    } else if (ty >= 10 && ty <= 22 && tx >= 20 && tx <= 44) {
                        float dx = (tx - 32.0f) / 12.0f; float dy = (ty - 16.0f) / 6.0f;
                        if (dx*dx + dy*dy > 0.8f && dx*dx + dy*dy < 1.2f && ty < 16) { r = 120; g = 70; b = 30; }
                        if (ty == 16 && tx >= 20 && tx <= 44) { r = 200; g = 200; b = 200; }
                    } else if (tx >= 22 && tx <= 42 && ty >= 48 && ty <= 62) {
                        if (ty >= 48 && ty <= 52 && tx >= 22 && tx <= 42) { r = 70; g = 70; b = 75; } 
                        else if (ty > 52 && ty <= 58 && tx >= 28 && tx <= 36) { r = 60; g = 60; b = 65; } 
                        else if (ty > 58 && ty <= 62 && tx >= 26 && tx <= 38) { r = 50; g = 50; b = 55; }
                    }
                    break;
                }
                case TexID::IgrejaAltar: {
                    r = 255; g = 215; b = 0;
                    if ((tx + ty) % 4 == 0) { r = 255; g = 240; b = 150; }
                    break;
                }
                case TexID::IgrejaParede: {
                    bool isVitral = (tx % 32 >= 8 && tx % 32 <= 24 && ty >= 12 && ty <= 52);
                    if (isVitral) {
                        int padrao = (tx / 4 + ty / 4) % 4;
                        if (padrao == 0) { r = 230; g = 30; b = 30; }
                        else if (padrao == 1) { r = 30; g = 120; b = 230; }
                        else if (padrao == 2) { r = 230; g = 200; b = 20; }
                        else { r = 30; g = 200; b = 100; }
                    } else { r = 80; g = 80; b = 85; }
                    break;
                }
                case TexID::PatioMuro: {
                    bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
                    if (isJunta) { r = 60; g = 60; b = 60; } 
                    else {
                        bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                        if (hasGrain) { r = 100; g = 100; b = 100; } else { r = 120; g = 120; b = 120; }
                    }
                    break;
                }
                case TexID::FlorestaEstrutura: {
                    bool isTabua = (tx % 8 == 0); 
                    if (isTabua) { r = 45; g = 25; b = 10; } 
                    else {
                        bool hasGrain = ((tx * 3 + ty * 7) % 5) == 0;
                        if (hasGrain) { r = 80; g = 50; b = 20; } else { r = 101; g = 67; b = 33; }
                    }
                    bool isCogumelo = ((tx * 11 + ty * 13) % 47) < 2;
                    if (isCogumelo) { r = 255; g = 50; b = 50; } 
                    else if (((tx * 17 + ty * 19) % 53) < 2) { r = 200; g = 200; b = 255; }
                    break;
                }
                case TexID::PadraoEstrutura: {
                    bool isJunta = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
                    if (isJunta) { r = 120; g = 120; b = 120; } 
                    else {
                        bool hasGrain = ((tx * 7 + ty * 13) % 10) < 3;
                        if (hasGrain) { r = 140; g = 50; b = 30; } else { r = 160; g = 60; b = 40; }
                    }
                    break;
                }
                case TexID::ArvoreCoracao: {
                    float cx = (tx - 32.0f); float cy = (ty - 32.0f);
                    float dist = std::sqrt(cx*cx + cy*cy);
                    float angle = std::atan2(cy, cx);
                    float spiral = std::sin(dist * 0.2f + angle * 4.0f + tx * 0.1f);
                    bool hasMoss = ((tx * 17 + ty * 13) % 100) < 20 || (spiral > 0.8f);
                    if (hasMoss) { r = 30; g = 80; b = 20; } 
                    else if (spiral > 0.0f) { r = 50; g = 30; b = 15; } 
                    else { r = 25; g = 15; b = 10; }
                    break;
                }
                case TexID::ArvoreFloresta: {
                    int limiteFolhas = 28 + ((tx * 7) % 10);
                    if (ty < limiteFolhas) {
                        bool sombraFolha = ((tx * 7 + ty * 13) % 11) < 4; 
                        if (sombraFolha) { r = 15; g = 65; b = 15; } else { r = 25; g = 95; b = 25; }
                    } else {
                        bool isBordaEscura = (tx < 6 || tx > 57);
                        bool isSombra = (tx >= 6 && tx <= 12) || (tx >= 51 && tx <= 57);
                        bool hasWoodGrain = ((tx * 3 + ty * 7) % 5) == 0;
                        if (isBordaEscura) { r = 15; g = 10; b = 5; } 
                        else if (isSombra) { r = 40; g = 25; b = 10; } 
                        else if (hasWoodGrain) { r = 80; g = 55; b = 30; } 
                        else { r = 100; g = 65; b = 35; }
                    }
                    break;
                }
                case TexID::PedraVila: {
                    bool isJuntaPedra = ((tx * 3 + ty * 7) % 9) < 2 || ((tx * 11 + ty * 5) % 13) < 2;
                    if (isJuntaPedra) { r = 30; g = 30; b = 30; } 
                    else {
                        bool hasGrain = ((tx * 17 + ty * 23) % 7) < 3;
                        if (hasGrain) { r = 55; g = 50; b = 45; } else { r = 75; g = 70; b = 65; }
                    }
                    break;
                }
                case TexID::PedraSpawn: {
                    bool isJuntaBranca = (ty % 4 == 0) || (((ty / 4) % 2 == 0) && tx % 8 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
                    if (isJuntaBranca) { r = 140; g = 140; b = 140; } 
                    else {
                        bool hasGrain = ((tx * 11 + ty * 17) % 10) < 3;
                        if (hasGrain) { r = 210; g = 210; b = 210; } else { r = 240; g = 240; b = 240; }
                    }
                    break;
                }
                case TexID::SalaChefeParede: {
                    float cx = (tx - 32.0f); float cy = (ty - 32.0f);
                    float dist = std::sqrt(cx*cx + cy*cy);
                    float angle = std::atan2(cy, cx);
                    float spiral = std::sin(dist * 0.5f - angle * 3.0f);
                    if (spiral > 0.0f) { r = 50; g = 50; b = 50; } else { r = 15; g = 15; b = 15; }
                    break;
                }
                case TexID::CavernaCoracaoParede: {
                    bool isTerra = ((tx * 7 + ty * 13) % 10) < 4;
                    bool isMusgoDenso = ((tx * 19 + ty * 23) % 15) < 5;
                    if (isMusgoDenso) { r = 25; g = 70; b = 20; } 
                    else if (isTerra) { r = 60; g = 35; b = 15; } 
                    else { r = 45; g = 90; b = 30; }
                    break;
                }
                
                // --- CHÃO E TETOS ---
                case TexID::ChaoLabirintoBorda: {
                    r = 40; g = 40; b = 30; break;
                }
                case TexID::ChaoLabirinto: {
                    if (((tx + ty) & 1) == 0) { r = 180; g = 160; b = 110; } else { r = 160; g = 140; b = 95; }
                    break;
                }
                case TexID::ChaoSalaChefeDentro:
                case TexID::ChaoSalaChefeFora: {
                    r = 5; g = 5; b = 5; break;
                }
                case TexID::ChaoCoracaoMusgo: {
                    r = 30; g = 80; b = 20; break;
                }
                case TexID::ChaoCoracaoTerra: {
                    r = 50; g = 30; b = 15; break;
                }
                case TexID::ChaoCoracaoEscuro: {
                    r = 25; g = 15; b = 10; break;
                }
                case TexID::ChaoGramaFloresta: {
                    if (((tx + ty) & 1) == 0) { r = 4; g = 25; b = 4; }
                    else if (((tx * 3 + ty * 5) & 7) < 2) { r = 2; g = 15; b = 2; }
                    else { r = 6; g = 30; b = 6; }
                    break;
                }
                case TexID::ChaoGramaVila: {
                    if (((tx + ty) & 1) == 0) { r = 10; g = 60; b = 10; }
                    else if (((tx * 3 + ty * 5) & 7) < 2) { r = 8; g = 45; b = 8; }
                    else { r = 15; g = 80; b = 15; }
                    break;
                }
                case TexID::ChaoTerra: {
                    if (((tx + ty) & 1) == 0) { r = 28; g = 18; b = 8; }
                    else if (((tx * 3 + ty * 5) & 7) < 2) { r = 22; g = 12; b = 4; }
                    else { r = 25; g = 15; b = 5; }
                    break;
                }
                case TexID::ChaoPadrao: {
                    if (((tx + ty) & 1) == 0) { r = 24; g = 24; b = 24; }
                    else if (((tx * 3 + ty * 5) & 7) < 2) { r = 16; g = 16; b = 16; }
                    else { r = 20; g = 20; b = 20; }
                    break;
                }
                case TexID::TetoIndoorsCoracaoMusgo: {
                    r = 30; g = 80; b = 20; break;
                }
                case TexID::TetoIndoorsCoracaoMadeira: {
                    r = 50; g = 30; b = 15; break;
                }
                case TexID::TetoIndoorsCoracaoEscuro: {
                    r = 25; g = 15; b = 10; break;
                }
                case TexID::TetoIndoorsPadrao: {
                    bool isJunta = (ty % 10 == 0) || (tx % 8 == 0);
                    if (isJunta) { r = 30; g = 15; b = 5; } else { r = 45; g = 25; b = 10; }
                    break;
                }

                default:
                    r = 255; g = 0; b = 255; // Magenta for missing textures
                    break;
            }

            cache[index][ty * 64 + tx] = { r, g, b };
        }
    }
}
