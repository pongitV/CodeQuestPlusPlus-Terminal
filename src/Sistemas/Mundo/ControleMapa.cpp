#include "ControleMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../../Interface/Telas/Atributos/TelaAtributos.h"
#include "../../Interface/Telas/Diario/TelaDiario.h"
#include "../../Interface/Telas/Menu/TelaMenu.h"
#include "../../Interface/Telas/Pause/TelaPause.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Interface/Telas/MapaMundial/TelaMapaMundial.h"
#include "../Combate/Combate.h"
#include "../Progresso/Progressao.h"
#include "../../Core/Controladores/Debug.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Core/Utilidades/GeradorAleatorio.h"
#include "../Raycaster/Raycaster.h"
#include "../Raycaster/RaycasterMundo.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

namespace {
    std::string extrairCorBaseDoRaycaster(char celula, const std::string& tituloDoMapa, bool isFloresta) {
        // Sampleia a textura no "meio" do bloco (tx=33, ty=33) para evitar as linhas escuras de rejunte/sombra
        std::vector<std::tuple<int, int, int>> luzesVazias;
        std::string pixelANSI = RaycasterMundo::obterPixelParedeInternal(tituloDoMapa, isFloresta, 0.0f, 10.0f, celula, 33, 0, 64, 33.0f/64.0f, 0.0f, luzesVazias, 0.0f, 0.0f);
        if (pixelANSI.find("\033[48;2;") == 0) {
            size_t pos_m = pixelANSI.find('m');
            if (pos_m != std::string::npos) {
                return "\033[38;2;" + pixelANSI.substr(7, pos_m - 7) + "m";
            }
        }
        return Aparencia::cor(Cor::CINZA);
    }

    void calcularCameraAxis(int maxVisivel, int posicaoJogador, int tamanhoMapa, int& start, int& end) {
        start = 0;
        end = tamanhoMapa;

        if (end > maxVisivel) {
            start = std::max(0, posicaoJogador - (maxVisivel / 2));
            end = start + maxVisivel;
            if (end > tamanhoMapa) {
                end = tamanhoMapa;
                start = std::max(0, end - maxVisivel);
            }
        }
    }
}

static bool s_exploracao3DAtiva = true;
static bool s_recemTrocouDeMapa = false;
static float s_posCamera3DX = -1.0f;
static float s_posCamera3DY = -1.0f;
static float s_anguloCamera3D = 0.0f;
static std::string s_tituloMapaAtual = "";

void ControleMapa::sinalizarTrocaDeMapa3D() { s_recemTrocouDeMapa = true; }

bool ControleMapa::isExploracao3DAtiva() { return s_exploracao3DAtiva; }

bool ControleMapa::processarInputEComandos(char tecla, Personagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    // --- TELA DE PAUSE ---
    if (tecla == 27 || tecla == '\033') // Códigos ASCII para a tecla ESC
    {
        TelaPause::exibir(jogador);
        restaurarTela();
        return true; // Retorna true para o mapa não processar movimento neste frame
    }

    // --- MENU DE DEBUG (ISOLADO PARA FACIL REMOCAO FUTURA) ---
    if (tecla == '\\' || tecla == '`' || tecla == '=')
    {
        Debug::exibirMenuDebug(jogador);
        restaurarTela();
        return true;
    }
    // --- FIM DO MENU DE DEBUG ---

    ComandoMapa comando = ControleDeInput::traduzirTeclaParaComando(tecla);

    // Movimentação (Não abre menus, portanto retorna falso)
    if (comando == ComandoMapa::Cima) { proximaPosicaoY--; return false; }
    if (comando == ComandoMapa::Baixo) { proximaPosicaoY++; return false; }
    if (comando == ComandoMapa::Esquerda) { proximaPosicaoX--; return false; }
    if (comando == ComandoMapa::Direita) { proximaPosicaoX++; return false; }

    // Menus (Retornam true informando o mapa que a tela precisa pular a interação de movimento)
    if (comando == ComandoMapa::Inventario)
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Ficha)
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Bestiario)
    {
        TelaDiario::exibir(jogador);
        restaurarTela();
        return true;
    }
    return false;
}

void ControleMapa::aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa) {
    if (posicaoY < 0) posicaoY = 0; else if (posicaoY >= static_cast<int>(matrizDoMapa.size())) posicaoY = static_cast<int>(matrizDoMapa.size()) - 1;
    if (posicaoX < 0) posicaoX = 0; else if (posicaoX >= static_cast<int>(matrizDoMapa[0].size())) posicaoX = static_cast<int>(matrizDoMapa[0].size()) - 1;
}

void ControleMapa::processarCombate(
    Personagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<Personagem>> inimigosParaBatalha, 
    int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    Aparencia::iniciarInteracaoPopup();
    std::vector<std::string> texto = { 
        Aparencia::cor(Cor::AMARELO) + "[!] " + mensagemDeAviso + Aparencia::cor(Cor::RESET) 
    };
    std::vector<std::string> opcoesCombate = { "Nao, recuar", "Sim, batalha!" };
    int opcaoEscolhidaPeloJogador = ControleDeInput::lerSelecaoMenuEmPopup(tituloDoCombate, texto, opcoesCombate, Cor::VERMELHO);

    if (opcaoEscolhidaPeloJogador == 1) {
        Combate combate(jogadorAtual, std::move(inimigosParaBatalha));
        if (s_exploracao3DAtiva) {
            combate.setContexto3D(true, matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, s_tituloMapaAtual);
        }
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < quantidadeDeCelulasOcupadas; ++i) matrizDoMapaAtual[posicaoYAposCombate][posicaoXInicialDoInimigo + i] = '.';
            posicaoXDoJogador = posicaoXAposCombate;
            posicaoYDoJogador = posicaoYAposCombate;
        }
    }

    if (exploracaoEstaAtiva && !s_exploracao3DAtiva) restaurarTela();
}

void ControleMapa::entrarSubMapa(
    std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
    int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
    std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
    const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela)
{
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
    posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
    posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

    if (!subMapaJaFoiVisitado) { matrizDoMapaAtual = matrizDoSubMapaGerada; subMapaJaFoiVisitado = true; } 
    else { matrizDoMapaAtual = matrizDoSubMapaSalva; }
    padronizarTamanhoDoMapa(matrizDoMapaAtual);

    posicaoXDoJogador = posicaoXInicialNoSubMapa;
    posicaoYDoJogador = posicaoYInicialNoSubMapa;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = tituloDoSubMapa;
    if (!s_exploracao3DAtiva) restaurarTela();
}

void ControleMapa::moverInimigosAleatoriamente(std::vector<std::string>& matrizDoMapaAtual, const std::vector<std::string>& matrizOriginal, const std::string& simbolosInimigos, int jogadorX, int jogadorY) {
    if (simbolosInimigos.empty()) return;

    struct Pos { int x, y; char c; };
    std::vector<Pos> inimigosAtuais;
    
    for (int y = 0; y < static_cast<int>(matrizDoMapaAtual.size()); ++y) {
        for (int x = 0; x < static_cast<int>(matrizDoMapaAtual[y].size()); ++x) {
            if (simbolosInimigos.find(matrizDoMapaAtual[y][x]) != std::string::npos) {
                // Ignora o caractere se ele fizer parte de um marcador de mapa/teleporte (ex: ^S, ^Vila)
                if (x > 0 && matrizDoMapaAtual[y][x-1] == '^') continue;

                inimigosAtuais.push_back({x, y, matrizDoMapaAtual[y][x]});
            }
        }
    }

    for (const auto& inimigo : inimigosAtuais) {
        if (matrizDoMapaAtual[inimigo.y][inimigo.x] != inimigo.c) continue; // Pode ter sido alterado (morto/já movido)
        
        int originX = -1, originY = -1;
        // Encontra o spawn original no raio 3x3
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int oy = inimigo.y + dy;
                int ox = inimigo.x + dx;
                if (oy >= 0 && oy < static_cast<int>(matrizOriginal.size()) && ox >= 0 && ox < static_cast<int>(matrizOriginal[oy].size())) {
                    if (matrizOriginal[oy][ox] == inimigo.c) {
                        originX = ox;
                        originY = oy;
                        break;
                    }
                }
            }
            if (originX != -1) break;
        }

        if (originX == -1) continue;

        std::vector<std::pair<int, int>> movimentosPossiveis;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int ty = originY + dy;
                int tx = originX + dx;
                if (ty >= 0 && ty < static_cast<int>(matrizDoMapaAtual.size()) && tx >= 0 && tx < static_cast<int>(matrizDoMapaAtual[ty].size())) {
                    // Só pode se mover para células vazias, e não pode pisar em cima do jogador
                    if (matrizDoMapaAtual[ty][tx] == '.' && (tx != jogadorX || ty != jogadorY)) {
                        // O inimigo anda 1 de cada vez, então limitamos aos adjacentes atuais dele dentro do raio 3x3 da origem
                        if (std::abs(tx - inimigo.x) <= 1 && std::abs(ty - inimigo.y) <= 1) {
                            movimentosPossiveis.push_back({tx, ty});
                        }
                    }
                }
            }
        }

        movimentosPossiveis.push_back({inimigo.x, inimigo.y}); // Opção de permanecer parado

        int escolha = GeradorAleatorio::obterInteiro(0, static_cast<int>(movimentosPossiveis.size()) - 1);
        int nx = movimentosPossiveis[escolha].first;
        int ny = movimentosPossiveis[escolha].second;

        if (nx != inimigo.x || ny != inimigo.y) {
            matrizDoMapaAtual[inimigo.y][inimigo.x] = '.';
            matrizDoMapaAtual[ny][nx] = inimigo.c;
        }
    }
}

int ControleMapa::animarIntroducaoMapa(
    const std::string& tituloDoMapa,
    const std::vector<std::string>& arteDoMapa,
    int /*larguraArte*/,
    const std::vector<std::string>& arteTransicao,
    int /*larguraTransicao*/,
    Cor corTema,
    const std::vector<std::string>& matrizDoMapa,
    int posicaoXDoJogador,
    int posicaoYDoJogador,
    const std::function<std::string(char, int, int)>& formatadorCelula,
    bool animar,
    bool usarAnimacaoBanner,
    const std::function<void()>& acaoAposFadeInArte
) {
    if (ControleMapa::isExploracao3DAtiva()) {
        return 0; 
    }

    Aparencia::limparTela();
    Aparencia::ocultarCursor();

    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();

    if (!animar) {
        Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO);
        int linhaInicialMapa = Aparencia::obterPosicaoCursorY();
        renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        return linhaInicialMapa;
    }

    if (acaoAposFadeInArte) {
        acaoAposFadeInArte();
        Aparencia::limparTela();
    }
    
    Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO);
    int linhaInicialMapa = Aparencia::obterPosicaoCursorY();

    std::vector<std::string> bannerBase;
    if (usarAnimacaoBanner) {
        if (!arteDoMapa.empty()) {
            bannerBase = arteDoMapa;
        } else if (!arteTransicao.empty()) {
            bannerBase = arteTransicao;
        }
    }

    if (bannerBase.empty() || !usarAnimacaoBanner) {
        Aparencia::animarFadeIn(20, 25, [&](int /*frame*/, int intensidade) {
            std::ostringstream buffer;
            std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
            
            Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO);
            renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
            
            std::string escurecedor = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + "m";
            
            std::cout.rdbuf(oldCout);
            std::cout << "\033[H" << escurecedor << buffer.str() << "\033[0m" << std::flush;
        });
        
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto(tituloDoMapa, Cor::BRANCO);
        renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        
        return linhaInicialMapa;
    }

    std::vector<std::string> banner;
    for (const auto& l : bannerBase) {
        // Remover cores antigas e forcar branco negrito
        banner.push_back("\033[1;37m" + Aparencia::removerCoresANSI(l) + "\033[0m");
    }
    
    int maxW = 0;
    for (const auto& l : banner) {
        int w = Aparencia::obterComprimentoVisual(l);
        if (w > maxW) maxW = w;
    }
    
    int bannerHeight = banner.size();
    int startXBox = (larguraTerminal - maxW) / 2;
    if (startXBox < 0) startXBox = 0;

    // Fazer cache do mapa para double-buffering sem piscar
    int startX, endX;
    calcularCameraHorizontal(larguraTerminal, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);
    std::string margemEsquerdaDoMapa = calcularMargemCentralizada(larguraTerminal, endX - startX);
    
    std::string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string margemEsquerdaControles = calcularMargemCentralizada(larguraTerminal, textoDeControles.length());
    
    // O Controle de mapa usa 2 linhas de offset (texto na linha 0, \n vai pra 1, \n vai pra 2)
    int offsetMapaReal = 2;
    
    int startY, endY;
    calcularCameraVertical(alturaTerminal, linhaInicialMapa, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);
    
    std::vector<std::string> linhasDoMapaCache;
    for (int y = startY; y < endY; y++) {
        std::string linhaStr = margemEsquerdaDoMapa;
        linhaStr.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            linhaStr += formatadorCelula(matrizDoMapa[y][x], x, y);
        }
        linhasDoMapaCache.push_back(linhaStr);
    }

    // Desenha os controles e o mapa inteiro APENAS UMA VEZ antes da animacao
    std::ostringstream initialMap;
    initialMap << "\033[" << (linhaInicialMapa + 1) << ";1H\033[K" << margemEsquerdaControles << Aparencia::cor(Cor::CINZA) << textoDeControles << Aparencia::cor(Cor::RESET) << "\n\033[K\n";
    for (int i = 0; i < (int)linhasDoMapaCache.size(); i++) {
        initialMap << "\033[" << (linhaInicialMapa + 1 + offsetMapaReal + i) << ";1H" << linhasDoMapaCache[i] << "\033[K";
    }
    std::cout << initialMap.str() << std::flush;

    // 1. Dropdown animation (Desce sobrepondo o mapa)
    int destinoY = (alturaTerminal - linhaInicialMapa) / 6; // Desce um pouco, igual ao 3D
    int maxPassos = 8;
    for (int passo = 0; passo <= maxPassos; passo++) {
        std::ostringstream telaFrame;
        int currentY = -bannerHeight + (passo * (bannerHeight + destinoY)) / maxPassos;
        
        // Restaura o mapa e os controles em cima da caixa de desenho do frame
        telaFrame << "\033[" << (linhaInicialMapa + 1) << ";1H\033[K" << margemEsquerdaControles << Aparencia::cor(Cor::CINZA) << textoDeControles << Aparencia::cor(Cor::RESET);
        telaFrame << "\033[" << (linhaInicialMapa + 2) << ";1H\033[K";
        telaFrame << "\033[" << (linhaInicialMapa + 3) << ";1H\033[K";
        
        // Redesenha as linhas do mapa desde o topo ate o limite inferior do banner
        // Isso apaga o "rastro" que o banner deixa ao descer
        int endDrawY = currentY + bannerHeight + 2;
        for (int drawY = offsetMapaReal; drawY <= endDrawY; drawY++) {
            if (drawY - offsetMapaReal >= 0 && drawY - offsetMapaReal < (int)linhasDoMapaCache.size()) {
                telaFrame << "\033[" << (linhaInicialMapa + 1 + drawY) << ";1H\033[K" << linhasDoMapaCache[drawY - offsetMapaReal];
            }
        }
        
        // Desenha o banner
        for (int i = 0; i < bannerHeight; i++) {
            int drawY = linhaInicialMapa + currentY + i;
            if (drawY >= linhaInicialMapa && drawY < alturaTerminal) {
                telaFrame << "\033[" << (drawY + 1) << ";" << (startXBox + 1) << "H" << banner[i];
            }
        }
        std::cout << telaFrame.str() << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    ControleDeInput::limparBuffer();
    
    // Assegura que o mapa esta totalmente renderizado ao final
    renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
    
    return linhaInicialMapa;
}


void ControleMapa::animarFlashbang(int r, int g, int b) {
    int LARGURA_TELA = Aparencia::obterLarguraTerminal();
    int ALTURA_TELA = Aparencia::obterAlturaTerminal();
    if (LARGURA_TELA <= 0) LARGURA_TELA = 120;
    if (ALTURA_TELA <= 0) ALTURA_TELA = 30;

    std::cout << "\033[?25l"; // Hide cursor
    std::string colorPrefix = "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
    
    // Lista de caracteres de dithering para o fade out
    std::vector<std::string> fadeChars = {"█", "▓", "▒", "░", " "};
    
    for (int passo = 0; passo < (int)fadeChars.size(); passo++) {
        std::string buffer = "\033[H";
        buffer.reserve(LARGURA_TELA * ALTURA_TELA * 20);
        
        for (int y = 0; y < ALTURA_TELA; y++) {
            for (int x = 0; x < LARGURA_TELA; x++) {
                if (y == ALTURA_TELA - 1 && x == LARGURA_TELA - 1) break; // prevent scroll
                if (fadeChars[passo] == " ") {
                    buffer += "\033[40m \033[0m"; // Preto final
                } else {
                    buffer += colorPrefix + "\033[38;2;255;255;255m" + fadeChars[passo] + "\033[0m";
                }
            }
            if (y < ALTURA_TELA - 1) buffer += "\n";
        }
        std::cout << buffer << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    Aparencia::limparTela();
}

void ControleMapa::calcularCameraVertical(int alturaDoTerminal, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = std::max(5, alturaDoTerminal - linhaInicial - 4);
    calcularCameraAxis(maxLinhasVisiveis, posicaoYDoJogador, tamanhoDoMapa, startY, endY);
}

void ControleMapa::calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX) {
    int maxColunasVisiveis = std::max(10, larguraDoTerminal); // Usa a largura total do terminal
    calcularCameraAxis(maxColunasVisiveis, posicaoXDoJogador, larguraDoMapa, startX, endX);
}

std::string ControleMapa::calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto) {
    int espacos = (larguraDoTerminal - larguraDoTexto) / 2;
    return std::string(espacos > 0 ? espacos : 0, ' ');
}

void ControleMapa::padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa) {
    size_t maxLength = 0;
    // Primeiro removemos os espacos em branco a direita para encontrar a largura real do mapa
    for (auto& linha : matrizDoMapa) {
        size_t lastChar = linha.find_last_not_of(" \t\r\n");
        if (lastChar != std::string::npos) {
            linha.erase(lastChar + 1);
        } else {
            linha.clear(); // A linha eh apenas espacos
        }
        if (linha.length() > maxLength) maxLength = linha.length();
    }
    // Depois preenchemos todas as linhas ate o maxLength com espacos para formar a matriz
    for (auto& linha : matrizDoMapa) {
        if (linha.length() < maxLength) linha.append(maxLength - linha.length(), ' ');
    }
}

std::string ControleMapa::formatarCelula(char celula, int x, int y, const std::string& tituloDoMapa, const std::vector<std::string>& matrizDoMapa, bool isMinimapa) {
    thread_local std::string ultimoTitulo = "";
    thread_local std::string tituloUpper = "";
    thread_local bool isReino = false, isInterior = false, isFloresta = false, isVila = false, isSpawn = false;

    if (ultimoTitulo != tituloDoMapa) {
        ultimoTitulo = tituloDoMapa;
        tituloUpper = tituloDoMapa;
        for (char& ch : tituloUpper) ch = std::toupper(static_cast<unsigned char>(ch));
        
        isReino = (tituloUpper.find("CASTELO") != std::string::npos || tituloUpper.find("REINO") != std::string::npos);
        isInterior = (tituloUpper.find("LABIRINTO") != std::string::npos || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos || tituloUpper.find("CAVERNA") != std::string::npos);
        isFloresta = (tituloUpper.find("FLORESTA") != std::string::npos);
        isVila = (tituloUpper.find("VILA") != std::string::npos);
        isSpawn = (tituloUpper.find("INICIO") != std::string::npos);
    }
    
    // Teleporte
    if (celula == '^') return Aparencia::cor(Cor::NEGRITO, Cor::TELEPORTE) + "^" + Aparencia::cor(Cor::RESET);
    
    // Água
    if (celula == '~') return Aparencia::corRGB(50, 150, 255) + "≈" + Aparencia::cor(Cor::RESET);
    
    // Árvores
    if (celula == '*') {
        bool isTrunk = false;
        if (y > 0 && matrizDoMapa[y-1][x] == '*') {
            int countHorizontal = 0;
            if (x > 0 && matrizDoMapa[y][x-1] == '*') countHorizontal++;
            if (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && matrizDoMapa[y][x+1] == '*') countHorizontal++;
            if (countHorizontal <= 1) isTrunk = true;
        }
        if (isTrunk) return Aparencia::cor(Cor::MADEIRA) + "█" + Aparencia::cor(Cor::RESET);
        return Aparencia::cor(Cor::VERDE) + "▲" + Aparencia::cor(Cor::RESET);
    }
    
    // Verifica se é uma letra de placa de chão (Label) ANTES de checar as entidades
    if (RaycasterMundo::isMapLabel(x, y, matrizDoMapa)) {
        return Aparencia::cor(Cor::CINZA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    // Entidades
    if (isVila || isSpawn) {
        if (celula == 'G' || celula == 'O') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET); // Inimigos Vermelhos
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::CIANO) + "B" + Aparencia::cor(Cor::RESET); // Bjorn Ciano
        if (celula == 'F' && x > 0 && matrizDoMapa[y][x-1] == '{') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "F" + Aparencia::cor(Cor::RESET); // Franchesco Amarelo
        if (celula == 'P') return Aparencia::cor(Cor::NEGRITO, Cor::MARROM) + "P" + Aparencia::cor(Cor::RESET); // Placa Marrom
    } else if (isFloresta) {
        if (celula == 'S' && (!isInterior || tituloUpper.find("CHEFE") != std::string::npos)) return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "S" + Aparencia::cor(Cor::RESET);
        if (celula == 'F' || celula == 'A') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        if (celula == 'M') return Aparencia::cor(Cor::NEGRITO, Cor::MAGENTA) + "M" + Aparencia::cor(Cor::RESET);
        if (celula == 'B') return Aparencia::cor(Cor::NEGRITO, Cor::DOURADO) + "B" + Aparencia::cor(Cor::RESET);
    } else if (isReino) {
        if (celula == 'T') return Aparencia::cor(Cor::NEGRITO, Cor::VERMELHO) + "T" + Aparencia::cor(Cor::RESET);
        if (celula == 'G') return Aparencia::cor(Cor::NEGRITO, Cor::AMARELO) + "G" + Aparencia::cor(Cor::RESET);
        if (celula == 'C') return Aparencia::cor(Cor::NEGRITO, Cor::PRATA) + "C" + Aparencia::cor(Cor::RESET);
    }
    
    if (tituloUpper == "SALA DO CHEFE" && (celula == 'M' || celula == 'A' || celula == 'H' || celula == 'O' || celula == 'R' || celula == 'G')) {
        return Aparencia::cor(Cor::NEGRITO, Cor::BRANCO) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    // Casas e Estruturas
    if (!isInterior && !isReino) {
        std::string corEstrutura = extrairCorBaseDoRaycaster('|', tituloUpper, isFloresta);
        
        if (celula == '_') return corEstrutura + "▄" + Aparencia::cor(Cor::RESET);
        if (celula == '|' || celula == '[' || celula == ']') return corEstrutura + "█" + Aparencia::cor(Cor::RESET);
        std::string estruturas = "{}/\\<>;=-:+";
        if (estruturas.find(celula) != std::string::npos) return corEstrutura + std::string(1, celula) + Aparencia::cor(Cor::RESET);
        
        if (celula == '#') {
            if (isFloresta) return Aparencia::cor(Cor::VERDE) + "█" + Aparencia::cor(Cor::RESET); // Arvores continuam verdes
            std::string corMuro = extrairCorBaseDoRaycaster('#', tituloUpper, isFloresta);
            return corMuro + "█" + Aparencia::cor(Cor::RESET);
        }
    }
    
    // Castelo
    if (isReino) {
        if (celula == '|') return Aparencia::cor(Cor::MADEIRA) + "█" + Aparencia::cor(Cor::RESET); // Portao de madeira
        std::string estruturas = "_[]{}/\\<>;=-+#";
        if (estruturas.find(celula) != std::string::npos) {
            std::string corCastelo = extrairCorBaseDoRaycaster(celula, tituloUpper, isFloresta);
            return corCastelo + "█" + Aparencia::cor(Cor::RESET);
        }
    }
    
    // Labirinto
    if (isInterior) {
        if (tituloUpper.find("LABIRINTO") != std::string::npos) {
            std::string corLabirinto = extrairCorBaseDoRaycaster('|', tituloUpper, isFloresta);
            auto isHWall = [](char c) { return c == '=' || c == '.' || c == '\''; };
            auto isVWall = [](char c) { return c == '|' || c == '+' || c == 'S' || c == 'E'; };

            if (celula == '=') return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
            if (celula == '|') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                if (right && left) return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
                if (right) return corLabirinto + "├" + Aparencia::cor(Cor::RESET);
                if (left) return corLabirinto + "┤" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "│" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '.') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                
                if (left && right && down) return corLabirinto + "┬" + Aparencia::cor(Cor::RESET);
                if (right && down) return corLabirinto + "┌" + Aparencia::cor(Cor::RESET);
                if (left && down) return corLabirinto + "┐" + Aparencia::cor(Cor::RESET);
                if (left && right) return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '\'') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && up) return corLabirinto + "┴" + Aparencia::cor(Cor::RESET);
                if (right && up) return corLabirinto + "└" + Aparencia::cor(Cor::RESET);
                if (left && up) return corLabirinto + "┘" + Aparencia::cor(Cor::RESET);
                if (left && right) return corLabirinto + "─" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "█" + Aparencia::cor(Cor::RESET);
            }
            if (celula == '+') {
                bool right = (x + 1 < static_cast<int>(matrizDoMapa[y].length()) && isHWall(matrizDoMapa[y][x+1]));
                bool left = (x > 0 && isHWall(matrizDoMapa[y][x-1]));
                bool down = (y + 1 < static_cast<int>(matrizDoMapa.size()) && isVWall(matrizDoMapa[y+1][x]));
                bool up = (y > 0 && isVWall(matrizDoMapa[y-1][x]));
                
                if (left && right && down && up) return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
                if (left && right && down) return corLabirinto + "┬" + Aparencia::cor(Cor::RESET);
                if (left && right && up) return corLabirinto + "┴" + Aparencia::cor(Cor::RESET);
                if (up && down && left) return corLabirinto + "┤" + Aparencia::cor(Cor::RESET);
                if (up && down && right) return corLabirinto + "├" + Aparencia::cor(Cor::RESET);
                return corLabirinto + "┼" + Aparencia::cor(Cor::RESET);
            }
        }
        else if (tituloUpper.find("CAVERNA") != std::string::npos) {
            std::string corCaverna = extrairCorBaseDoRaycaster('#', tituloUpper, isFloresta);
            if (celula == '#') return corCaverna + "█" + Aparencia::cor(Cor::RESET);
            if (celula == '.') {
                if (isMinimapa) return "\033[38;2;50;50;50m.\033[0m";
                return "\033[38;2;40;40;40m·\033[0m";
            }
        }
    }
    
    // Chão / Labels
    if (celula == '.' && (!isInterior || tituloUpper.find("CHEFE") != std::string::npos || tituloUpper.find("CORACAO") != std::string::npos)) {
        if (isMinimapa) return "\033[38;2;50;50;50m.\033[0m";
        return "\033[38;2;40;40;40m·\033[0m";
    }
    
    if (std::isalpha(celula) && celula != ' ' && celula != 'S' && celula != 'F' && celula != 'A' && celula != 'M' && celula != 'B' && celula != 'T' && celula != 'G' && celula != 'C') {
        return Aparencia::cor(Cor::CINZA) + std::string(1, celula) + Aparencia::cor(Cor::RESET);
    }
    
    return std::string(1, celula);
}

void ControleMapa::renderizarMapa(const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, int larguraDoTerminal, int alturaDoTerminal, int linhaInicial, const std::function<std::string(char, int, int)>& formatadorCelula) {
    int startX, endX;
    calcularCameraHorizontal(larguraDoTerminal, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);

    std::string margemEsquerdaDoMapa = calcularMargemCentralizada(larguraDoTerminal, endX - startX);

    std::string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventario | C: Ficha | B: Diario | M: Mapa";
    std::string margemEsquerdaControles = calcularMargemCentralizada(larguraDoTerminal, textoDeControles.length());

    Aparencia::moverCursor(0, linhaInicial);

    int startY, endY;
    calcularCameraVertical(alturaDoTerminal, linhaInicial, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);

    std::cout << margemEsquerdaControles << Aparencia::cor(Cor::CINZA) << textoDeControles << Aparencia::cor(Cor::RESET) << "\n\n";

    for (int y = startY; y < endY; y++) {
        std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
        linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            linhaSendoRenderizada += formatadorCelula(matrizDoMapa[y][x], x, y);
        }
        std::cout << linhaSendoRenderizada << "\033[K\n";
    }
    std::cout << "\033[J" << std::flush;
}

ProximaTransicaoMapa ControleMapa::executarLoopDeExploracao(
    Personagem* jogadorAtual,
    std::vector<std::string>& matrizDoMapaAtual,
    int& posicaoXDoJogador,
    int& posicaoYDoJogador,
    bool& exploracaoEstaAtiva,
    const std::string& tituloDoMapaAtual,
    const std::function<std::string()>& obterSimbolosInimigos,
    const std::function<std::vector<std::string>()>& obterLayoutOriginal,
    const std::function<void(int, int, int)>& processarInteracao,
    const std::function<std::string(char, int, int)>& formatador,
    const std::function<void()>& restaurarTela,
    int& linhaInicialParaDesenharOMapa,
    bool& precisaRenderizar
) {
    auto ultimoMovimentoInimigos = std::chrono::steady_clock::now();
    ProximaTransicaoMapa destinoViagemRapida = ProximaTransicaoMapa::Nenhuma;
    s_anguloCamera3D = 0.0f;
    s_posCamera3DX = -1.0f;
    s_posCamera3DY = -1.0f;
    s_tituloMapaAtual = tituloDoMapaAtual;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
        s_tituloMapaAtual = tituloDoMapaAtual;
        
        auto agora = std::chrono::steady_clock::now();
        bool tempoDeMoverInimigos = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoMovimentoInimigos).count() >= 800;

        if (tempoDeMoverInimigos) {
            ControleMapa::moverInimigosAleatoriamente(matrizDoMapaAtual, obterLayoutOriginal(), obterSimbolosInimigos(), posicaoXDoJogador, posicaoYDoJogador);
            ultimoMovimentoInimigos = std::chrono::steady_clock::now();
            precisaRenderizar = true;
        }

        int larguraDoTerminal = Aparencia::obterLarguraTerminal();
        
        if (precisaRenderizar && !s_exploracao3DAtiva) {
            int alturaDoTerminal = Aparencia::obterAlturaTerminal();

            ControleMapa::renderizarMapa(matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, larguraDoTerminal, alturaDoTerminal, linhaInicialParaDesenharOMapa, formatador);

            precisaRenderizar = false;
        }

        char teclaPressionadaPeloJogador = '\0';
        bool processarInput = false;

            if (!s_exploracao3DAtiva && ControleDeInput::teclaPressionada()) {
            teclaPressionadaPeloJogador = ControleDeInput::lerTecla();
            processarInput = true;
        }

        if (s_exploracao3DAtiva || (processarInput && (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V'))) {
            static std::string tituloAnterior = "";
            int tipoAnimacao = 0;
            
            bool trocandoDePerspectiva = !s_exploracao3DAtiva;
            if (trocandoDePerspectiva) {
                tipoAnimacao = 1; // Olho abrindo (entrou via 'V')
            }
            if (s_recemTrocouDeMapa && !trocandoDePerspectiva) {
                tipoAnimacao = 2; // Porta + Banner
            }
            
            if (tituloAnterior != tituloDoMapaAtual) {
                if (tituloAnterior != "" && !trocandoDePerspectiva) {
                    tipoAnimacao = 2;
                }
                tituloAnterior = tituloDoMapaAtual;
            }

            s_recemTrocouDeMapa = false;
            s_exploracao3DAtiva = true;

                if (s_posCamera3DX == -1.0f || static_cast<int>(s_posCamera3DX) != posicaoXDoJogador || static_cast<int>(s_posCamera3DY) != posicaoYDoJogador) {
                    s_posCamera3DX = static_cast<float>(posicaoXDoJogador) + 0.5f;
                    s_posCamera3DY = static_cast<float>(posicaoYDoJogador) + 0.5f;
                }
                
                int hitX = -1, hitY = -1;
                char acaoPendente = Raycaster::iniciarExploracao3D(matrizDoMapaAtual, s_posCamera3DX, s_posCamera3DY, s_anguloCamera3D, tituloDoMapaAtual, jogadorAtual, hitX, hitY, tipoAnimacao);
                
                posicaoXDoJogador = static_cast<int>(s_posCamera3DX);
                posicaoYDoJogador = static_cast<int>(s_posCamera3DY);
                
                bool isTrigger = false;
                if (hitX != -1 && hitY != -1) {
                    ControleMapa::aplicarLimitesDeMapa(hitX, hitY, matrizDoMapaAtual);
                    
                    char cell = matrizDoMapaAtual[hitY][hitX];
                    // Verifica se o jogador parou em um trigger (Inimigos ou Teleportes)
                    std::string triggers = "^GOBFSAMTHRPC";
                    if (triggers.find(cell) != std::string::npos) {
                        isTrigger = true;
                    }
                    
                    processarInteracao(hitX, hitY, larguraDoTerminal); // Aciona o combate/NPC caso o jogador tenha parado em cima de um
                }
                
                if (acaoPendente == 'M') {
                    teclaPressionadaPeloJogador = 'M';
                    processarInput = true;
                } else if (!isTrigger) {
                    s_exploracao3DAtiva = false;
                    restaurarTela();
                    precisaRenderizar = true;
                    continue;
                } else {
                    continue;
                }
        }

        if (processarInput) {
            if (teclaPressionadaPeloJogador == 'v' || teclaPressionadaPeloJogador == 'V') {
                continue; // Ja foi tratado acima
            }

            if (teclaPressionadaPeloJogador == 'm' || teclaPressionadaPeloJogador == 'M') {
                LocalizacaoMapa loc = LocalizacaoMapa::VilaInicial;
                std::string tituloUpper = tituloDoMapaAtual;
                std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), ::toupper);
                
                if (tituloUpper.find("FLORESTA") != std::string::npos || 
                    tituloUpper.find("BOSQUE") != std::string::npos ||
                    tituloUpper.find("LABIRINTO") != std::string::npos ||
                    tituloUpper.find("CHEFE") != std::string::npos ||
                    tituloUpper.find("ARVORE") != std::string::npos) {
                    loc = LocalizacaoMapa::Floresta;
                } else if (tituloUpper.find("REINO") != std::string::npos || tituloUpper.find("CASTELO") != std::string::npos) {
                    loc = LocalizacaoMapa::Reino;
                }
                
                int progressoVila = Progressao::instancia().obterProgressoVila(jogadorAtual);
                int progressoFloresta = Progressao::instancia().obterProgressoFloresta(jogadorAtual);
                int progressoReino = Progressao::instancia().obterProgressoReino(jogadorAtual);

                ProximaTransicaoMapa destino = TelaMapaMundial::exibir(loc, progressoVila, progressoFloresta, progressoReino);

                if (destino != ProximaTransicaoMapa::Nenhuma) {
                    destinoViagemRapida = destino;
                    exploracaoEstaAtiva = false; // Sinaliza para sair do loop e processar a viagem
                    break;
                }
                // Se nenhum destino foi escolhido, apenas restaura a tela e continua a exploração.
                if (!s_exploracao3DAtiva) {
                    restaurarTela();
                    precisaRenderizar = true;
                }
                continue;
            }

            int proximaPosicaoX = posicaoXDoJogador;
            int proximaPosicaoY = posicaoYDoJogador;

            bool abriuMenu = ControleMapa::processarInputEComandos(teclaPressionadaPeloJogador, jogadorAtual, proximaPosicaoX, proximaPosicaoY, restaurarTela);
            
            if (jogadorAtual->obterVoltarProMenu()) break;
            if (abriuMenu) continue;

            ControleMapa::aplicarLimitesDeMapa(proximaPosicaoX, proximaPosicaoY, matrizDoMapaAtual);
            processarInteracao(proximaPosicaoX, proximaPosicaoY, larguraDoTerminal);
            
            precisaRenderizar = true;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    return destinoViagemRapida;
}
