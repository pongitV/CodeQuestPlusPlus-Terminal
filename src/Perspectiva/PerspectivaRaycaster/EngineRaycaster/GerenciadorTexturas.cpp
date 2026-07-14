#include "GerenciadorTexturas.h"
#include <cmath>
#include <algorithm>

bool GerenciadorTexturas::inicializado = false;
CorRGB GerenciadorTexturas::cache[256][16384];
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
    int res = obterResolucao(id);
    for (int ty = 0; ty < res; ty++) {
        for (int tx = 0; tx < res; tx++) {
            uint8_t r = 0, g = 0, b = 0;

            switch (id) {
                case TexID::LabirintoMadeira: {
                    bool isWoodBase = (ty > 108);
                    bool isWoodenPillar = (tx % 64 < 4);
                    bool isWoodenFrameX = (tx % 32 < 2);
                    bool isWoodenFrameY = (ty % 32 < 2);
                    if (isWoodBase || isWoodenPillar || isWoodenFrameX || isWoodenFrameY) {
                        r = 70; g = 40; b = 20;
                    } else {
                        r = 235; g = 220; b = 190;
                    }
                    break;
                }
                case TexID::LabirintoArcoPilar: {
                    int shadow = (tx % 64 < 8 || tx % 64 > 24) ? -20 : 20;
                    r = std::clamp(130 + shadow, 0, 255);
                    g = std::clamp(130 + shadow, 0, 255);
                    b = std::clamp(120 + shadow, 0, 255);
                    bool isRachadura = ((tx * 13 + ty * 7) % 31) < 2 || ((tx * 5 + ty * 11) % 47) < 2;
                    if (isRachadura) { r = 40; g = 40; b = 40; }
                    break;
                }
                case TexID::LabirintoArcoFundo: {
                    bool isJunta = (ty % 8 == 0) || (((ty / 4) % 2 == 0) && tx % 16 == 0) || (((ty / 4) % 2 == 1) && (tx + 4) % 8 == 0); 
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
                    bool isTabua = (tx % 20 == 0);
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
                    bool isVitral = (tx % 64 >= 8 && tx % 64 <= 24 && ty >= 12 && ty <= 52);
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
                    bool isTabua = (tx % 16 == 0); 
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
                        int vidroCol = (tx % 24);
                        if (vidroCol >= 3 && vidroCol <= 8 && ((ty % 32) > 4 && (ty % 32) < 12)) {
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
                            bool isVerticalLine = (tx % 8 == 0);
                            if (isVerticalLine) { r = 40; g = 20; b = 10; } 
                            else { r = 60; g = 30; b = 15; }
                        }
                    }
                    break;
                }
                case TexID::ManequimAnok: {
                    bool isVerticalJoint = (tx == 0 || tx == 63);
                    bool isHorizontalJoint = (ty % 32 == 0);
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
                                int dobra = (tx % 12 < 3) ? 20 : 0;
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
                    else if (ty > 16 && ty < 24 && (tx % 32 > 2 && tx % 32 < 14)) { 
                        r = 120; g = 40; b = 40; 
                        if (tx % 8 == 0) { r = 200; g = 180; b = 120; } 
                    } else if (ty > 32 && ty < 80 && (tx % 24 > 2 && tx % 24 < 10)) { 
                        r = 160; g = 140; b = 100; 
                        if (ty < 35) { r = 100; g = 80; b = 50; } 
                    } else if (ty > 48 && ty < 56 && (tx % 40 > 4 && tx % 40 < 16)) { 
                        r = 100; g = 60; b = 20; 
                        if (tx % 40 < 6 || tx % 40 > 14 || ty == 52) { r = 80; g = 80; b = 80; } 
                    } else {
                        bool isTabua = (tx % 16 == 0); 
                        if (isTabua) { r = 40; g = 20; b = 10; } else { r = 50; g = 30; b = 15; }
                    }
                    break;
                }
                case TexID::Bjorn: {
                    bool isTabua = (tx % 32 == 0); 
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
                    bool isVitral = (tx % 64 >= 8 && tx % 64 <= 24 && ty >= 12 && ty <= 52);
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
                    float nx = tx + 1000.0f + 4.0f * GerenciadorTexturas::fastSin(ty * 0.2f) + 2.0f * GerenciadorTexturas::fastSin(tx * 0.1f);
                    float ny = ty + 1000.0f + 4.0f * GerenciadorTexturas::fastSin(tx * 0.2f) + 2.0f * GerenciadorTexturas::fastSin(ty * 0.1f);
                    
                    int stoneSize = 16;
                    int row = (int)(ny / stoneSize);
                    float offsetX = (row % 2 == 0) ? 0.0f : (stoneSize / 2.0f);
                    int col = (int)((nx + offsetX) / stoneSize);
                    
                    float localX = std::fmod(nx + offsetX, (float)stoneSize);
                    float localY = std::fmod(ny, (float)stoneSize);
                    
                    float dx = localX - (stoneSize / 2.0f);
                    float dy = localY - (stoneSize / 2.0f);
                    float dist = std::sqrt(dx*dx + dy*dy);
                    float maxDist = (stoneSize / 2.0f) * 1.1f;
                    
                    float stoneFactor = 1.0f - std::clamp((dist - (maxDist * 0.65f)) / (maxDist * 0.35f), 0.0f, 1.0f);
                    
                    int hash = (row * 31 + col * 17) % 30;
                    float noise = GerenciadorTexturas::fastSin(tx * 0.5f) + GerenciadorTexturas::fastSin(ty * 0.5f);
                    
                    int baseColor = 120 + hash + (int)(noise * 8);
                    int rejunteColor = 85 + (int)(noise * 4);
                    float lighting = 0.85f + 0.15f * (1.0f - (dist / maxDist));
                    
                    int c = (int)(rejunteColor * (1.0f - stoneFactor) + baseColor * lighting * stoneFactor);
                    r = g = b = std::clamp(c, 0, 255);
                    break;
                }
                case TexID::FlorestaEstrutura: {
                    bool isTabua = (tx % 16 == 0); 
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
                    bool isJuntaY = (ty % 8 == 0);
                    bool isJuntaX = ((ty / 8) % 2 == 0) ? (tx % 16 == 0) : ((tx + 8) % 16 == 0);
                    if (isJuntaY || isJuntaX) { 
                        r = 200; g = 135; b = 115; // Rejunte
                    } else {
                        // Tijolo realista e suave
                        int brickRow = ty / 8;
                        int brickCol = ((ty / 8) % 2 == 0) ? (tx / 16) : ((tx + 8) / 16);
                        int hash = (brickRow * 17 + brickCol * 31) % 15;
                        
                        float grain = GerenciadorTexturas::fastSin(tx * 0.3f) * GerenciadorTexturas::fastSin(ty * 0.3f);
                        int offset = hash + (int)(grain * 6.0f);
                        
                        r = std::clamp(165 + offset, 0, 255); 
                        g = std::clamp(75 + offset / 2, 0, 255); 
                        b = std::clamp(45 + offset / 2, 0, 255); 
                    }
                    break;
                }
                case TexID::ArvoreCoracao: {
                    float stx = tx * (64.0f / res);
                    float sty = ty * (64.0f / res);
                    
                    int trunkId = (int)(stx / 16.0f); 
                    float localX = std::fmod(stx, 16.0f) - 8.0f;
                    
                    float nx = localX / 8.0f;
                    float lighting = 1.0f - (nx * nx * 0.6f);
                    
                    float noiseY = GerenciadorTexturas::fastSin(sty * 0.2f);
                    float barkNoise = GerenciadorTexturas::fastSin(stx * 0.8f + noiseY * 2.0f);
                    
                    r = std::clamp((int)((80 + barkNoise * 10 + trunkId*4) * lighting), 0, 255);
                    g = std::clamp((int)((45 + barkNoise * 6 + trunkId*2) * lighting), 0, 255);
                    b = std::clamp((int)((35 + barkNoise * 6 + trunkId*2) * lighting), 0, 255);
                    
                    float canopyEdge = 25.0f + 4.0f * GerenciadorTexturas::fastSin(stx * 0.3f) + 3.0f * GerenciadorTexturas::fastSin(stx * 0.8f);
                    if (sty < canopyEdge) {
                        float leafNoise = GerenciadorTexturas::fastSin(stx * 0.6f + sty * 0.6f) + GerenciadorTexturas::fastSin(stx * 1.5f + sty * 0.5f);
                        float depth = 1.0f - (sty / canopyEdge) * 0.4f;
                        r = std::clamp((int)((70 + leafNoise * 10) * depth), 0, 255);
                        g = std::clamp((int)((35 + leafNoise * 5) * depth), 0, 255);
                        b = std::clamp((int)((25 + leafNoise * 5) * depth), 0, 255);
                    }
                    break;
                }
                case TexID::ArvoreFloresta: {
                    float stx = tx * (64.0f / res);
                    float sty = ty * (64.0f / res);
                    
                    int trunkId = (int)(stx / 16.0f); 
                    float localX = std::fmod(stx, 16.0f) - 8.0f;
                    
                    float nx = localX / 8.0f;
                    float lighting = 1.0f - (nx * nx * 0.6f);
                    
                    float noiseY = GerenciadorTexturas::fastSin(sty * 0.2f);
                    float barkNoise = GerenciadorTexturas::fastSin(stx * 0.8f + noiseY * 2.0f);
                    
                    r = std::clamp((int)((95 + barkNoise * 12 + trunkId*5) * lighting), 0, 255);
                    g = std::clamp((int)((65 + barkNoise * 8 + trunkId*3) * lighting), 0, 255);
                    b = std::clamp((int)((45 + barkNoise * 8 + trunkId*3) * lighting), 0, 255);
                    
                    float canopyEdge = 25.0f + 4.0f * GerenciadorTexturas::fastSin(stx * 0.3f) + 3.0f * GerenciadorTexturas::fastSin(stx * 0.8f);
                    if (sty < canopyEdge) {
                        float leafNoise = GerenciadorTexturas::fastSin(stx * 0.6f + sty * 0.6f) + GerenciadorTexturas::fastSin(stx * 1.5f + sty * 0.5f);
                        float depth = 1.0f - (sty / canopyEdge) * 0.4f;
                        r = std::clamp((int)((30 + leafNoise * 8) * depth), 0, 255);
                        g = std::clamp((int)((120 + leafNoise * 15) * depth), 0, 255);
                        b = std::clamp((int)((40 + leafNoise * 8) * depth), 0, 255);
                    }
                    break;
                }
                case TexID::PedraVila: {
                    float nx = tx + 1000.0f + 2.0f * GerenciadorTexturas::fastSin(ty * 0.4f) + GerenciadorTexturas::fastSin(tx * 0.2f);
                    float ny = ty + 1000.0f + 2.0f * GerenciadorTexturas::fastSin(tx * 0.4f) + GerenciadorTexturas::fastSin(ty * 0.2f);
                    
                    int stoneSize = 8;
                    int row = (int)(ny / stoneSize);
                    float offsetX = (row % 2 == 0) ? 0.0f : (stoneSize / 2.0f);
                    int col = (int)((nx + offsetX) / stoneSize);
                    
                    float localX = std::fmod(nx + offsetX, (float)stoneSize);
                    float localY = std::fmod(ny, (float)stoneSize);
                    
                    float dx = localX - (stoneSize / 2.0f);
                    float dy = localY - (stoneSize / 2.0f);
                    float dist = std::sqrt(dx*dx + dy*dy);
                    float maxDist = (stoneSize / 2.0f) * 1.1f;
                    
                    float stoneFactor = 1.0f - std::clamp((dist - (maxDist * 0.65f)) / (maxDist * 0.35f), 0.0f, 1.0f);
                    
                    int hash = (row * 31 + col * 17) % 30;
                    float noise = GerenciadorTexturas::fastSin(tx * 0.5f) + GerenciadorTexturas::fastSin(ty * 0.5f);
                    
                    int baseColor = 120 + hash + (int)(noise * 8);
                    int rejunteColor = 85 + (int)(noise * 4);
                    float lighting = 0.85f + 0.15f * (1.0f - (dist / maxDist));
                    
                    int c = (int)(rejunteColor * (1.0f - stoneFactor) + baseColor * lighting * stoneFactor);
                    r = g = b = std::clamp(c, 0, 255);
                    break;
                }
                case TexID::PedraSpawn: {
                    float nx = tx + 128.0f + 3.0f * GerenciadorTexturas::fastSin(ty * 0.15f);
                    float ny = ty + 128.0f + 3.0f * GerenciadorTexturas::fastSin(tx * 0.15f);
                    int row = (int)(ny / 32.0f);
                    float offsetX = (row % 2 == 0) ? 0.0f : 16.0f;
                    float cx = std::fmod(nx + offsetX, 32.0f) - 16.0f;
                    float cy = std::fmod(ny, 32.0f) - 16.0f;
                    float distSq = cx * cx + cy * cy;
                    
                    if (distSq < 210.0f) {
                        int shadow = (int)(distSq * 0.08f);
                        r = std::clamp(210 - shadow, 0, 255);
                        g = std::clamp(215 - shadow, 0, 255);
                        b = std::clamp(215 - shadow, 0, 255);
                    } else {
                        r = 140; g = 140; b = 140; // Rejunte claro
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
                    float nx = tx + 30.0f * GerenciadorTexturas::fastSin(ty * 0.05f); // tronco retorcido verticalmente
                    float trunkNoise = GerenciadorTexturas::fastSin(nx * 0.2f);
                    float barkDetail = GerenciadorTexturas::fastSin(nx * 0.8f + ty * 0.1f);
                    
                    float mossNoise = GerenciadorTexturas::fastSin(tx * 0.1f + ty * 0.1f) + GerenciadorTexturas::fastSin(tx * 0.03f - ty * 0.07f);
                    
                    if (mossNoise > 0.3f) {
                        float mossDetail = GerenciadorTexturas::fastSin(tx * 0.5f) * GerenciadorTexturas::fastSin(ty * 0.5f);
                        r = std::clamp(20 + (int)(mossDetail * 10), 0, 255);
                        g = std::clamp(65 + (int)(mossDetail * 20), 0, 255);
                        b = std::clamp(25 + (int)(mossDetail * 10), 0, 255);
                    } else {
                        float lighting = 0.7f + 0.3f * trunkNoise;
                        int baseColor = 55 + (int)(barkDetail * 10);
                        r = std::clamp((int)(baseColor * lighting), 0, 255);
                        g = std::clamp((int)((baseColor - 20) * lighting), 0, 255);
                        b = std::clamp((int)((baseColor - 35) * lighting), 0, 255);
                    }
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
                case TexID::ChaoCoracaoMusgo:
                case TexID::ChaoCoracaoTerra:
                case TexID::ChaoCoracaoEscuro: {
                    float noise = GerenciadorTexturas::fastSin(tx * 0.15f + ty * 0.1f) + GerenciadorTexturas::fastSin(ty * 0.1f - tx * 0.05f);
                    float detail = GerenciadorTexturas::fastSin(tx * 0.8f) * GerenciadorTexturas::fastSin(ty * 0.8f);
                    
                    if (noise > 0.0f) {
                        int baseR = 40, baseG = 25, baseB = 15;
                        r = std::clamp(baseR + (int)(detail * 5), 0, 255);
                        g = std::clamp(baseG + (int)(detail * 5), 0, 255);
                        b = std::clamp(baseB + (int)(detail * 3), 0, 255);
                    } else {
                        int baseR = 15, baseG = 40, baseB = 15;
                        r = std::clamp(baseR + (int)(detail * 8), 0, 255);
                        g = std::clamp(baseG + (int)(detail * 12), 0, 255);
                        b = std::clamp(baseB + (int)(detail * 8), 0, 255);
                    }
                    if (id == TexID::ChaoCoracaoEscuro) {
                        r = r / 2; g = g / 2; b = b / 2;
                    }
                    break;
                }
                case TexID::ChaoGramaFloresta: {
                    float noise = GerenciadorTexturas::fastSin(tx * 0.08f + ty * 0.04f) + GerenciadorTexturas::fastSin(ty * 0.08f - tx * 0.04f);
                    if (noise > 0.5f) { r = 5; g = 30; b = 5; }
                    else if (noise > -0.5f) { r = 4; g = 26; b = 4; }
                    else { r = 3; g = 22; b = 3; }
                    break;
                }
                case TexID::ChaoGramaVila: {
                    float noise = GerenciadorTexturas::fastSin(tx * 0.08f + ty * 0.04f) + GerenciadorTexturas::fastSin(ty * 0.08f - tx * 0.04f);
                    if (noise > 0.5f) { r = 12; g = 70; b = 12; }
                    else if (noise > -0.5f) { r = 10; g = 62; b = 10; }
                    else { r = 8; g = 54; b = 8; }
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
                case TexID::TetoIndoorsCoracaoMusgo:
                case TexID::TetoIndoorsCoracaoMadeira:
                case TexID::TetoIndoorsCoracaoEscuro: {
                    float nx = tx + 15.0f * GerenciadorTexturas::fastSin(ty * 0.1f);
                    float ny = ty + 15.0f * GerenciadorTexturas::fastSin(tx * 0.1f);
                    
                    float rootNoise = GerenciadorTexturas::fastSin(nx * 0.15f + ny * 0.15f);
                    float barkDetail = GerenciadorTexturas::fastSin(nx * 0.6f + ny * 0.6f);
                    float mossNoise = GerenciadorTexturas::fastSin(tx * 0.12f - ty * 0.12f);
                    
                    if (mossNoise > 0.2f) {
                        r = 20; g = 55; b = 25;
                    } else {
                        float lighting = 0.7f + 0.3f * rootNoise;
                        int baseColor = 45 + (int)(barkDetail * 8);
                        r = std::clamp((int)(baseColor * lighting), 0, 255);
                        g = std::clamp((int)((baseColor - 20) * lighting), 0, 255);
                        b = std::clamp((int)((baseColor - 30) * lighting), 0, 255);
                    }
                    
                    if (id == TexID::TetoIndoorsCoracaoEscuro) {
                        r = (int)(r * 0.6f); g = (int)(g * 0.6f); b = (int)(b * 0.6f);
                    }
                    break;
                }
                case TexID::TetoIndoorsPadrao: {
                    bool isJunta = (ty % 20 == 0) || (tx % 16 == 0);
                    if (isJunta) { r = 30; g = 15; b = 5; } else { r = 45; g = 25; b = 10; }
                    break;
                }

                default:
                    r = 255; g = 0; b = 255; // Magenta for missing textures
                    break;
            }

            cache[index][ty * res + tx] = { r, g, b };
        }
    }
}
