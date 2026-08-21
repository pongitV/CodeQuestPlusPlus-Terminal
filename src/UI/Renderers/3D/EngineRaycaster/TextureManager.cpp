#include "UI/Renderers/3D/EngineRaycaster/TextureManager.h"
#include <cmath>
#include <algorithm>

bool ManagerTextures::initialized = false;
ColorRGB ManagerTextures::cache[256][16384];
float ManagerTextures::tableYes[4096];

float ManagerTextures::fastYes(float angle) {
    // angulo em radianos. Normaliza para 0-2PI
    constexpr float TWO_PI = 2.0f * 3.14159265f;
    float a = std::fmod(angle, TWO_PI);
    if (a < 0) a += TWO_PI;
    int index = (int)((a / TWO_PI) * 4096.0f) % 4096;
    return tableYes[index];
}

float ManagerTextures::fastCos(float angle) {
    return fastYes(angle + 1.57079632f); // angle + PI/2
}

void ManagerTextures::boot() {
    if (initialized) return;

    // Gerar tabelas trigonometricas
    for (int i = 0; i < 4096; i++) {
        tableYes[i] = std::sin((float)i / 4096.0f * 2.0f * 3.14159265f);
    }

    // Gerar Texturas
    for (int i = 1; i <= static_cast<int>(TexID::CeilingIndoorsPattern); i++) {
        generate(static_cast<TexID>(i));
    }

    initialized = true;
}

void ManagerTextures::generate(TexID id) {
    int index = static_cast<int>(id);
    int res = 128; 

    for (int ty = 0; ty < res; ty++) {
        for (int tx = 0; tx < res; tx++) {
            uint8_t r = 0, g = 0, b = 0;

            switch (id) {

                case TexID::LabyrinthWood: {
                    // TEMA: Painel de madeira e estuque, representando as paredes comuns do labirinto.

                    // Placas verticais (w=32) com rodape e teto
                    bool isBorderY = (ty < 8 || ty > 119);
                    bool isBorderX = (tx % 32 < 4);
                    float details = fastYes(tx * 0.2f + fastYes(ty * 0.1f) * 10.0f) * 1.5f;
                    
                    if (isBorderY || isBorderX) {
                        r = std::clamp((int)(50 + details * 5), 0, 255);
                        g = std::clamp((int)(30 + details * 5), 0, 255);
                        b = std::clamp((int)(15 + details * 5), 0, 255);
                    } else {
                        r = std::clamp((int)(180 + details * 20), 0, 255);
                        g = std::clamp((int)(160 + details * 15), 0, 255);
                        b = std::clamp((int)(120 + details * 10), 0, 255);
                    }
                    break;
                }
                case TexID::LabyrinthBowPillar: {
                    // TEMA: Pilares de pedra do arco, desgastados e com pequenas rachaduras.

                    // Pilar redondo/sombreado
                    float cx = (tx - 64.0f) / 64.0f;
                    float cylinder = std::sqrt(std::max(0.0f, 1.0f - cx*cx));
                    float details = fastYes(tx * 0.4f) * fastYes(ty * 0.2f) + fastYes((tx + ty)*0.3f);
                    
                    int base = (int)(cylinder * 150) + (int)(details * 15);
                    bool crack = ((tx * 13 + ty * 7) % 150) < 4 || ((tx * 5 + ty * 11) % 200) < 3;
                    
                    if (crack) { r = 40; g = 40; b = 40; }
                    else {
                        r = std::clamp(base - 10, 0, 255);
                        g = std::clamp(base - 10, 0, 255);
                        b = std::clamp(base - 20, 0, 255);
                    }
                    break;
                }
                case TexID::LabyrinthBowBackground: {
                    // TEMA: Fundo de pedra escura do arco no labirinto.

                    // Bloquinhos de pedra pequenos
                    int bx = tx / 16;
                    int by = ty / 8;
                    if (by % 2 == 1) bx = (tx + 8) / 16;
                    
                    bool isJoint = (tx % 16 < 2) || (ty % 8 < 2);
                    if (by % 2 == 1) isJoint = ((tx + 8) % 16 < 2) || (ty % 8 < 2);
                    
                    float details = fastYes(tx * 0.5f) * fastYes(ty * 0.5f);
                    if (isJoint) {
                        r = 30; g = 30; b = 30;
                    } else {
                        int base = 90 + (int)(details * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::MorganaWood: {
                    // TEMA: Madeira escura e misteriosa da bruxa Morgana.

                    // Tabuas de madeira
                    int tableY = ty / 16;
                    float vx = tx * 0.1f + fastYes(ty * 0.3f) * 2.0f;
                    float grain = fastYes(vx);
                    
                    bool isJoint = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaWood) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::BridgeWood) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::KingdomWood) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJoint) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tableY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::ChurchStainedglass: {
                    // TEMA: Belos vitrais coloridos da igreja, filtrando luz celestial.

                    // Tematica da Igreja (Vitral, Altar, Parede, Teto)
                    float details = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::ChurchStainedglass) {
                        // Coloridos vitrais em padrao de diamante
                        float diag = std::abs(fastYes((tx + ty)*0.1f) + fastYes((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::ChurchAltar || id == TexID::ChurchWallAltar) {
                        // Marmore com veios dourados
                        float vein = std::abs(fastYes(tx * 0.1f + ty * 0.2f + fastYes(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Marmore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        if (joint) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::BridgeWood: {
                    // TEMA: Pranchas de madeira gasta formando a estrutura de uma ponte.

                    // Tabuas de madeira
                    int tableY = ty / 16;
                    float vx = tx * 0.1f + fastYes(ty * 0.3f) * 2.0f;
                    float grain = fastYes(vx);
                    
                    bool isJoint = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaWood) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::BridgeWood) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::KingdomWood) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJoint) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tableY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::Alchemist: {
                    // TEMA: Laboratorio e Apotecario do Alquimista Real (Pocoes, Alambique, Ervas e Pergaminhos)
                    // Fundo de pedra nobre de castelo e estantes de mogno polido
                    int baseStone = 85 + (int)(fastYes(tx * 0.3f) * 10.0f + fastCos(ty * 0.3f) * 10.0f);
                    r = std::clamp(baseStone - 5, 0, 255);
                    g = std::clamp(baseStone - 5, 0, 255);
                    b = std::clamp(baseStone + 5, 0, 255);

                    // Estantes de madeira escura com guarnicao de latao
                    bool isShelf1 = (ty >= 34 && ty <= 38);
                    bool isShelf2 = (ty >= 82 && ty <= 86);
                    if (isShelf1 || isShelf2) {
                        r = 90; g = 50; b = 25;
                        if (ty == 34 || ty == 82) { r = 210; g = 175; b = 50; } // Borda dourada
                        if (ty == 38 || ty == 86) { r = 35; g = 20; b = 10; }  // Sombra
                    }
                    // Rodape de mogno com gavetas e gaveteiros
                    else if (ty >= 114) {
                        r = 65; g = 38; b = 20;
                        if ((tx % 42 < 2) || (ty == 114)) { r = 30; g = 18; b = 10; }
                        if (ty == 120 && (tx % 42 >= 18 && tx % 42 <= 24)) { r = 220; g = 180; b = 50; } // Puxador dourado
                    }
                    // Ervas secas penduradas no teto (tx: 8..24 e tx: 104..120, ty: 0..28)
                    else if ((tx >= 8 && tx <= 24 && ty <= 28) || (tx >= 104 && tx <= 120 && ty <= 28)) {
                        int hx = (tx < 30) ? (tx - 16) : (tx - 112);
                        if (ty < 8 && std::abs(hx) <= 1) { r = 160; g = 130; b = 70; } // Corda de sisal
                        else if (ty >= 8) {
                            float herbShape = fastYes(tx * 0.8f + ty * 0.4f);
                            if (herbShape > -0.2f && std::abs(hx) <= (ty - 6) / 2) {
                                r = 60 + (int)(herbShape * 25);
                                g = 130 + (int)(herbShape * 35);
                                b = 45 + (int)(herbShape * 20);
                            }
                        }
                    }
                    // Pocao de Vida (Vermelha brilhante com rolha) tx: 36..50, ty: 12..34
                    else if (tx >= 36 && tx <= 50 && ty >= 12 && ty <= 34) {
                        int cx = tx - 43, cy = ty - 24;
                        if (ty <= 16 && std::abs(cx) <= 2) { r = 140; g = 85; b = 45; } // Rolha
                        else if (ty <= 19 && std::abs(cx) <= 3) { r = 200; g = 220; b = 240; } // Gargalo de vidro
                        else if (cx * cx + cy * cy <= 64) {
                            float d = std::sqrt((float)(cx * cx + cy * cy));
                            if (d > 7.0f) { r = 220; g = 235; b = 250; } // Borda do vidro
                            else if (cx < -2 && cy < -2) { r = 255; g = 180; b = 180; } // Reflexo de luz
                            else {
                                r = 220; g = 25 + (int)(d * 5); b = 35; // Liquido vermelho carmesim
                            }
                        }
                    }
                    // Pocao de Mana (Azul arcano vibrante) tx: 58..72, ty: 12..34
                    else if (tx >= 58 && tx <= 72 && ty >= 12 && ty <= 34) {
                        int cx = tx - 65, cy = ty - 24;
                        if (ty <= 16 && std::abs(cx) <= 2) { r = 140; g = 85; b = 45; } // Rolha
                        else if (ty <= 19 && std::abs(cx) <= 3) { r = 200; g = 220; b = 240; }
                        else if (cx * cx + cy * cy <= 64) {
                            float d = std::sqrt((float)(cx * cx + cy * cy));
                            if (d > 7.0f) { r = 210; g = 230; b = 255; }
                            else if (cx < -2 && cy < -2) { r = 190; g = 230; b = 255; } // Brilho
                            else {
                                r = 25; g = 110 + (int)(d * 10); b = 245; // Liquido azul safira
                            }
                        }
                    }
                    // Pocao de Estamina / Elixir (Verde esmeralda brilhante) tx: 80..92, ty: 14..34
                    else if (tx >= 80 && tx <= 92 && ty >= 14 && ty <= 34) {
                        int cx = tx - 86, cy = ty - 25;
                        if (ty <= 18 && std::abs(cx) <= 2) { r = 140; g = 85; b = 45; }
                        else if (std::abs(cx) <= 4 && cy >= -5 && cy <= 8) {
                            if (std::abs(cx) == 4 || cy == 8) { r = 200; g = 240; b = 220; }
                            else if (cx == -2 && cy < 0) { r = 200; g = 255; b = 200; }
                            else { r = 35; g = 215; b = 65; } // Liquido verde esmeralda
                        }
                    }
                    // Alambique de Destilacao e Tubo de Vidro (tx: 10..38, ty: 44..82)
                    else if (tx >= 10 && tx <= 38 && ty >= 44 && ty <= 82) {
                        int cx = tx - 22, cy = ty - 68;
                        // Tubo curvado de condensador
                        if (ty >= 44 && ty <= 56 && std::abs(tx - (22 + (ty - 44))) <= 2) {
                            r = 210; g = 230; b = 245;
                        }
                        // Balão de vidro e liquido purpura
                        else if (cx * cx + cy * cy <= 120) {
                            float d = std::sqrt((float)(cx * cx + cy * cy));
                            if (d > 9.5f) { r = 220; g = 235; b = 250; }
                            else if (cx < -2 && cy < -2) { r = 245; g = 200; b = 255; }
                            else { r = 175; g = 35; b = 210; } // Liquido purpura magico
                        }
                    }
                    // Pergaminho com Runas e Circulo Alquimico (tx: 46..76, ty: 44..78)
                    else if (tx >= 46 && tx <= 76 && ty >= 44 && ty <= 78) {
                        int px = tx - 61, py = ty - 61;
                        if (tx == 46 || tx == 76 || ty == 44 || ty == 78) { r = 180; g = 150; b = 100; } // Borda gasta
                        else {
                            r = 235; g = 220; b = 185; // Pergaminho antigo
                            // Circulo alquimico e runas em tinta marrom
                            int distRune = px * px + py * py;
                            if (std::abs(distRune - 100) <= 15 || std::abs(px) == std::abs(py) || (px == 0 && std::abs(py) <= 10)) {
                                r = 100; g = 45; b = 20; // Tinta de runa
                            }
                        }
                    }
                    // Almofariz e Pilão com ervas moidas (tx: 86..116, ty: 52..82)
                    else if (tx >= 86 && tx <= 116 && ty >= 52 && ty <= 82) {
                        int mx = tx - 101, my = ty - 70;
                        // Pilao inclinado
                        if (tx + ty >= 155 && tx + ty <= 161 && ty >= 52 && ty <= 72) {
                            r = 170; g = 170; b = 175; // Pedra do pilao
                        }
                        // Tigela do almofariz
                        else if (my >= 0 && mx * mx + my * my * 2 <= 140) {
                            if (my == 0 && std::abs(mx) <= 8) { r = 40; g = 180; b = 60; } // Po de erva verde
                            else if (mx * mx + my * my * 2 >= 110) { r = 130; g = 130; b = 135; }
                            else { r = 90; g = 90; b = 95; }
                        }
                    }
                    // Frascos menores de reagentes na prateleira de baixo (tx: 20..110, ty: 90..112)
                    else if (ty >= 90 && ty <= 112) {
                        int jarIdx = (tx - 15) / 20;
                        int jx = (tx - 15) % 20;
                        if (jx >= 4 && jx <= 16 && ty >= 94 && ty <= 112) {
                            if (ty <= 96 && jx >= 7 && jx <= 13) { r = 140; g = 90; b = 40; } // Rolha
                            else if (jx == 4 || jx == 16 || ty == 112) { r = 190; g = 210; b = 230; } // Vidro
                            else {
                                if (jarIdx % 3 == 0) { r = 240; g = 180; b = 30; } // Po de ouro/enxofre
                                else if (jarIdx % 3 == 1) { r = 40; g = 200; b = 210; } // Cristal ciano
                                else { r = 230; g = 60; b = 140; } // Extrato de flor arcana
                            }
                        }
                    }
                    break;
                }
                case TexID::EntryChurch: {
                    // TEMA: Entrada da Igreja e Altar Sagrado (Vitral Gotico Celestial, Crucifixo Dourado, Velas e Escrituras)
                    // Fundo de cantaria de marmore sagrado e pedra nobre clara
                    int marble = 200 + (int)(fastYes(tx * 0.15f + ty * 0.15f) * 15.0f);
                    r = std::clamp(marble, 0, 255);
                    g = std::clamp(marble, 0, 255);
                    b = std::clamp(marble + 10, 0, 255);

                    // Vitral Gotico Celestial com luz colorida no topo (tx: 36..92, ty: 4..52)
                    if (tx >= 36 && tx <= 92 && ty >= 4 && ty <= 52) {
                        int vx = tx - 64, vy = ty - 28;
                        // Arco em ogiva
                        if (vy <= 22 && (vy >= 0 || (vx * vx * 2 - vy * 12 <= 280))) {
                            if (std::abs(vx) >= 26 || vy == 22 || std::abs(vx) == std::abs(vy) || vx == 0) {
                                r = 35; g = 35; b = 40; // Chumbo do vitral
                            } else {
                                int pane = ((tx / 10) + (ty / 10)) % 4;
                                if (pane == 0) { r = 240; g = 50; b = 60; } // Rubi sagrado
                                else if (pane == 1) { r = 40; g = 130; b = 250; } // Safira celestial
                                else if (pane == 2) { r = 255; g = 210; b = 45; } // Ouro divino
                                else { r = 180; g = 60; b = 220; } // Ametista
                            }
                        }
                    }
                    // Crucifixo Dourado com raios de luz (tx: 54..74, ty: 12..38)
                    else if (tx >= 54 && tx <= 74 && ty >= 12 && ty <= 38) {
                        int cx = tx - 64, cy = ty - 24;
                        if ((std::abs(cx) <= 2 && cy >= -10 && cy <= 12) || (std::abs(cy + 2) <= 2 && std::abs(cx) <= 8)) {
                            r = 255; g = 220; b = 50; // Ouro puro
                        }
                    }
                    // Prateleira / Altar de marmore sagrado (ty: 82..88)
                    if (ty >= 82 && ty <= 88) {
                        r = 235; g = 235; b = 240;
                        if (ty == 82) { r = 220; g = 180; b = 45; } // Filete dourado
                    }
                    // Casticais Dourados com Velas Acesas (tx: 14..30 e tx: 98..114, ty: 48..82)
                    else if ((tx >= 14 && tx <= 30 && ty >= 48 && ty <= 82) || (tx >= 98 && tx <= 114 && ty >= 48 && ty <= 82)) {
                        int kx = (tx < 50) ? (tx - 22) : (tx - 106);
                        // Chama da vela
                        if (ty >= 48 && ty <= 56 && std::abs(kx) <= (ty >= 52 ? (56 - ty) : (ty - 48))) {
                            r = 255; g = 225; b = 70; // Chama amarela/branca
                        }
                        // Corpo da vela de cera
                        else if (ty >= 57 && ty <= 70 && std::abs(kx) <= 3) {
                            r = 245; g = 240; b = 225;
                        }
                        // Castical de ouro
                        else if (ty >= 71 && ty <= 82 && std::abs(kx) <= (ty >= 78 ? 7 : 2)) {
                            r = 225; g = 185; b = 45;
                        }
                    }
                    // Livro de Escrituras Sagradas / Biblia Aberta no centro (tx: 44..84, ty: 56..82)
                    else if (tx >= 44 && tx <= 84 && ty >= 56 && ty <= 82) {
                        int bx = tx - 64, by = ty - 68;
                        if (std::abs(bx) <= 18 && by >= -10 && by <= 12) {
                            if (bx == 0) { r = 220; g = 30; b = 30; } // Fita marcadora vermelha
                            else {
                                r = 240; g = 235; b = 215; // Folhas de pergaminho
                                if (std::abs(by) % 3 == 0 && std::abs(bx) > 3) { r = 60; g = 50; b = 40; } // Texto sagrado
                            }
                        }
                    }
                    // Painel de marmore esculpido na base com cruzes douradas (ty >= 90)
                    else if (ty >= 90) {
                        r = 215; g = 215; b = 220;
                        if (ty == 90 || ty == 127 || tx % 32 < 2) { r = 160; g = 160; b = 165; }
                        int mx = (tx % 32) - 16, my = ty - 108;
                        if ((std::abs(mx) <= 1 && std::abs(my) <= 8) || (std::abs(my) <= 1 && std::abs(mx) <= 5)) {
                            r = 220; g = 180; b = 45; // Cruz dourada em relevo
                        }
                    }
                    break;
                }
                case TexID::MannequinAnok: {
                    // TEMA: Alfaiataria e Estilo Real de Anok (Manequim com Tunica Real, Rolos de Tecido, Veludo e Tesouras)
                    // Parede de salao nobre com paineis de carvalho elegante
                    int woodBase = 110 + (int)(fastYes(tx * 0.15f) * 15.0f);
                    r = std::clamp(woodBase, 0, 255);
                    g = std::clamp(woodBase - 35, 0, 255);
                    b = std::clamp(woodBase - 65, 0, 255);

                    // Varao dourado e cortinados de veludo purpura e carmesim no topo (ty: 0..32)
                    if (ty <= 6) {
                        r = 230; g = 190; b = 45; // Varao dourado
                    } else if (ty <= 32) {
                        float drape = fastYes(tx * 0.25f);
                        if (ty >= 28 && ty <= 31) {
                            r = 220; g = 180; b = 40; // Franja de ouro
                        } else {
                            if (tx < 64) {
                                r = std::clamp((int)(110 + drape * 30), 0, 255);
                                g = std::clamp((int)(20 + drape * 10), 0, 255);
                                b = std::clamp((int)(140 + drape * 35), 0, 255); // Veludo purpura real
                            } else {
                                r = std::clamp((int)(170 + drape * 35), 0, 255);
                                g = std::clamp((int)(25 + drape * 10), 0, 255);
                                b = std::clamp((int)(40 + drape * 15), 0, 255); // Veludo carmesim
                            }
                        }
                    }
                    // Tesoura de alfaiate dourada pendurada (tx: 12..28, ty: 36..64)
                    else if (tx >= 12 && tx <= 28 && ty >= 36 && ty <= 64) {
                        int sx = tx - 20, sy = ty - 50;
                        if (sy >= 6 && (std::abs(sx - 4) <= 2 || std::abs(sx + 4) <= 2)) {
                            r = 225; g = 185; b = 45; // Aneis da tesoura
                        } else if (sy <= 5 && std::abs(sx) <= 2 + sy / 3) {
                            r = 220; g = 225; b = 235; // Laminas de aco polido
                        }
                    }
                    // Rolos de tecidos nobres empilhados na esquerda (tx: 6..38, ty: 66..112)
                    else if (tx >= 6 && tx <= 38 && ty >= 66 && ty <= 112) {
                        int roll = (ty - 66) / 15;
                        int ry = (ty - 66) % 15;
                        if (ry == 0 || ry == 14 || tx == 6 || tx == 38) { r = 40; g = 25; b = 15; } // Borda do rolo
                        else {
                            if (roll == 0) { r = 25; g = 80; b = 210; } // Seda azul real
                            else if (roll == 1) { r = 215; g = 175; b = 30; } // Brocado de ouro
                            else { r = 35; g = 155; b = 75; } // Veludo esmeralda
                        }
                    }
                    // Manequim Central com Tunica Real Bordada (tx: 44..84, ty: 34..105)
                    else if (tx >= 44 && tx <= 84 && ty >= 34 && ty <= 105) {
                        int mx = tx - 64;
                        // Cabeca/Pescoco do manequim de madeira
                        if (ty <= 42 && std::abs(mx) <= 5) {
                            r = 175; g = 120; b = 70;
                        }
                        // Ombros e Torso com Tunica Real Carmesim e Ouro
                        else if (ty >= 43 && ty <= 88) {
                            int halfW = (ty <= 60) ? (6 + (ty - 43) * 12 / 17) : (18 - (ty - 60) * 5 / 28);
                            if (std::abs(mx) <= halfW) {
                                // Fita metrica amarela atravessada no peito
                                if (std::abs(mx - (ty - 65)) <= 2) {
                                    r = 245; g = 225; b = 40;
                                    if ((tx + ty) % 4 == 0) { r = 30; g = 30; b = 30; } // Marcacoes da fita
                                }
                                // Brocado dourado central e botoes
                                else if (std::abs(mx) <= 2) {
                                    r = 235; g = 195; b = 45; // Botoes de ouro
                                } else if (std::abs(mx) >= halfW - 2) {
                                    r = 215; g = 175; b = 40; // Borda dourada
                                } else {
                                    r = 165; g = 25; b = 38; // Tunica carmesim real
                                }
                            }
                        }
                        // Pedestal de madeira torneada
                        else if (ty >= 89 && std::abs(mx) <= (ty >= 100 ? (ty - 96) : 3)) {
                            r = 150; g = 95; b = 50;
                        }
                    }
                    // Carreteis de linha e prateleira a direita (tx: 90..122, ty: 36..112)
                    else if (tx >= 90 && tx <= 122 && ty >= 36 && ty <= 112) {
                        int spoolRow = (ty - 40) / 22;
                        int sy = (ty - 40) % 22;
                        if (sy >= 2 && sy <= 18 && tx >= 94 && tx <= 118) {
                            if (sy <= 4 || sy >= 16) { r = 180; g = 130; b = 75; } // Madeira do carretel
                            else {
                                if (spoolRow == 0) { r = 240; g = 200; b = 40; } // Linha de ouro
                                else if (spoolRow == 1) { r = 180; g = 30; b = 190; } // Linha purpura
                                else { r = 230; g = 40; b = 40; } // Linha escarlate
                            }
                        } else if (sy >= 19) {
                            r = 120; g = 75; b = 40; // Prateleira
                        }
                    }
                    // Base do piso e botas de couro nobre (ty >= 112)
                    else if (ty >= 112) {
                        // Par de botas de couro com fivela dourada (tx: 48..80, ty: 114..126)
                        if (tx >= 48 && tx <= 80 && ty >= 114) {
                            int bx = (tx < 64) ? (tx - 54) : (tx - 70);
                            if (std::abs(bx) <= 5) {
                                r = 55; g = 30; b = 15; // Couro escuro
                                if (ty == 118 && std::abs(bx) <= 3) { r = 225; g = 185; b = 45; } // Fivela dourada
                            } else {
                                r = 75; g = 45; b = 25; // Chao de madeira
                            }
                        } else {
                            r = 75; g = 45; b = 25;
                        }
                    }
                    break;
                }
                case TexID::Francesco: {
                    // TEMA: Empório do Mercador Franchesco (Sacos de Ouro, Bau de Tesouro, Balanca e Provisoes)
                    // Fundo de vigas rusticas de carvalho e prateleiras comerciais
                    int wood = 100 + (int)(fastYes(tx * 0.2f) * 12.0f);
                    r = std::clamp(wood, 0, 255);
                    g = std::clamp(wood - 35, 0, 255);
                    b = std::clamp(wood - 65, 0, 255);

                    // Viga superior de sustentacao e lanterna acesa (ty: 0..38)
                    if (ty <= 8) {
                        r = 60; g = 35; b = 18;
                    }
                    // Embutidos suspensos: linguicas/alhos secos na esquerda (tx: 10..28, ty: 8..36)
                    else if (tx >= 10 && tx <= 28 && ty <= 36) {
                        int sx = tx - 19;
                        if (std::abs(sx) <= 3) {
                            if (ty % 10 <= 2) { r = 160; g = 130; b = 70; } // Amarra
                            else { r = 175; g = 45; b = 30; } // Linguica defumada
                        }
                    }
                    // Lanterna de Latao Comercial suspensa a direita (tx: 96..118, ty: 8..38)
                    else if (tx >= 96 && tx <= 118 && ty <= 38) {
                        int lx = tx - 107;
                        if (ty <= 14 && std::abs(lx) <= 1) { r = 80; g = 80; b = 80; } // Corrente
                        else if (ty >= 15 && ty <= 36 && std::abs(lx) <= 7) {
                            if (ty == 15 || ty == 36 || std::abs(lx) == 7) {
                                r = 215; g = 175; b = 45; // Armadura de latao
                            } else {
                                r = 255; g = 220; b = 80; // Vidro incandescente amarelo
                            }
                        }
                    }
                    // Prateleira comercial central (ty: 80..86)
                    else if (ty >= 80 && ty <= 86) {
                        r = 125; g = 75; b = 35;
                        if (ty == 80) { r = 160; g = 100; b = 50; }
                    }
                    // Sacos de Ouro amarrados com moedas reluzentes (tx: 10..42, ty: 44..80)
                    else if (tx >= 10 && tx <= 42 && ty >= 44 && ty <= 80) {
                        int sx = tx - 26, sy = ty - 65;
                        if (sy >= -18 && sy <= 15 && sx * sx + sy * sy <= 200) {
                            // Amarra e moedas saindo no topo
                            if (sy <= -10) {
                                if (std::abs(sx) <= 3) { r = 140; g = 100; b = 50; } // No da corda
                                else { r = 255; g = 215; b = 0; } // Moedas de ouro
                            } else {
                                r = 185; g = 145; b = 90; // Tecido de juta / linho rustico
                                if (sy == 0 && std::abs(sx) <= 2) { r = 215; g = 175; b = 0; } // Simbolo da moeda
                            }
                        }
                    }
                    // Balanca de Precisao de Mercador no centro (tx: 44..74, ty: 40..80)
                    else if (tx >= 44 && tx <= 74 && ty >= 40 && ty <= 80) {
                        int bx = tx - 59, by = ty - 56;
                        // Haste vertical
                        if (std::abs(bx) <= 1 && by >= -14 && by <= 22) {
                            r = 220; g = 180; b = 45;
                        }
                        // Braco horizontal da balanca
                        else if (std::abs(by + 10) <= 1 && std::abs(bx) <= 12) {
                            r = 220; g = 180; b = 45;
                        }
                        // Pratos suspensos
                        else if ((std::abs(bx + 10) <= 4 || std::abs(bx - 10) <= 4) && by >= 2 && by <= 6) {
                            r = 235; g = 195; b = 50;
                        }
                    }
                    // Bau de Tesouro com cintas de latao a direita (tx: 74..118, ty: 46..80)
                    else if (tx >= 74 && tx <= 118 && ty >= 46 && ty <= 80) {
                        int cx = tx - 96, cy = ty - 64;
                        if (std::abs(cx) <= 18 && cy >= -16 && cy <= 16) {
                            // Cintas de metal e fechadura
                            if (std::abs(cx) == 18 || cy == -16 || cy == 16 || std::abs(cx) == 8 || cy == 0) {
                                r = 225; g = 185; b = 45; // Metal dourado
                                if (cy == 2 && std::abs(cx) <= 2) { r = 20; g = 20; b = 20; } // Buraco da chave
                            } else {
                                r = 120; g = 65; b = 30; // Madeira nobre do bau
                            }
                        }
                    }
                    // Caixotes de madeira e barris de provisao na base (ty >= 86)
                    else if (ty >= 86) {
                        // Barril de carvalho com aros de ferro na esquerda (tx: 10..52, ty: 88..126)
                        if (tx >= 10 && tx <= 52) {
                            int bx = tx - 31;
                            if (std::abs(bx) <= 18) {
                                if (ty == 92 || ty == 104 || ty == 118 || ty == 125) {
                                    r = 80; g = 80; b = 85; // Aros de ferro
                                } else {
                                    r = 110; g = 65; b = 30; // Aduelas de carvalho
                                }
                            }
                        }
                        // Caixote de mantimentos na direita (tx: 68..118, ty: 88..126)
                        else if (tx >= 68 && tx <= 118) {
                            if (tx == 68 || tx == 118 || ty == 88 || ty == 126 || (tx - 68) == (ty - 88)) {
                                r = 145; g = 95; b = 45; // Reforco em X
                            } else {
                                r = 175; g = 120; b = 65; // Madeira do caixote
                            }
                        }
                    }
                    break;
                }
                case TexID::Kiss: {
                    // TEMA: Forja e Armaria Real de Bjorn (Espadas, Escudos, Martelos, Bigorna e Brilho da Forja)
                    // Parede de tijolos de forja escurecidos pelo fogo e fuligem
                    int brickW = 32, brickH = 16;
                    int bx = tx / brickW, by = ty / brickH;
                    if (by % 2 == 1) bx = (tx + 16) / brickW;
                    bool isJoint = (tx % brickW < 2) || (ty % brickH < 2);
                    if (by % 2 == 1 && ((tx + 16) % brickW < 2)) isJoint = true;

                    // Brilho quente da forja que emana de baixo
                    float heat = (float)ty / 128.0f;
                    int brickVar = ((bx * 7 + by * 11) % 15) - 7;
                    int baseStone = 45 + brickVar + (int)(heat * 30.0f);
                    if (isJoint) {
                        r = 20; g = 15; b = 15;
                    } else {
                        r = std::clamp((int)(baseStone + heat * 50), 0, 255);
                        g = std::clamp((int)(baseStone + heat * 20), 0, 255);
                        b = std::clamp(baseStone - 10, 0, 255);
                    }

                    // Suporte de ferramentas de ferro no topo (ty: 8..14)
                    if (ty >= 8 && ty <= 14) {
                        r = 70; g = 70; b = 75;
                        if (tx % 20 < 4) { r = 200; g = 180; b = 100; } // Rebites
                    }
                    // Martelo de Forja pesado pendurado a esquerda (tx: 10..26, ty: 12..48)
                    else if (tx >= 10 && tx <= 26 && ty >= 12 && ty <= 48) {
                        int hx = tx - 18;
                        if (ty >= 12 && ty <= 24 && std::abs(hx) <= 6) {
                            r = 160; g = 165; b = 175; // Cabeca de aco do martelo
                            if (ty == 12 || ty == 24 || std::abs(hx) == 6) { r = 90; g = 90; b = 95; }
                        } else if (ty >= 25 && std::abs(hx) <= 2) {
                            r = 150; g = 90; b = 45; // Cabo de madeira
                        }
                    }
                    // Tenazes de Ferreiro penduradas a direita (tx: 102..118, ty: 12..48)
                    else if (tx >= 102 && tx <= 118 && ty >= 12 && ty <= 48) {
                        int tx_rel = tx - 110;
                        if (std::abs(tx_rel) <= (ty <= 26 ? 3 : 5)) {
                            r = 130; g = 130; b = 135; // Ferro forjado
                        }
                    }
                    // Espada Longa Forjada no centro (tx: 56..72, ty: 22..90)
                    else if (tx >= 56 && tx <= 72 && ty >= 22 && ty <= 90) {
                        int sx = tx - 64;
                        // Pomo da espada
                        if (ty >= 22 && ty <= 26 && std::abs(sx) <= 3) {
                            r = 220; g = 180; b = 45; // Ouro/Latao
                        }
                        // Empunhadura de couro
                        else if (ty >= 27 && ty <= 38 && std::abs(sx) <= 2) {
                            r = 85; g = 45; b = 20;
                        }
                        // Guarda-mao / Guarda-cruz
                        else if (ty >= 39 && ty <= 43 && std::abs(sx) <= 7) {
                            r = 215; g = 175; b = 45;
                        }
                        // Lamina de aco duplo fio reluzente
                        else if (ty >= 44 && ty <= 90 && std::abs(sx) <= (ty >= 84 ? (90 - ty) / 2 : 3)) {
                            if (sx == 0) { r = 120; g = 130; b = 145; } // Sulco central (fuller)
                            else if (sx == -1 || sx == -2) { r = 245; g = 250; b = 255; } // Fio brilhante
                            else { r = 195; g = 205; b = 220; }
                        }
                    }
                    // Escudo Medieval de Cavaleiro na esquerda (tx: 16..46, ty: 50..92)
                    else if (tx >= 16 && tx <= 46 && ty >= 50 && ty <= 92) {
                        int ex = tx - 31, ey = ty - 68;
                        if (ey >= -18 && ey <= 24 && std::abs(ex) <= (ey <= 5 ? 14 : (14 - (ey - 5) * 14 / 19))) {
                            if (std::abs(ex) >= 12 || ey == -18) {
                                r = 190; g = 195; b = 205; // Borda de aco
                            } else if (ex * ex + ey * ey <= 16) {
                                r = 225; g = 185; b = 45; // Emblema dourado central
                            } else {
                                r = 150; g = 30; b = 35; // Fundo vermelho heráldico
                            }
                        }
                    }
                    // Machado de Batalha / Adaga na direita (tx: 82..112, ty: 50..92)
                    else if (tx >= 82 && tx <= 112 && ty >= 50 && ty <= 92) {
                        int ax = tx - 97, ay = ty - 70;
                        if (std::abs(ax) <= 2 && ay >= -18 && ay <= 20) {
                            r = 140; g = 85; b = 45; // Cabo de madeira
                        } else if (ay >= -16 && ay <= 2 && ax >= 3 && ax <= 14) {
                            r = 210; g = 220; b = 235; // Lamina do machado afiada
                            if (ax == 14) { r = 255; g = 255; b = 255; }
                        }
                    }
                    // Bigorna e brasas incandescentes da forja na base (ty >= 94)
                    else if (ty >= 94) {
                        // Bigorna de aco no centro (tx: 44..84, ty: 96..124)
                        if (tx >= 44 && tx <= 84 && ty >= 96 && ty <= 124) {
                            int anx = tx - 64, any = ty - 110;
                            if (any <= -4 && (anx >= -18 && anx <= 18)) {
                                r = 175; g = 180; b = 190; // Face superior da bigorna
                            } else if (any > -4 && any <= 8 && std::abs(anx) <= 10) {
                                r = 90; g = 95; b = 105; // Corpo da bigorna
                            } else if (any > 8 && std::abs(anx) <= 16) {
                                r = 120; g = 75; b = 40; // Toco de carvalho suporte
                            } else {
                                r = 240; g = 90; b = 15; // Brilho de fogo
                            }
                        }
                        // Lingotes de ferro e brasas ardentes nas laterais
                        else {
                            float fireNoise = fastYes(tx * 0.4f + fastYes(ty * 0.5f) * 3.0f);
                            if (fireNoise > 0.0f) {
                                r = 255; g = std::clamp((int)(110 + fireNoise * 80), 0, 255); b = 20; // Brasas
                            } else {
                                r = 180; g = 40; b = 10;
                            }
                        }
                    }
                    break;
                }
                case TexID::Knight: {
                    // TEMA: Posto da Guarda e Armaria Real (Estandarte Real, Tocha Acesa, Lancas, Escudo da Coroa e Elmo)
                    // Parede de blocos de pedra cinza medieval do castelo
                    int brickW = 32, brickH = 16;
                    int bx = tx / brickW, by = ty / brickH;
                    if (by % 2 == 1) bx = (tx + 16) / brickW;
                    bool isJoint = (tx % brickW < 2) || (ty % brickH < 2);
                    if (by % 2 == 1 && ((tx + 16) % brickW < 2)) isJoint = true;

                    int baseStone = 120 + ((bx * 7 + by * 13) % 20) - 10;
                    if (isJoint) {
                        r = 45; g = 45; b = 48;
                    } else {
                        r = std::clamp(baseStone, 0, 255);
                        g = std::clamp(baseStone + 2, 0, 255);
                        b = std::clamp(baseStone + 8, 0, 255);
                    }

                    // Tocha de Ferro na parede com chama a direita (tx: 98..120, ty: 6..46)
                    if (tx >= 98 && tx <= 120 && ty >= 6 && ty <= 46) {
                        int fx = tx - 109;
                        // Chama viva
                        if (ty <= 22 && std::abs(fx) <= (22 - ty) / 2 + 1) {
                            r = 255; g = std::clamp(140 + (ty - 6) * 12, 0, 255); b = 25;
                        }
                        // Suporte da tocha em ferro fundido
                        else if (ty >= 23 && ty <= 46 && std::abs(fx) <= (ty <= 32 ? 4 : 2)) {
                            r = 50; g = 50; b = 55;
                        }
                    }
                    // Estandarte Real Purpura com Brasao da Coroa a esquerda (tx: 10..42, ty: 4..68)
                    else if (tx >= 10 && tx <= 42 && ty >= 4 && ty <= 68) {
                        int ex = tx - 26;
                        if (ty <= 8 && std::abs(ex) <= 15) {
                            r = 220; g = 180; b = 45; // Suporte de ouro
                        } else if (ty >= 9 && ty <= 62 && std::abs(ex) <= 14) {
                            // Brasao dourado da coroa no centro do estandarte
                            int cy = ty - 32;
                            if (std::abs(ex) <= 6 && std::abs(cy) <= 6) {
                                r = 240; g = 200; b = 45; // Coroa real bordada
                            } else {
                                r = 100; g = 25; b = 135; // Tecido purpura real
                            }
                        } else if (ty >= 63 && ty <= 68 && std::abs(ex) <= 14) {
                            r = 215; g = 175; b = 40; // Franja dourada
                        }
                    }
                    // Lancas / Alabardas Reais Cruzadas no centro (tx: 44..84, ty: 24..96)
                    else if (tx >= 44 && tx <= 84 && ty >= 24 && ty <= 96) {
                        int lx = tx - 64;
                        // Pontas de aco reluzente das lancas
                        if (ty >= 24 && ty <= 42 && (std::abs(lx - (ty - 34)) <= 2 || std::abs(lx + (ty - 34)) <= 2)) {
                            r = 230; g = 235; b = 245; // Aco polido
                        }
                        // Hastes de madeira
                        else if (ty >= 43 && (std::abs(lx - (ty - 60) / 2) <= 1 || std::abs(lx + (ty - 60) / 2) <= 1)) {
                            r = 130; g = 80; b = 40;
                        }
                    }
                    // Escudo de Cavaleiro Real no centro (tx: 48..80, ty: 46..90)
                    else if (tx >= 48 && tx <= 80 && ty >= 46 && ty <= 90) {
                        int kx = tx - 64, ky = ty - 68;
                        if (ky >= -20 && ky <= 20 && std::abs(kx) <= (ky <= 4 ? 14 : (14 - (ky - 4) * 14 / 17))) {
                            if (std::abs(kx) >= 12 || ky == -20) {
                                r = 225; g = 230; b = 240; // Borda de prata
                            } else if (kx * kx + ky * ky <= 16) {
                                r = 240; g = 195; b = 45; // Coroa dourada central
                            } else {
                                r = 85; g = 20; b = 120; // Campo purpura real
                            }
                        }
                    }
                    // Elmo de Cavaleiro Polido com visor (tx: 86..116, ty: 50..86)
                    else if (tx >= 86 && tx <= 116 && ty >= 50 && ty <= 86) {
                        int hx = tx - 101, hy = ty - 68;
                        if (hx * hx + hy * hy <= 140) {
                            if (hy >= -2 && hy <= 2 && std::abs(hx) <= 9) {
                                r = 20; g = 20; b = 25; // Fenda dos olhos / visor
                            } else if (hx < -2 && hy < -2) {
                                r = 245; g = 250; b = 255; // Brilho de reflexo do aco
                            } else {
                                r = 180; g = 185; b = 195; // Aco de armadura
                            }
                        }
                    }
                    // Suporte de armas e alvo de treino na base (ty >= 96)
                    else if (ty >= 96) {
                        if (tx >= 44 && tx <= 84 && ty <= 104) {
                            r = 110; g = 70; b = 35; // Suporte de madeira
                        }
                    }
                    break;
                }
                case TexID::KingdomWood: {
                    // TEMA: Madeira polida e detalhada do castelo real.

                    // Tabuas de madeira
                    int tableY = ty / 16;
                    float vx = tx * 0.1f + fastYes(ty * 0.3f) * 2.0f;
                    float grain = fastYes(vx);
                    
                    bool isJoint = (ty % 16 < 2);
                    
                    int baseR = 100, baseG = 60, baseB = 30;
                    if (id == TexID::MorganaWood) { baseR = 60; baseG = 30; baseB = 50; }
                    if (id == TexID::BridgeWood) { baseR = 80; baseG = 70; baseB = 50; }
                    if (id == TexID::KingdomWood) { baseR = 130; baseG = 90; baseB = 40; }
                    
                    if (isJoint) {
                        r = baseR / 3; g = baseG / 3; b = baseB / 3;
                    } else {
                        int mod = (tableY % 2 == 0) ? 10 : -10;
                        r = std::clamp(baseR + mod + (int)(grain * 15), 0, 255);
                        g = std::clamp(baseG + mod + (int)(grain * 10), 0, 255);
                        b = std::clamp(baseB + mod + (int)(grain * 5), 0, 255);
                    }
                    break;
                }
                case TexID::ChurchAltar: {
                    // TEMA: Marmore branco e dourado compondo o altar sagrado.

                    // Tematica da Igreja (Vitral, Altar, Parede, Teto)
                    float details = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::ChurchStainedglass) {
                        // Coloridos vitrais em padrao de diamante
                        float diag = std::abs(fastYes((tx + ty)*0.1f) + fastYes((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::ChurchAltar || id == TexID::ChurchWallAltar) {
                        // Marmore com veios dourados
                        float vein = std::abs(fastYes(tx * 0.1f + ty * 0.2f + fastYes(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Marmore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        if (joint) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::ChurchWall: {
                    // TEMA: Paredes sagradas de pedra lisa da igreja.

                    // Tematica da Igreja (Vitral, Altar, Parede, Teto)
                    float details = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::ChurchStainedglass) {
                        // Coloridos vitrais em padrao de diamante
                        float diag = std::abs(fastYes((tx + ty)*0.1f) + fastYes((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::ChurchAltar || id == TexID::ChurchWallAltar) {
                        // Marmore com veios dourados
                        float vein = std::abs(fastYes(tx * 0.1f + ty * 0.2f + fastYes(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Marmore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        if (joint) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::ChurchWallAltar: {
                    // TEMA: A parede principal logo atras do altar, ricamente adornada.

                    // Tematica da Igreja (Vitral, Altar, Parede, Teto)
                    float details = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::ChurchStainedglass) {
                        // Coloridos vitrais em padrao de diamante
                        float diag = std::abs(fastYes((tx + ty)*0.1f) + fastYes((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::ChurchAltar || id == TexID::ChurchWallAltar) {
                        // Marmore com veios dourados
                        float vein = std::abs(fastYes(tx * 0.1f + ty * 0.2f + fastYes(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Marmore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        if (joint) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::ChurchCeiling: {
                    // TEMA: Teto em arco da igreja, desenhado com padroes arquitetonicos altos.

                    // Tematica da Igreja (Vitral, Altar, Parede, Teto)
                    float details = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::ChurchStainedglass) {
                        // Coloridos vitrais em padrao de diamante
                        float diag = std::abs(fastYes((tx + ty)*0.1f) + fastYes((tx - ty)*0.1f));
                        if (diag < 0.2f) { r = 10; g = 10; b = 10; } // chumbo do vitral
                        else {
                            int colorId = ((tx/16) + (ty/16)) % 3;
                            if (colorId == 0) { r = 200; g = 50; b = 50; }
                            if (colorId == 1) { r = 50; g = 100; b = 250; }
                            if (colorId == 2) { r = 250; g = 200; b = 50; }
                        }
                    } else if (id == TexID::ChurchAltar || id == TexID::ChurchWallAltar) {
                        // Marmore com veios dourados
                        float vein = std::abs(fastYes(tx * 0.1f + ty * 0.2f + fastYes(tx * 0.05f)*2.0f));
                        if (vein < 0.15f) { r = 230; g = 180; b = 50; } // Ouro
                        else { r = 240; g = 240; b = 245; } // Marmore
                    } else {
                        // Pedra clara sagrada
                        int bx = tx / 32; int by = ty / 32;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 32 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        if (joint) { r = 150; g = 150; b = 150; }
                        else { r = 210; g = 210; b = 215; }
                    }
                    break;
                }
                case TexID::PatioWall: {
                    // TEMA: Muros de pedra do patio, cobertos por finos musgos.

                    // Tijolos cobertos de musgo / Arvores densas
                    if (id == TexID::TreeForest) {
                        // Casca de arvore
                        float bark = fastYes(tx * 0.4f + fastYes(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        float moss = fastYes(tx * 0.1f) * fastYes(ty * 0.1f);
                        if (joint) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::ForestStructure: {
                    // TEMA: Ruinas esquecidas cobertas pela densa floresta.

                    // Tijolos cobertos de musgo / Arvores densas
                    if (id == TexID::TreeForest) {
                        // Casca de arvore
                        float bark = fastYes(tx * 0.4f + fastYes(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        float moss = fastYes(tx * 0.1f) * fastYes(ty * 0.1f);
                        if (joint) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::PatternStructure: {
                    // TEMA: Paredes de alvenaria abandonada generica.

                    // Tijolos cobertos de musgo / Arvores densas
                    if (id == TexID::TreeForest) {
                        // Casca de arvore
                        float bark = fastYes(tx * 0.4f + fastYes(ty * 0.1f)*3.0f);
                        int baseR = 60, baseG = 40, baseB = 20;
                        r = std::clamp(baseR + (int)(bark*15), 0, 255);
                        g = std::clamp(baseG + (int)(bark*10), 0, 255);
                        b = std::clamp(baseB + (int)(bark*5), 0, 255);
                    } else {
                        // Estrutura
                        int bx = tx / 32; int by = ty / 16;
                        if (by % 2 == 1) bx = (tx + 16) / 32;
                        bool joint = (tx % 32 < 2) || (ty % 16 < 2);
                        if (by % 2 == 1 && ((tx + 16) % 32 < 2)) joint = true;
                        
                        float moss = fastYes(tx * 0.1f) * fastYes(ty * 0.1f);
                        if (joint) { r = 40; g = 40; b = 40; }
                        else if (moss > 0.0f) {
                            r = 40; g = 100; b = 40; // Musgo
                        } else {
                            r = 110; g = 110; b = 110;
                        }
                    }
                    break;
                }
                case TexID::TreeHeart: {
                    // TEMA: Casca retorcida e viva das imensas arvores do coracao.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::TreeForest: {
                    // TEMA: Parede de limite de mapa da floresta densa (bosque continuo de multiplas arvores).
                    float ftx = static_cast<float>(tx);
                    float fty = static_cast<float>(ty);

                    // Curvatura organica dos troncos
                    float sway1 = fastYes(fty * 0.08f) * 3.5f;
                    float sway2 = fastYes(fty * 0.06f + 1.2f) * 4.0f;
                    float sway3 = fastYes(fty * 0.07f + 2.5f) * 3.5f;
                    
                    float cx1 = 22.0f + sway1;
                    float cx2 = 64.0f + sway2;
                    float cx3 = 106.0f + sway3;
                    
                    // Raizes alargadas na base
                    float flare1 = (fty > 90.0f) ? (fty - 90.0f) * 0.15f : 0.0f;
                    float flare2 = (fty > 85.0f) ? (fty - 85.0f) * 0.18f : 0.0f;
                    float flare3 = (fty > 95.0f) ? (fty - 95.0f) * 0.12f : 0.0f;
                    
                    float r1 = 9.0f + flare1;
                    float r2 = 12.0f + flare2;
                    float r3 = 8.5f + flare3;
                    
                    float d1 = std::abs(ftx - cx1);
                    float d2 = std::abs(ftx - cx2);
                    float d3 = std::abs(ftx - cx3);

                    // Troncos de segundo plano (mais escuros/profundos)
                    float cxBg1 = 43.0f + fastYes(fty * 0.05f) * 2.0f;
                    float cxBg2 = 86.0f + fastYes(fty * 0.05f + 1.8f) * 2.0f;
                    float dBg1 = std::abs(ftx - cxBg1);
                    float dBg2 = std::abs(ftx - cxBg2);
                    
                    // Galhos diagonais saindo dos troncos
                    bool isBranch = false;
                    float branchDist = 999.0f;
                    if (fty >= 22.0f && fty <= 55.0f) {
                        float prog = (55.0f - fty) / 33.0f;
                        float bx = 22.0f - prog * 18.0f;
                        float bDist = std::abs(ftx - bx);
                        float bRad = 4.0f * (1.0f - prog * 0.4f);
                        if (bDist <= bRad) { isBranch = true; branchDist = std::min(branchDist, bDist / bRad); }
                    }
                    if (fty >= 18.0f && fty <= 48.0f) {
                        float prog = (48.0f - fty) / 30.0f;
                        float bx = 64.0f + prog * 21.0f;
                        float bDist = std::abs(ftx - bx);
                        float bRad = 4.5f * (1.0f - prog * 0.4f);
                        if (bDist <= bRad) { isBranch = true; branchDist = std::min(branchDist, bDist / bRad); }
                    }
                    if (fty >= 16.0f && fty <= 45.0f) {
                        float prog = (45.0f - fty) / 29.0f;
                        float bx = 64.0f - prog * 20.0f;
                        float bDist = std::abs(ftx - bx);
                        float bRad = 4.0f * (1.0f - prog * 0.4f);
                        if (bDist <= bRad) { isBranch = true; branchDist = std::min(branchDist, bDist / bRad); }
                    }
                    if (fty >= 25.0f && fty <= 55.0f) {
                        float prog = (55.0f - fty) / 30.0f;
                        float bx = 106.0f + prog * 18.0f;
                        float bDist = std::abs(ftx - bx);
                        float bRad = 3.8f * (1.0f - prog * 0.4f);
                        if (bDist <= bRad) { isBranch = true; branchDist = std::min(branchDist, bDist / bRad); }
                    }

                    bool isFgTrunk = (fty >= 35.0f && (d1 <= r1 || d2 <= r2 || d3 <= r3)) || isBranch;
                    bool isBgTrunk = (!isFgTrunk && fty >= 28.0f && (dBg1 <= 5.5f || dBg2 <= 5.5f));

                    // Copas e folhas densas (clusters de folhagem espalhados pelo topo e meio)
                    float leafCoverage = 0.0f;
                    
                    struct WallPuff { float x, y, rx, ry; };
                    WallPuff wallPuffs[] = {
                        { 22.0f, 18.0f, 26.0f, 18.0f },
                        { 64.0f, 15.0f, 30.0f, 20.0f },
                        { 106.0f, 20.0f, 26.0f, 18.0f },
                        { 0.0f, 28.0f, 22.0f, 16.0f },
                        { 128.0f, 28.0f, 22.0f, 16.0f },
                        { 44.0f, 32.0f, 24.0f, 17.0f },
                        { 86.0f, 34.0f, 25.0f, 17.0f },
                        { 20.0f, 48.0f, 22.0f, 15.0f },
                        { 64.0f, 44.0f, 24.0f, 16.0f },
                        { 108.0f, 48.0f, 22.0f, 15.0f },
                        { 42.0f, 62.0f, 18.0f, 13.0f },
                        { 88.0f, 64.0f, 18.0f, 13.0f }
                    };

                    float puffShading = 1.0f;
                    for (const auto& p : wallPuffs) {
                        float dx = ftx - p.x;
                        float dy = fty - p.y;
                        float dist = (dx * dx) / (p.rx * p.rx) + (dy * dy) / (p.ry * p.ry);
                        if (dist < 1.0f) {
                            float coverage = (1.0f - dist);
                            if (coverage > leafCoverage) {
                                leafCoverage = coverage;
                                puffShading = 1.0f - (dy / p.ry) * 0.45f - (std::abs(dx) / p.rx) * 0.25f;
                            }
                        }
                    }

                    if (fty < 28.0f) {
                        float topDensity = (28.0f - fty) / 28.0f;
                        if (topDensity > leafCoverage) {
                            leafCoverage = std::max(leafCoverage, topDensity);
                            puffShading = std::max(puffShading, 0.85f + 0.3f * (1.0f - fty / 28.0f));
                        }
                    }

                    float leafNoise = fastYes(ftx * 0.55f + fty * 0.45f) * 1.2f + fastYes(ftx * 1.2f - fty * 0.7f) * 0.6f + fastYes(ftx * 2.1f + fty * 1.5f) * 0.3f;
                    bool isForegroundLeaf = (leafCoverage > 0.35f && (fty < 38.0f || (fty < 65.0f && !isFgTrunk)));
                    
                    if (isFgTrunk && !isForegroundLeaf) {
                        float normX = (ftx - cx2) / r2;
                        if (d1 <= r1) { normX = (ftx - cx1) / r1; }
                        else if (d3 <= r3) { normX = (ftx - cx3) / r3; }
                        else if (isBranch) { normX = branchDist; }

                        normX = std::clamp(normX, -1.0f, 1.0f);
                        float cylLight = 1.0f - (normX * normX) * 0.55f;

                        float barkNoise = fastYes(ftx * 0.6f + fastYes(fty * 0.12f) * 4.0f) * 1.2f + fastYes(ftx * 1.4f) * 0.5f;
                        float mossNoise = fastYes(ftx * 0.25f + fty * 0.15f) * 1.5f + fastYes(ftx * 0.5f - fty * 0.3f) * 0.6f;
                        bool hasMoss = (fty > 75.0f && mossNoise > 0.1f) || (fty > 105.0f);

                        if (hasMoss) {
                            float mossFactor = std::clamp((fty - 75.0f) / 45.0f + mossNoise * 0.3f, 0.0f, 1.0f);
                            int mR = (int)((35 + barkNoise * 8) * cylLight);
                            int mG = (int)((85 + mossNoise * 20 + barkNoise * 10) * cylLight);
                            int mB = (int)((30 + barkNoise * 6) * cylLight);
                            
                            int bR = (int)((68 + barkNoise * 14) * cylLight);
                            int bG = (int)((44 + barkNoise * 10) * cylLight);
                            int bB = (int)((26 + barkNoise * 6) * cylLight);

                            r = std::clamp((int)(bR * (1.0f - mossFactor) + mR * mossFactor), 0, 255);
                            g = std::clamp((int)(bG * (1.0f - mossFactor) + mG * mossFactor), 0, 255);
                            b = std::clamp((int)(bB * (1.0f - mossFactor) + mB * mossFactor), 0, 255);
                        } else {
                            r = std::clamp((int)((72 + barkNoise * 16) * cylLight), 0, 255);
                            g = std::clamp((int)((46 + barkNoise * 11) * cylLight), 0, 255);
                            b = std::clamp((int)((28 + barkNoise * 7) * cylLight), 0, 255);
                        }
                    }
                    else if (isBgTrunk && leafCoverage < 0.5f) {
                        float normX = (dBg1 <= 5.5f) ? (ftx - cxBg1) / 5.5f : (ftx - cxBg2) / 5.5f;
                        float cylLight = 0.65f - (normX * normX) * 0.25f;
                        float barkNoise = fastYes(ftx * 0.7f + fty * 0.2f);
                        r = std::clamp((int)((45 + barkNoise * 10) * cylLight), 0, 255);
                        g = std::clamp((int)((30 + barkNoise * 7) * cylLight), 0, 255);
                        b = std::clamp((int)((18 + barkNoise * 4) * cylLight), 0, 255);
                    }
                    else if (leafCoverage > 0.05f) {
                        float shade = std::clamp(puffShading + leafNoise * 0.25f, 0.4f, 1.4f);
                        float heightFactor = 1.0f - (fty / 100.0f) * 0.35f;
                        float finalLight = shade * heightFactor;

                        int baseR = 32, baseG = 95, baseB = 34;
                        if (finalLight > 1.05f) {
                            baseR = 58; baseG = 145; baseB = 52;
                        } else if (finalLight < 0.65f) {
                            baseR = 15; baseG = 52; baseB = 18;
                        }

                        r = std::clamp((int)((baseR + leafNoise * 12) * finalLight), 0, 255);
                        g = std::clamp((int)((baseG + leafNoise * 20) * finalLight), 0, 255);
                        b = std::clamp((int)((baseB + leafNoise * 10) * finalLight), 0, 255);
                    }
                    else {
                        float bgNoise = fastYes(ftx * 0.4f + fty * 0.4f);
                        if (fty > 105.0f) {
                            r = std::clamp((int)(28 + bgNoise * 8), 0, 255);
                            g = std::clamp((int)(55 + bgNoise * 14), 0, 255);
                            b = std::clamp((int)(22 + bgNoise * 6), 0, 255);
                        } else {
                            r = std::clamp((int)(12 + bgNoise * 4), 0, 255);
                            g = std::clamp((int)(32 + bgNoise * 8), 0, 255);
                            b = std::clamp((int)(14 + bgNoise * 4), 0, 255);
                        }
                    }
                    break;
                }
                case TexID::StoneVillage:
                case TexID::StoneSpawn:
                case TexID::CaveHeartWall: {
                    // TEMA: Paredes construA-das com pedras arredondadas (Cobblestone HD).
                    
                    float stoneSize = 24.0f;
                    float sx = tx / stoneSize;
                    float sy = ty / stoneSize;
                    
                    // Adiciona ruido as coordenadas para tornar as pedras irregulares
                    float noiseX = fastYes(ty * 0.15f) * 0.5f;
                    float noiseY = fastYes(tx * 0.15f) * 0.5f;
                    
                    int cx = (int)(sx + noiseX);
                    int cy = (int)(sy + noiseY);
                    
                    float dx = (sx + noiseX) - (cx + 0.5f);
                    float dy = (sy + noiseY) - (cy + 0.5f);
                    
                    float dist = std::sqrt(dx*dx + dy*dy);
                    
                    bool isBorder = dist > 0.40f;
                    
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f) + fastYes((tx + ty) * 0.5f);
                    int stoneR = 100, stoneG = 100, stoneB = 100;
                    if (id == TexID::CaveHeartWall) { stoneR = 60; stoneG = 30; stoneB = 80; }
                    
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
                        
                        r = std::clamp((int)(stoneR + details * 15.0f + highlight + shadow), 0, 255);
                        g = std::clamp((int)(stoneG + details * 15.0f + highlight + shadow), 0, 255);
                        b = std::clamp((int)(stoneB + details * 15.0f + highlight + shadow), 0, 255);
                    }
                    break;
                }
                case TexID::RoomBossWall: {
                    // TEMA: Rochas negras e afiadas isolando a furia do chefe.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::DarkBricks: {
                    // TEMA: Paredes rústicas de chalés e casas da vila em tijolos de terracota aquecida e cantaria.
                    int brickW = 32;
                    int brickH = 16;
                    int by = ty / brickH;
                    int bx = tx / brickW;
                    if (by % 2 == 1) bx = (tx + brickW / 2) / brickW;

                    int lx = (by % 2 == 1) ? ((tx + brickW / 2) % brickW) : (tx % brickW);
                    int ly = ty % brickH;

                    bool isJoint = (lx < 2) || (ly < 2);

                    // Variacao artesanal de queima no forno de cada tijolo
                    int hash = ((bx * 41 + by * 19) ^ ((bx + 7) * (by + 13))) & 0xFF;
                    int brickType = hash % 4;
                    int varR = 0, varG = 0, varB = 0;
                    if (brickType == 0) { varR = 16; varG = -4; varB = -8; }       // Terracota queimada profunda
                    else if (brickType == 1) { varR = -12; varG = -8; varB = -6; } // Barro envelhecido rústico
                    else if (brickType == 2) { varR = 8; varG = 12; varB = 6; }    // Argila clara dourada
                    else { varR = ((hash % 15) - 7); varG = ((hash % 11) - 5); varB = ((hash % 9) - 4); }

                    // Granulação mineral e porosidade da argila cozida
                    float grain = fastYes(tx * 0.45f) * fastYes(ty * 0.45f) * 10.0f + fastCos((tx + ty) * 0.3f) * 6.0f;
                    float sandNoise = fastYes(tx * 0.8f + ty * 0.8f) * 4.0f;

                    if (isJoint) {
                        // Argamassa rústica de areia e cal
                        int mortar = 48 + (int)sandNoise;
                        r = std::clamp(mortar + 8, 0, 255);
                        g = std::clamp(mortar + 3, 0, 255);
                        b = std::clamp(mortar - 2, 0, 255);
                    } else {
                        int baseR = 152 + varR + (int)grain;
                        int baseG = 88 + varG + (int)(grain * 0.7f);
                        int baseB = 64 + varB + (int)(grain * 0.5f);

                        // Realce de chanfro e iluminação 3D superior e lateral
                        if (ly == 2 || ly == 3) { baseR += 26; baseG += 18; baseB += 12; }
                        if (lx == 2 || lx == 3) { baseR += 18; baseG += 12; baseB += 8; }
                        // Sombreamento 3D inferior e lateral
                        if (ly == brickH - 1 || ly == brickH - 2) { baseR -= 28; baseG -= 20; baseB -= 16; }
                        if (lx == brickW - 1 || lx == brickW - 2) { baseR -= 18; baseG -= 14; baseB -= 10; }

                        // Pátina rústica na base da parede
                        if (ty >= 118) {
                            baseR -= 15; baseG -= 5; baseB -= 8;
                        }

                        r = std::clamp(baseR, 0, 255);
                        g = std::clamp(baseG, 0, 255);
                        b = std::clamp(baseB, 0, 255);
                    }
                    break;
                }
                case TexID::KingdomStone: {
                    // TEMA: Tijolos e cantaria de pedra cinza medieval nobre do Reino e Castelo.
                    int brickW = 32;
                    int brickH = 16;
                    int by = ty / brickH;
                    int bx = tx / brickW;
                    if (by % 2 == 1) bx = (tx + brickW / 2) / brickW;

                    int lx = (by % 2 == 1) ? ((tx + brickW / 2) % brickW) : (tx % brickW);
                    int ly = ty % brickH;

                    bool isJoint = (lx < 2) || (ly < 2);

                    int hash = ((bx * 37 + by * 17) ^ ((bx + 3) * (by + 7))) & 0xFF;
                    int varR = (hash % 21) - 10;
                    int varG = ((hash * 3) % 21) - 10;
                    int varB = ((hash * 7) % 25) - 12;

                    float grain = fastYes(tx * 0.4f) * fastYes(ty * 0.4f) * 10.0f + fastCos((tx + ty) * 0.25f) * 6.0f;

                    if (isJoint) {
                        r = 42; g = 42; b = 46; // Rejunte escuro de argamassa
                    } else {
                        int baseR = 124 + varR + (int)grain;
                        int baseG = 126 + varG + (int)grain;
                        int baseB = 136 + varB + (int)grain;

                        // Bevel 3D iluminação nas bordas superiores/esquerdas
                        if (ly == 2 || ly == 3) { baseR += 22; baseG += 22; baseB += 24; }
                        if (lx == 2 || lx == 3) { baseR += 16; baseG += 16; baseB += 18; }
                        // Sombreamento nas bordas inferiores/direitas
                        if (ly == brickH - 1 || ly == brickH - 2) { baseR -= 22; baseG -= 22; baseB -= 24; }
                        if (lx == brickW - 1 || lx == brickW - 2) { baseR -= 16; baseG -= 16; baseB -= 18; }

                        r = std::clamp(baseR, 0, 255);
                        g = std::clamp(baseG, 0, 255);
                        b = std::clamp(baseB, 0, 255);
                    }
                    break;
                }
                case TexID::BridgeStone: {
                    // TEMA: Blocos macicos de pedra cinza medieval reforçada da Ponte do Reino.
                    int blockW = 42;
                    int blockH = 21;
                    int by = ty / blockH;
                    int bx = tx / blockW;
                    if (by % 2 == 1) bx = (tx + blockW / 2) / blockW;

                    int lx = (by % 2 == 1) ? ((tx + blockW / 2) % blockW) : (tx % blockW);
                    int ly = ty % blockH;

                    bool isJoint = (lx < 2) || (ly < 2);

                    int hash = ((bx * 43 + by * 23) ^ ((bx + 5) * (by + 11))) & 0xFF;
                    int varR = (hash % 23) - 11;
                    int varG = ((hash * 5) % 23) - 11;
                    int varB = ((hash * 9) % 25) - 12;

                    float stoneGrain = fastYes(tx * 0.35f) * fastCos(ty * 0.35f) * 12.0f + fastYes((tx - ty) * 0.2f) * 8.0f;

                    if (isJoint) {
                        r = 38; g = 38; b = 42;
                    } else {
                        int baseR = 115 + varR + (int)stoneGrain;
                        int baseG = 118 + varG + (int)stoneGrain;
                        int baseB = 128 + varB + (int)stoneGrain;

                        if (ly == 2 || ly == 3) { baseR += 24; baseG += 24; baseB += 26; }
                        if (lx == 2 || lx == 3) { baseR += 18; baseG += 18; baseB += 20; }
                        if (ly == blockH - 1 || ly == blockH - 2) { baseR -= 24; baseG -= 24; baseB -= 26; }
                        if (lx == blockW - 1 || lx == blockW - 2) { baseR -= 18; baseG -= 18; baseB -= 20; }

                        r = std::clamp(baseR, 0, 255);
                        g = std::clamp(baseG, 0, 255);
                        b = std::clamp(baseB, 0, 255);
                    }
                    break;
                }
                case TexID::WallInvalidates: {
                    // TEMA: Fallback para texturas nao encontradas.

                    // Padrao HD generico para outros itens nao especificados
                    float pattern = fastYes(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                case TexID::FloorLabyrinthEdge: {
                    // TEMA: Borda do chao do labirinto, misturando pedra com terra umida.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::FloorLabyrinth || id == TexID::FloorLabyrinthEdge) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::FloorEarth) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao generico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::FloorLabyrinth: {
                    // TEMA: Ladrilhos de pedra quebrados do labirinto.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::FloorLabyrinth || id == TexID::FloorLabyrinthEdge) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::FloorEarth) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao generico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::FloorRoomBossOut: {
                    // TEMA: Solo de pedra escura antes da entrada do chefe.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::FloorRoomBossInside: {
                    // TEMA: Piso vulcanico e irregular na arena do chefe.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::FloorHeartMoss: {
                    // TEMA: Chao coberto de musgo espesso que pulsa vida.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::FloorHeartEarth: {
                    // TEMA: Terra fertil, espiralada e macia da area do coracao.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::FloorHeartDark: {
                    // TEMA: Raizes mortas e terra petrificada nas sombras.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::FloorGrassForest: {
                    // TEMA: Grama verde e realista da floresta iluminada por raios solares.

                    float nx = tx * 0.15f;
                    float ny = ty * 0.15f;
                    float macroNoise = fastYes(nx * 0.2f) * fastYes(ny * 0.3f);
                    float clumpNoise = fastYes(nx + ny * 0.5f) + fastYes(ny - nx * 0.5f);
                    float bladeNoise = fastYes(tx * 0.8f + ty * 1.5f) * fastYes(tx * 1.2f - ty * 0.5f);
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
                case TexID::FloorGrassVillage: {
                    // TEMA: Grama vibrante, macia e aparada da pacifica vila.

                    float nx = tx * 0.15f;
                    float ny = ty * 0.15f;
                    float macroNoise = fastYes(nx * 0.3f) * fastYes(ny * 0.2f);
                    float clumpNoise = fastYes(nx * 0.8f + ny * 0.4f) + fastYes(ny * 0.8f - nx * 0.4f);
                    float bladeNoise = fastYes(tx * 1.1f + ty * 1.8f) * fastYes(tx * 1.5f - ty * 0.9f);
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
                case TexID::FloorEarth: {
                    // TEMA: Caminho de terra seca, desgastado pela passagem de viajantes.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::FloorLabyrinth || id == TexID::FloorLabyrinthEdge) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::FloorEarth) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao generico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::FloorPattern: {
                    // TEMA: Solo basico de poeira e pedregulhos finos.

                    // Chaos (Labirinto, Terra, Padrao)
                    float noise = fastYes(tx * 0.2f) * fastYes(ty * 0.2f);
                    if (id == TexID::FloorLabyrinth || id == TexID::FloorLabyrinthEdge) {
                        bool tile = (tx / 64) % 2 == (ty / 64) % 2;
                        if (tile) { r = 120; g = 120; b = 125; } else { r = 100; g = 100; b = 105; }
                    } else if (id == TexID::FloorEarth) {
                        // Detalhes de terra ressecada
                        int base = 80 + (int)(noise * 20);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base - 30, 0, 255);
                        b = std::clamp(base - 50, 0, 255);
                    } else {
                        // Chao Padrao generico poeira
                        int base = 60 + (int)(noise * 15);
                        r = std::clamp(base, 0, 255);
                        g = std::clamp(base, 0, 255);
                        b = std::clamp(base, 0, 255);
                    }
                    break;
                }
                case TexID::CeilingIndoorsHeartMoss: {
                    // TEMA: Teto abobadado de rocha incrustado de musgos biologicoscentes.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::CeilingIndoorsHeartWood: {
                    // TEMA: Raizes gigantescas se entrelacando no teto acima.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::CeilingIndoorsHeartDark: {
                    // TEMA: Teto cavernoso e negro onde a luz mal consegue alcancar.

                    // Ambientes Corruptos/Vivos (Coracao, Chefe)
                    float distCenter = std::sqrt((tx-64.0f)*(tx-64.0f) + (ty-64.0f)*(ty-64.0f));
                    float spiral = fastYes(distCenter * 0.1f + fastYes(tx * 0.05f) * 3.0f);
                    float details = fastYes(tx * 0.3f) * fastYes(ty * 0.3f);
                    
                    if (id == TexID::TreeHeart || id == TexID::CeilingIndoorsHeartWood) {
                        // Raizes espirais e escuras pulsantes
                        if (spiral > 0.0f) { r = 60; g = 20; b = 20; }
                        else { r = 40; g = 15; b = 15; }
                    } else if (id == TexID::FloorHeartMoss || id == TexID::CeilingIndoorsHeartMoss) {
                        // Musgo bio-luminescente e vivo
                        if (details > 0.3f) { r = 50; g = 180; b = 80; }
                        else { r = 30; g = 100; b = 40; }
                    } else if (id == TexID::RoomBossWall || id == TexID::FloorRoomBossInside || id == TexID::FloorRoomBossOut) {
                        // Rocha vulcanica vulcanica rasgada
                        bool crack = std::abs(fastYes(tx*0.2f + ty*0.1f + fastYes(tx*0.1f)*5.0f)) < 0.1f;
                        if (crack) { r = 250; g = 100; b = 20; } // Lava
                        else {
                            int dark = 30 + (int)(details * 20);
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
                case TexID::CeilingIndoorsPattern: {
                    // TEMA: Um teto generico e plano de pedra rustica para interiores.

                    // Padrao HD generico para outros itens nao especificados
                    float pattern = fastYes(tx * 0.2f) * fastCos(ty * 0.2f);
                    int base = 128 + (int)(pattern * 50);
                    r = std::clamp(base, 0, 255);
                    g = std::clamp(base, 0, 255);
                    b = std::clamp(base, 0, 255);
                    break;
                }
                default: {
                    // TEMA: Textura de depuracao para TexIDs nao implementados
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
