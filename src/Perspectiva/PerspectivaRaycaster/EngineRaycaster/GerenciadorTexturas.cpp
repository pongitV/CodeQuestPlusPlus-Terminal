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
    int res = 128; 

    for (int ty = 0; ty < res; ty++) {
        for (int tx = 0; tx < res; tx++) {
            uint8_t r = 0, g = 0, b = 0;

            switch (id) {

                case TexID::LabirintoMadeira: {
                    // TEMA: Painel de madeira e estuque, representando as paredes comuns do labirinto.

                    // Placas verticais (w=32) com rodape e teto
                    bool isBorderY = (ty < 8 || ty > 119);
                    bool isBorderX = (tx % 32 < 4);
                    float detail = fastSin(tx * 0.2f + fastSin(ty * 0.1f) * 10.0f) * 1.5f;
                    
                    if (isBorderY || isBorderX) {
                        r = std::clamp((int)(50 + detail * 5), 0, 255);
                        g = std::clamp((int)(30 + detail * 5), 0, 255);
                        b = std::clamp((int)(15 + detail * 5), 0, 255);
                    } else {
                        r = std::clamp((int)(180 + detail * 20), 0, 255);
                        g = std::clamp((int)(160 + detail * 15), 0, 255);
                        b = std::clamp((int)(120 + detail * 10), 0, 255);
                    }
                    break;
                }
                case TexID::LabirintoArcoPilar: {
                    // TEMA: Pilares de pedra do arco, desgastados e com pequenas rachaduras.

                    // Pilar redondo/sombreado
                    float cx = (tx - 64.0f) / 64.0f;
                    float cylinder = std::sqrt(std::max(0.0f, 1.0f - cx*cx));
                    float detail = fastSin(tx * 0.4f) * fastSin(ty * 0.2f) + fastSin((tx + ty)*0.3f);
                    
                    int base = (int)(cylinder * 150) + (int)(detail * 15);
                    bool crack = ((tx * 13 + ty * 7) % 150) < 4 || ((tx * 5 + ty * 11) % 200) < 3;
                    
                    if (crack) { r = 40; g = 40; b = 40; }
                    else {
                        r = std::clamp(base - 10, 0, 255);
                        g = std::clamp(base - 10, 0, 255);
                        b = std::clamp(base - 20, 0, 255);
                    }
                    break;
                }
                case TexID::LabirintoArcoFundo: {
                    // TEMA: Fundo de pedra escura do arco no labirinto.

                    // Bloquinhos de pedra pequenos
                    int bx = tx / 16;
                    int by = ty / 8;
                    if (by % 2 == 1) bx = (tx + 8) / 16;
                    
                    bool isJunta = (tx % 16 < 2) || (ty % 8 < 2);
                    if (by % 2 == 1) isJunta = ((tx + 8) % 16 < 2) || (ty % 8 < 2);
                    
                    float detail = fastSin(tx * 0.5f) * fastSin(ty * 0.5f);
                    if (isJunta) {
                        r = 30; g = 30; b = 30;
                    } else {
                        int base = 90 + (int)(detail * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::MorganaMadeira: {
                    // TEMA: Madeira escura e misteriosa da bruxa Morgana.

                    // Tabuas de madeira
                    int tabuaY = ty / 16;
                    float vx = tx * 0.1f + fastSin(ty * 0.3f) * 2.0f;
                    float grain = fastSin(vx);
                    
                    bool isJunta = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaMadeira) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::PonteMadeira) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::ReinoMadeira) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJunta) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tabuaY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::IgrejaVitral: {
                    // TEMA: Belos vitrais coloridos da igreja, filtrando luz celestial.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::PonteMadeira: {
                    // TEMA: Pranchas de madeira gasta formando a estrutura de uma ponte.

                    // Tabuas de madeira
                    int tabuaY = ty / 16;
                    float vx = tx * 0.1f + fastSin(ty * 0.3f) * 2.0f;
                    float grain = fastSin(vx);
                    
                    bool isJunta = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaMadeira) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::PonteMadeira) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::ReinoMadeira) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJunta) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tabuaY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::Alquimista: {
                    // TEMA: Azulejos e estantes de pedra sujos de poções do alquimista.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::EntradaIgreja: {
                    // TEMA: Grandes portões pesados de madeira e pedra na entrada da igreja.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::ManequimAnok: {
                    // TEMA: Madeira talhada com inscrições de batalha, alvo de treinos de Anok.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::Franchesco: {
                    // TEMA: Tecidos finos e pedras polidas da loja do mercador Franchesco.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::Bjorn: {
                    // TEMA: Paredes brutas de ferro e pedra da forja de Bjorn.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::Cavaleiro: {
                    // TEMA: Superfície de pedra nobre com brasões, próxima ao cavaleiro.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::ReinoMadeira: {
                    // TEMA: Madeira polida e detalhada do castelo real.

                    // Tabuas de madeira
                    int tabuaY = ty / 16;
                    float vx = tx * 0.1f + fastSin(ty * 0.3f) * 2.0f;
                    float grain = fastSin(vx);
                    
                    bool isJunta = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaMadeira) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::PonteMadeira) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::ReinoMadeira) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJunta) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tabuaY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::IgrejaAltar: {
                    // TEMA: Mármore branco e dourado compondo o altar sagrado.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::IgrejaParede: {
                    // TEMA: Paredes sagradas de pedra lisa da igreja.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::IgrejaParedeAltar: {
                    // TEMA: A parede principal logo atrás do altar, ricamente adornada.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::IgrejaTeto: {
                    // TEMA: Teto em arco da igreja, desenhado com padrões arquitetônicos altos.

                    // Temática da Igreja (Vitral, Altar, Parede, Teto)
                    float detail = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::IgrejaVitral) {
                        // Coloridos vitrais em padrão de diamante
                        float diag = std::abs(fastSin((tx + ty)*0.1f) + fastSin((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::IgrejaAltar || id == TexID::IgrejaParedeAltar) {
                        // Mármore com veios dourados
                        float vein = std::abs(fastSin(tx * 0.1f + ty * 0.2f + fastSin(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Mármore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        if (junta) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::PatioMuro: {
                    // TEMA: Muros de pedra do pátio, cobertos por finos musgos.

                    // Tijolos cobertos de musgo / Árvores densas
                    if (id == TexID::ArvoreFloresta) {
                        // Casca de árvore
                        float bark = fastSin(tx * 0.4f + fastSin(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        float moss = fastSin(tx * 0.1f) * fastSin(ty * 0.1f);
                        if (junta) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::FlorestaEstrutura: {
                    // TEMA: Ruínas esquecidas cobertas pela densa floresta.

                    // Tijolos cobertos de musgo / Árvores densas
                    if (id == TexID::ArvoreFloresta) {
                        // Casca de árvore
                        float bark = fastSin(tx * 0.4f + fastSin(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        float moss = fastSin(tx * 0.1f) * fastSin(ty * 0.1f);
                        if (junta) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::PadraoEstrutura: {
                    // TEMA: Paredes de alvenaria abandonada genérica.

                    // Tijolos cobertos de musgo / Árvores densas
                    if (id == TexID::ArvoreFloresta) {
                        // Casca de árvore
                        float bark = fastSin(tx * 0.4f + fastSin(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        float moss = fastSin(tx * 0.1f) * fastSin(ty * 0.1f);
                        if (junta) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::ArvoreCoracao: {
                    // TEMA: Casca retorcida e viva das imensas árvores do coração.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ArvoreFloresta: {
                    // TEMA: Troncos grossos e rugosos de árvores da floresta densa.

                    // Tijolos cobertos de musgo / Árvores densas
                    if (id == TexID::ArvoreFloresta) {
                        // Casca de árvore
                        float bark = fastSin(tx * 0.4f + fastSin(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool junta = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) junta = true;
                        
                        float moss = fastSin(tx * 0.1f) * fastSin(ty * 0.1f);
                        if (junta) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::PedraVila:
                case TexID::PedraSpawn:
                case TexID::CavernaCoracaoParede: {
                    // TEMA: Paredes construA-das com pedras arredondadas (Cobblestone HD).
                    
                    float stoneSize = 24.0f;
                    float sx = tx / stoneSize;
                    float sy = ty / stoneSize;
                    
                    // Add noise to the coordinates to make the stones irregular
                    float noiseX = fastSin(ty * 0.15f) * 0.5f;
                    float noiseY = fastSin(tx * 0.15f) * 0.5f;
                    
                    int cx = (int)(sx + noiseX);
                    int cy = (int)(sy + noiseY);
                    
                    float dx = (sx + noiseX) - (cx + 0.5f);
                    float dy = (sy + noiseY) - (cy + 0.5f);
                    
                    float dist = std::sqrt(dx*dx + dy*dy);
                    
                    bool isBorder = dist > 0.40f;
                    
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f) + fastSin((tx + ty) * 0.5f);
                    int stoneR = 100, stoneG = 100, stoneB = 100;
                    if (id == TexID::CavernaCoracaoParede) { stoneR = 60; stoneG = 30; stoneB = 80; }
                    
                    // Variation between blocks
                    int blockId = cx * 13 + cy * 7;
                    stoneR += (blockId % 30) - 15;
                    stoneG += (blockId % 30) - 15;
                    stoneB += (blockId % 30) - 15;
                    
                    if (isBorder) {
                        r = std::clamp(stoneR - 50, 0, 255);
                        g = std::clamp(stoneG - 50, 0, 255);
                        b = std::clamp(stoneB - 50, 0, 255);
                    } else {
                        // Highlight top-left edge of stone
                        float highlight = (dx < -0.1f && dy < -0.1f) ? 20.0f : 0.0f;
                        float shadow = (dx > 0.1f && dy > 0.1f) ? -30.0f : 0.0f;
                        
                        r = std::clamp((int)(stoneR + detail * 15.0f + highlight + shadow), 0, 255);
                        g = std::clamp((int)(stoneG + detail * 15.0f + highlight + shadow), 0, 255);
                        b = std::clamp((int)(stoneB + detail * 15.0f + highlight + shadow), 0, 255);
                    }
                    break;
                }
                case TexID::SalaChefeParede: {
                    // TEMA: Rochas negras e afiadas isolando a fúria do chefe.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ParedeInvalida: {
                    // TEMA: Fallback para texturas não encontradas.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::ChaoLabirintoBorda: {
                    // TEMA: Borda do chão do labirinto, misturando pedra com terra úmida.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::ChaoLabirinto || id == TexID::ChaoLabirintoBorda) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::ChaoTerra) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao genérico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoLabirinto: {
                    // TEMA: Ladrilhos de pedra quebrados do labirinto.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::ChaoLabirinto || id == TexID::ChaoLabirintoBorda) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::ChaoTerra) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao genérico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoSalaChefeFora: {
                    // TEMA: Solo de pedra escura antes da entrada do chefe.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoSalaChefeDentro: {
                    // TEMA: Piso vulcânico e irregular na arena do chefe.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoCoracaoMusgo: {
                    // TEMA: Chão coberto de musgo espesso que pulsa vida.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoCoracaoTerra: {
                    // TEMA: Terra fértil, espiralada e macia da área do coração.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoCoracaoEscuro: {
                    // TEMA: Raízes mortas e terra petrificada nas sombras.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoGramaFloresta: {
                    // TEMA: Grama verde e realista da floresta iluminada por raios solares.

                    float nx = tx * 0.15f;
                    float ny = ty * 0.15f;
                    float macroNoise = fastSin(nx * 0.2f) * fastSin(ny * 0.3f);
                    float clumpNoise = fastSin(nx + ny * 0.5f) + fastSin(ny - nx * 0.5f);
                    float bladeNoise = fastSin(tx * 0.8f + ty * 1.5f) * fastSin(tx * 1.2f - ty * 0.5f);
                    int baseG = 30 + (int)(macroNoise * 8) + (int)(clumpNoise * 5);
                    if (bladeNoise > 0.5f) {
                        r = std::clamp((int)(baseG * 0.3f), 0, 255);
                        g = std::clamp(baseG + 18, 0, 255);
                        b = std::clamp((int)(baseG * 0.3f), 0, 255);
                    } else if (bladeNoise < -0.3f) {
                        r = std::clamp((int)(baseG * 0.1f), 0, 255);
                        g = std::clamp(baseG - 12, 0, 255);
                        b = std::clamp((int)(baseG * 0.15f), 0, 255);
                    } else {
                        r = std::clamp((int)(baseG * 0.2f), 0, 255);
                        g = std::clamp(baseG, 0, 255);
                        b = std::clamp((int)(baseG * 0.25f), 0, 255);
                    }
                    break;
                }
                case TexID::ChaoGramaVila: {
                    // TEMA: Grama vibrante, macia e aparada da pacífica vila.

                    float nx = tx * 0.15f;
                    float ny = ty * 0.15f;
                    float macroNoise = fastSin(nx * 0.3f) * fastSin(ny * 0.2f);
                    float clumpNoise = fastSin(nx * 0.8f + ny * 0.4f) + fastSin(ny * 0.8f - nx * 0.4f);
                    float bladeNoise = fastSin(tx * 1.1f + ty * 1.8f) * fastSin(tx * 1.5f - ty * 0.9f);
                    int baseG = 75 + (int)(macroNoise * 12) + (int)(clumpNoise * 8);
                    if (bladeNoise > 0.4f) {
                        r = std::clamp((int)(baseG * 0.4f), 0, 255);
                        g = std::clamp(baseG + 25, 0, 255);
                        b = std::clamp((int)(baseG * 0.2f), 0, 255);
                    } else if (bladeNoise < -0.4f) {
                        r = std::clamp((int)(baseG * 0.15f), 0, 255);
                        g = std::clamp(baseG - 20, 0, 255);
                        b = std::clamp((int)(baseG * 0.1f), 0, 255);
                    } else {
                        r = std::clamp((int)(baseG * 0.25f), 0, 255);
                        g = std::clamp(baseG, 0, 255);
                        b = std::clamp((int)(baseG * 0.15f), 0, 255);
                    }
                    break;
                }
                case TexID::ChaoTerra: {
                    // TEMA: Caminho de terra seca, desgastado pela passagem de viajantes.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::ChaoLabirinto || id == TexID::ChaoLabirintoBorda) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::ChaoTerra) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao genérico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::ChaoPadrao: {
                    // TEMA: Solo básico de poeira e pedregulhos finos.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastSin(tx * 0.2f) * fastSin(ty * 0.2f);
                    if (id == TexID::ChaoLabirinto || id == TexID::ChaoLabirintoBorda) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::ChaoTerra) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao genérico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::TetoIndoorsCoracaoMusgo: {
                    // TEMA: Teto abobadado de rocha incrustado de musgos biológicoscentes.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::TetoIndoorsCoracaoMadeira: {
                    // TEMA: Raízes gigantescas se entrelaçando no teto acima.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::TetoIndoorsCoracaoEscuro: {
                    // TEMA: Teto cavernoso e negro onde a luz mal consegue alcançar.

                    // Ambientes Corruptos/Vivos (Coração, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastSin(distCenter * 0.1f + fastSin(tx * 0.05f) * 3.0f);
                    float detail = fastSin(tx * 0.3f) * fastSin(ty * 0.3f);
                    
                    if (id == TexID::ArvoreCoracao || id == TexID::TetoIndoorsCoracaoMadeira) {
                        // Raízes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::ChaoCoracaoMusgo || id == TexID::TetoIndoorsCoracaoMusgo) {
                        // Musgo bio-luminescente e vivo
                        if (detail > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::SalaChefeParede || id == TexID::ChaoSalaChefeDentro || id == TexID::ChaoSalaChefeFora) {
                        // Rocha vulcânica vulcânica rasgada
                        bool crack = std::abs(fastSin(tx*0.2f + ty*0.1f + fastSin(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(detail * 20);
                            r = std::clamp(dark, 0, 255); g = std::clamp(dark, 0, 255); b = std::clamp(dark, 0, 255);
                        }
                    } else {
                        // Caverna e pedras escuras mutantes
                        int base = 25 + (int)(spiral * 10);
                        r = std::clamp(base + 10, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base + 5, 0, 255);
                    }
                    break;
                }
                case TexID::TetoIndoorsPadrao: {
                    // TEMA: Um teto genérico e plano de pedra rústica para interiores.

                    // Generic HD Pattern for other unspecified items
                    float pattern = fastSin(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                default: {
                    // TEMA: Textura de depuração para TexIDs não implementados
                    bool checker = ((tx / 16) % 2) == ((ty / 16) % 2);
                    r = checker ? 255 : 0;
                    g = 0;
                    b = checker ? 255 : 0;
                    break;
                }
            }
            cache[index][ty * res + tx] = {r, g, b};
        }
    }
}
