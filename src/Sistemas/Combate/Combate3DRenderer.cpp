#include "Combate3DRenderer.h"
#include "../Raycaster/Raycaster.h"
#include "../Raycaster/RaycasterSprites.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Interface/Telas/Combate/TelaCombate.h"
#include <algorithm>
#include <cctype>
#include <cmath>

extern Personagem* g_inimigoAtacanteParry;

// ═══════════════════════════════════════════════════════════════════
//  Arena de Combate por Bioma
// ═══════════════════════════════════════════════════════════════════
std::vector<std::string> Combate3DRenderer::obterArenaPorTitulo(const std::string& titulo) {
    std::string upper = titulo;
    for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

    if (upper.find("CHEFE") != std::string::npos) {
        return {
            "  .........................  ",
            " .............................  ",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            " ............................. ",
            "  ...........................  "
        };
    }
    if (upper.find("LABIRINTO") != std::string::npos) {
        return {
            "=================================",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "================================="
        };
    }
    if (upper.find("CAVERNA") != std::string::npos || upper.find("CORACAO") != std::string::npos) {
        return {
            "################################",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "################################"
        };
    }
    if (upper.find("PATIO DO REINO") != std::string::npos || upper.find("REINO") != std::string::npos) {
        return {
            "#|||||||||||||||||||||||||||||||#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#|||||||||||||||||||||||||||||||#"
        };
    }
    if (upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos) {
        return {
            "################################",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "#..............................#",
            "################################"
        };
    }
    // Vila / Inicio / Default
    return {
        "####_[]_########################",
        "#..............................#",
        "#..............................#",
        "#..............................#",
        "#..............................#",
        "#..............................#",
        "#..............................#",
        "#..............................#",
        "################################"
    };
}

// ═══════════════════════════════════════════════════════════════════
//  Cor base do sprite do inimigo (mesmas cores do RaycasterInimigos)
// ═══════════════════════════════════════════════════════════════════
std::tuple<int,int,int> Combate3DRenderer::obterCorSpriteInimigo(Personagem* inimigo) {
    if (!inimigo) return {255, 255, 255};
    std::string nome = inimigo->obterRaca()->obterNomeRaca();
    std::string upper = nome;
    for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

    if (upper.find("GOBLIN") != std::string::npos) return {100, 200, 50};
    if (upper.find("ORK") != std::string::npos || upper.find("ORC") != std::string::npos) return {50, 150, 50};
    if (upper.find("SLIME") != std::string::npos) return {50, 200, 255};
    if (upper.find("FADA") != std::string::npos) return {255, 100, 200};
    if (upper.find("ABOMINACAO") != std::string::npos || upper.find("ABOMINA") != std::string::npos) return {139, 69, 19};
    if (upper.find("TROLL") != std::string::npos) return {150, 150, 160};
    if (upper.find("MIMICO") != std::string::npos) return {200, 150, 50};
    if (upper.find("MAHORAGA") != std::string::npos) return {255, 255, 255};
    return {200, 200, 200}; // Default cinza claro
}

// ═══════════════════════════════════════════════════════════════════
//  Pintar texto no buffer 1D (overlay)
// ═══════════════════════════════════════════════════════════════════
void Combate3DRenderer::pintarTextoNoBuffer(std::vector<std::string>& tela, int larguraTela, int alturaMax, int posX, int posY, const std::string& texto, const std::string& corFg, const std::string& corBgOverride) {
    if (posY < 0 || posY >= alturaMax) return;
    std::string textoLimpo = Aparencia::removerCoresANSI(texto);
    
    // Helper para extrair o background da celula do Raycaster
    auto getBg = [](const std::string& s) {
        size_t pos = s.find("\033[48;2;");
        if (pos != std::string::npos) {
            size_t end = s.find('m', pos);
            if (end != std::string::npos) return s.substr(pos, end - pos + 1);
        }
        return std::string("\033[48;2;0;0;0m");
    };

    for (int i = 0; i < static_cast<int>(textoLimpo.length()); ++i) {
        int tx = posX + i;
        if (tx >= 0 && tx < larguraTela) {
            std::string bg = corBgOverride.empty() ? getBg(tela[posY * larguraTela + tx]) : corBgOverride;
            tela[posY * larguraTela + tx] = bg + corFg + std::string(1, textoLimpo[i]) + "\033[0m";
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
//  Renderizar Quadro Principal
// ═══════════════════════════════════════════════════════════════════
std::vector<std::string> Combate3DRenderer::renderizarQuadro(
    const std::string& tituloMapa, 
    Personagem* jogador, 
    const std::vector<Personagem*>& inimigos,
    Personagem* alvoAnimacao,
    int frame,
    int /*framesDeDanoJogador*/,
    int danoAmount,
    bool isCura,
    int tempoMs,
    bool isMorte,
    const std::vector<std::string>& dropsAnimacao,
    float spriteOpacity
) {
    // Gera uma arena dedicada ao bioma
    std::vector<std::string> arena = obterArenaPorTitulo(tituloMapa);
    
    // Posicao fixa: centro da arena, olhando para Norte
    float jX = static_cast<float>(arena[0].size()) / 2.0f;
    float jY = static_cast<float>(arena.size()) - 2.0f;
    float angulo = -1.57f; // Olhando pro Norte

    int larguraTela = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();
    if (larguraTela <= 0) larguraTela = 120;
    if (alturaTerminal <= 0) alturaTerminal = 40;

    int alturaHUD = 0;
    int altura3D = std::max(10, alturaTerminal - alturaHUD);

    // Renderiza o fundo 3D em um vetor achatado (LARGURA x ALTURA) com altura reduzida
    static std::vector<std::string> s_cachedBackground;
    static std::string s_cachedTituloMapa;
    static int s_cachedLarguraTela = 0;
    static int s_cachedAltura3D = 0;

    if (s_cachedBackground.empty() || s_cachedTituloMapa != tituloMapa || s_cachedLarguraTela != larguraTela || s_cachedAltura3D != altura3D) {
        s_cachedBackground = Raycaster::desenharQuadroEstatico3D(arena, jX, jY, angulo, tituloMapa, jogador, altura3D);
        s_cachedTituloMapa = tituloMapa;
        s_cachedLarguraTela = larguraTela;
        s_cachedAltura3D = altura3D;
    }

    std::vector<std::string> tela = s_cachedBackground;

    // Sobrepoe os inimigos
    int numInimigos = static_cast<int>(inimigos.size());
    for (int i = 0; i < numInimigos; ++i) {
        Personagem* inimigo = inimigos[i];
        if (inimigo && (inimigo->obterVida() > 0 || !inimigo->obterMorteAnimada())) {
            bool isAnimado = (alvoAnimacao != nullptr && inimigo == alvoAnimacao);
            int framesDano = (isAnimado && danoAmount > 0 && !isMorte) ? frame : 0;
            bool isMorteIni = (isMorte && isAnimado);
            int frameMorteIni = isMorteIni ? frame : 0;
            bool isSel = (TelaCombate::selecaoAlvoAtual == i);
            
            sobreporSprite(tela, inimigo, i, numInimigos, larguraTela, altura3D, framesDano, danoAmount, isCura, tempoMs, isMorteIni, frameMorteIni, dropsAnimacao, isSel, spriteOpacity);
        }
    }

    // Retorna a tela com a altura correspondente à cena 3D (o HUD clássico será impresso abaixo por TelaCombate)
    std::vector<std::string> linhasRenderizadas(altura3D);
    for (int y = 0; y < altura3D; y++) {
        std::string linha = "";
        for (int x = 0; x < larguraTela; x++) {
            linha += tela[y * larguraTela + x];
        }
        linhasRenderizadas[y] = std::move(linha);
    }

    return linhasRenderizadas;
}

// ═══════════════════════════════════════════════════════════════════
//  Sobrepor Sprite do Inimigo (com arte 3D texturizada)
// ═══════════════════════════════════════════════════════════════════
void Combate3DRenderer::sobreporSprite(
    std::vector<std::string>& tela, 
    Personagem* inimigo, 
    int inimigoIdx,
    int totalInimigos,
    int larguraTela, 
    int alturaVisivel, 
    int flashDanoInimigo, 
    int danoAmount, 
    bool isCura, 
    int tempoMs, 
    bool isMorte, 
    int frameMorte, 
    const std::vector<std::string>& dropsAnimacao, 
    bool isSelecionado,
    float spriteOpacity
) {
    // Usa a arte de MAPA (mesma do raycaster) em vez da arte de combate 2D
    const std::vector<std::string>& arteOriginalInimigo = inimigo->obterRaca()->obterAparenciaRaca();
    if (arteOriginalInimigo.empty()) return;

    (void)dropsAnimacao;

    double progresso = 0.0;
    int totalFramesMorte = 0;
    if (isMorte) {
        totalFramesMorte = 12; // 12 frames em 3D
        progresso = std::min(1.0, static_cast<double>(frameMorte) / totalFramesMorte);
    }

    // Obtem cor base para texturizacao estilo raycaster
    auto [baseR, baseG, baseB] = obterCorSpriteInimigo(inimigo);

    std::vector<std::string> arteUsada = arteOriginalInimigo;
    
    int alturaArte = static_cast<int>(arteUsada.size());
    int fator = 1;
    if (alturaArte > alturaVisivel * 0.7f) {
        fator = 2;
    }
    if (inimigo->obterNome().find("Mahoraga") != std::string::npos) {
        fator = std::max(2, fator + 1);
    }
    if (fator > 1) {
        arteUsada = Aparencia::reduzirEscalaAscii(arteUsada, fator, fator);
        alturaArte = static_cast<int>(arteUsada.size());
    }

    int larguraArte = 0;
    for (const auto& linha : arteUsada) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > larguraArte) larguraArte = comp;
    }

    // Sway horizontal based on time (apenas se não estiver no meio da animação de morte)
    int swayOff = 0;
    if (!isMorte) {
        int stepSway = (tempoMs / 200) % 8;
        if (stepSway < 0) stepSway += 8;
        int swayPattern[] = {0, 1, 2, 1, 0, -1, -2, -1};
        swayOff = swayPattern[stepSway] * 2;
    }
    
    int startY = (alturaVisivel - alturaArte) / 2;
    startY += alturaVisivel / 10;
    
    int startX = 0; // Será recalculado após obter o croppedWidth

    // Helper para extrair o background da celula do Raycaster
    auto getBg = [](const std::string& s) {
        size_t pos = s.find("\033[48;2;");
        if (pos != std::string::npos) {
            size_t end = s.find('m', pos);
            if (end != std::string::npos) return s.substr(pos, end - pos + 1);
        }
        return std::string("\033[48;2;0;0;0m");
    };

    auto parseAnsiRGB = [](const std::string& str) -> std::tuple<int,int,int> {
        int r = 0, g = 0, b = 0;
        size_t pos = str.find("48;2;");
        if (pos == std::string::npos) {
            pos = str.find("38;2;");
        }
        if (pos != std::string::npos) {
            pos += 5;
            while (pos < str.size() && str[pos] >= '0' && str[pos] <= '9') {
                r = r * 10 + (str[pos] - '0');
                pos++;
            }
            if (pos < str.size() && str[pos] == ';') {
                pos++;
                while (pos < str.size() && str[pos] >= '0' && str[pos] <= '9') {
                    g = g * 10 + (str[pos] - '0');
                    pos++;
                }
                if (pos < str.size() && str[pos] == ';') {
                    pos++;
                    while (pos < str.size() && str[pos] >= '0' && str[pos] <= '9') {
                        b = b * 10 + (str[pos] - '0');
                        pos++;
                    }
                }
            }
        }
        return {r, g, b};
    };

    // Texturiza cada caractere da arte com cores baseadas no caractere
    auto obterTgtRGB = [&](char c, int /*rx*/, int ry) -> std::tuple<int,int,int> {
        int currentBaseR = baseR;
        int currentBaseG = baseG;
        int currentBaseB = baseB;
        if (inimigo->obterNome().find("Mahoraga") != std::string::npos && (ry * fator) < 24) {
            currentBaseR = 255;
            currentBaseG = 215;
            currentBaseB = 0; // Amarelo/Dourado
        }
        int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
        if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; }
        else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; }
        else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; }
        else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); }
        else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); }
        else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; }
        
        if (flashDanoInimigo > 0 && flashDanoInimigo % 2 == 0) {
            if (isCura) {
                rMod = 50; gMod = 255; bMod = 50;
            } else {
                rMod = 255; gMod = 50; bMod = 50;
            }
        } else if (inimigo == g_inimigoAtacanteParry) {
            rMod = 255; gMod = 140; bMod = 0;
        } else if (isSelecionado) {
            if (TelaCombate::piscarSelecao) {
                rMod = (rMod + 255) / 2;
                gMod = (gMod + 255) / 2;
                bMod = bMod / 2;
            } else {
                int cinza = (rMod + gMod + bMod) / 3;
                rMod = (rMod + cinza) / 2;
                gMod = (gMod + cinza) / 2;
                bMod = (bMod + cinza) / 2;
            }
        }

        if (isMorte) {
            double fade = 1.0 - progresso;
            rMod = static_cast<int>(rMod * fade);
            gMod = static_cast<int>(gMod * fade);
            bMod = static_cast<int>(bMod * fade);
        }
        
        return {rMod, gMod, bMod};
    };

    // Auto-crop horizontal
    int minX = larguraArte, maxX = 0;
    for (const auto& linha : arteUsada) {
        std::string limpo = Aparencia::removerCoresANSI(linha);
        for (int i = 0; i < static_cast<int>(limpo.length()); ++i) {
            if (limpo[i] != ' ') {
                if (i < minX) minX = i;
                if (i > maxX) maxX = i;
            }
        }
    }
    if (minX > maxX) { minX = 0; maxX = larguraArte - 1; }
    int croppedWidth = maxX - minX + 1;
    
    int larguraColuna = larguraTela / totalInimigos;
    int centroColunaX = inimigoIdx * larguraColuna + larguraColuna / 2;
    startX = centroColunaX - croppedWidth / 2 + swayOff;

    bool desenharCorpo = (inimigo->obterVida() > 0 || !inimigo->obterMorteAnimada());

    if (desenharCorpo) {
        // Desenha contorno preto (borda do sprite) + corpo texturizado (mesclando fundo via spriteOpacity)
        for (int y = 0; y < alturaArte; y++) {
            int telaY = startY + y;
            if (telaY >= 0 && telaY < alturaVisivel) {
                std::string linhaSemCor = Aparencia::removerCoresANSI(arteUsada[y]);
                
                for (int rawX = minX; rawX <= maxX; rawX++) {
                    int x = rawX - minX;
                    int telaX = startX + x;
                    if (telaX >= 0 && telaX < larguraTela && rawX < static_cast<int>(linhaSemCor.length())) {
                        char c = linhaSemCor[rawX];
                        
                        if (c != ' ') {
                            if (isMorte) {
                                // Efeito de desintegração dithered (virando poeira)
                                int hash = (rawX * 37 + y * 57) % 100;
                                if (hash < progresso * 100) {
                                    // Renderiza partículas de poeira '.' flutuantes ou some o pixel
                                    if (progresso < 0.8 && (hash % 3 == 0)) {
                                        std::string bg = getBg(tela[telaY * larguraTela + telaX]);
                                        int dustIntensity = static_cast<int>(100 * (1.0 - progresso));
                                        std::string corDust = "\033[38;2;" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + "m";
                                        tela[telaY * larguraTela + telaX] = bg + corDust + "." + "\033[0m";
                                    }
                                    continue;
                                }
                            }

                            // Checa se eh borda (pixel adjacente a espaço ou borda da arte)
                            bool isEdge = false;
                            for (int dy = -1; dy <= 1; ++dy) {
                                for (int dx = -1; dx <= 1; ++dx) {
                                      if (dx == 0 && dy == 0) continue;
                                      int ny = y + dy;
                                      int nx = rawX + dx;
                                      if (ny < 0 || ny >= alturaArte || nx < minX || nx > maxX) { isEdge = true; }
                                      else {
                                          std::string adjLinha = Aparencia::removerCoresANSI(arteUsada[ny]);
                                          if (nx >= static_cast<int>(adjLinha.length()) || adjLinha[nx] == ' ') isEdge = true;
                                      }
                                }
                            }
                            
                            std::string bgStr = getBg(tela[telaY * larguraTela + telaX]);
                            auto [bgR, bgG, bgB] = parseAnsiRGB(bgStr);
                            int tgtR = 0, tgtG = 0, tgtB = 0;
                            
                            if (isEdge) {
                                tgtR = 0; tgtG = 0; tgtB = 0;
                            } else {
                                auto [tr, tg, tb] = obterTgtRGB(c, rawX, y);
                                tgtR = tr; tgtG = tg; tgtB = tb;
                            }
                            
                            int blendedR = static_cast<int>(bgR + (tgtR - bgR) * spriteOpacity);
                            int blendedG = static_cast<int>(bgG + (tgtG - bgG) * spriteOpacity);
                            int blendedB = static_cast<int>(bgB + (tgtB - bgB) * spriteOpacity);
                            
                            tela[telaY * larguraTela + telaX] = "\033[48;2;" + std::to_string(blendedR) + ";" + std::to_string(blendedG) + ";" + std::to_string(blendedB) + "m \033[0m";
                        }
                    }
                }
            }
        }
    }

    // Helper local para dividir string UTF-8
    auto splitUTF8 = [](const std::string& s) {
        std::vector<std::string> chars;
        for (size_t i = 0; i < s.length(); ) {
            int len = 1;
            unsigned char c = static_cast<unsigned char>(s[i]);
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            chars.push_back(s.substr(i, len));
            i += len;
        }
        return chars;
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8)
    auto paintStr = [&](int posX, int posY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        if (posY < 0 || posY >= alturaVisivel) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        int drawX = posX - len/2;
        for (int i = 0; i < len; ++i) {
            int tx = drawX + i;
            if (tx >= 0 && tx < larguraTela) {
                std::string bg = forcedBg.empty() ? getBg(tela[posY * larguraTela + tx]) : forcedBg;
                tela[posY * larguraTela + tx] = bg + color + chars[i] + "\033[0m";
            }
        }
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8, alinhado à esquerda)
    auto paintStrLeft = [&](int posX, int posY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        if (posY < 0 || posY >= alturaVisivel) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        for (int i = 0; i < len; ++i) {
            int tx = posX + i;
            if (tx >= 0 && tx < larguraTela) {
                std::string bg = forcedBg.empty() ? getBg(tela[posY * larguraTela + tx]) : forcedBg;
                tela[posY * larguraTela + tx] = bg + color + chars[i] + "\033[0m";
            }
        }
    };

    // Só desenha nameplate/HP bar se não estiver morrendo e se spriteOpacity >= 1.0f
    if (desenharCorpo && !isMorte && spriteOpacity >= 1.0f) {
        int nameY = startY - 2;
        if (nameY >= 0) {
            std::string nameplate = inimigo->obterNome();
            if (totalInimigos > 1) {
                nameplate += " (" + std::to_string(inimigoIdx + 1) + ")";
            }
            std::string nameColor = "\033[1;38;2;255;100;100m"; // Default vermelho claro
            
            if (isSelecionado) {
                nameplate = "> " + nameplate + " <";
                if (TelaCombate::piscarSelecao) {
                    nameColor = "\033[1;38;2;255;255;0m"; // Amarelo
                } else {
                    nameColor = "\033[1;38;2;120;120;120m"; // Cinza
                }
            } else if (inimigo == g_inimigoAtacanteParry) {
                nameColor = "\033[1;38;2;255;140;0m"; // Laranja ao atacar
            }
            
            paintStr(startX + croppedWidth/2, nameY, nameplate, nameColor);
            
            int hpY = startY - 1;
            if (hpY >= 0) {
                double pct = static_cast<double>(inimigo->obterVida()) / std::max(1, inimigo->obterVidaMaxima());
                std::string hpValStr = std::to_string(inimigo->obterVida()) + "/" + std::to_string(inimigo->obterVidaMaxima());
                
                int totalLen = 5 + 8 + 2 + hpValStr.length(); // "HP: [" (5) + 8 blocks + "] " (2) + hpValStr
                int drawX = (startX + croppedWidth / 2) - totalLen / 2;
                
                // 1. "HP: ["
                paintStrLeft(drawX, hpY, "HP: [", "\033[38;2;180;180;180m");
                drawX += 5;
                
                // 2. Blocks with gradient
                int blocks = 8;
                int qtdReal = static_cast<int>(pct * blocks * 8);
                
                Cor baseCorVida = (pct > 0.70) ? Cor::VERDE : (pct > 0.30) ? Cor::AMARELO : Cor::VERMELHO;
                
                for (int i = 0; i < blocks; ++i) {
                    int intensidade = 130 + (125 * i) / std::max(1, blocks - 1);
                    std::string corAtual = Aparencia::obterCorRGBFade(baseCorVida, intensidade);
                    int charIdx = i * 8;
                    if (qtdReal >= charIdx + 4) {
                        paintStrLeft(drawX + i, hpY, "█", corAtual);
                    } else {
                        paintStrLeft(drawX + i, hpY, "░", "\033[38;2;140;140;140m"); // Cor::CINZA is 140, 140, 140
                    }
                }
                drawX += blocks;
                
                // 3. "] "
                paintStrLeft(drawX, hpY, "] ", "\033[38;2;180;180;180m");
                drawX += 2;
                
                // 4. hpValStr (Orange if attacker, otherwise color based on life percentage)
                std::string hpColor = "\033[38;2;255;100;100m";
                if (inimigo == g_inimigoAtacanteParry) {
                    hpColor = "\033[38;2;255;140;0m"; // Laranja ao atacar
                } else {
                    hpColor = Aparencia::obterCorRGBFade(baseCorVida, 200);
                }
                paintStrLeft(drawX, hpY, hpValStr, hpColor);
            }
        }

        // Floating Combat Text (FCT)
        if (danoAmount > 0 && flashDanoInimigo > 0) {
            int fctY = startY - 3;
            if (fctY < 0) fctY = 0;
            
            std::string textFCT = isCura ? ("+" + std::to_string(danoAmount)) : ("-" + std::to_string(danoAmount));
            std::string colorFCT = isCura ? "\033[1;38;2;50;255;50m" : "\033[1;38;2;255;50;50m";
            
            int tremble = (!isCura && flashDanoInimigo % 2 == 0) ? 1 : -1;
            
            paintStr(startX + croppedWidth/2 + tremble, fctY, textFCT, colorFCT);
        }
    }


}
