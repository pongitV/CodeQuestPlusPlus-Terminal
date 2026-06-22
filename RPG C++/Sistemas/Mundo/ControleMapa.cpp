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
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

namespace {
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

static bool s_exploracao3DAtiva = false;

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
    const std::function<void()>& acaoAposFadeInArte
) {
    Aparencia::limparTela();
    Aparencia::ocultarCursor();

    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();

    if (animar) {
    // Calcula a largura real das artes para garantir a centralizacao perfeita sempre
    int maxLarguraArte = 0;
    for (const auto& linha : arteDoMapa) {
        int w = Aparencia::obterComprimentoVisual(linha);
        if (w > maxLarguraArte) maxLarguraArte = w;
    }

    int maxLarguraTrans = 0;
    for (const auto& linha : arteTransicao) {
        int w = Aparencia::obterComprimentoVisual(linha);
        if (w > maxLarguraTrans) maxLarguraTrans = w;
    }

    // 1. Fade In do Titulo (1.5s = 15 frames x 100ms)
    Aparencia::animarFadeIn(15, 100, [&](int /*frame*/, int intensidade) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        std::string corRGB = Aparencia::obterCorRGBFade(corTema, intensidade);

        buffer << "\033[H\033[J";

        if (!arteDoMapa.empty()) {
            int espacos = std::max(0, (larguraTerminal - maxLarguraArte) / 2);
            std::string margem(espacos, ' ');
            buffer << "\n";
            for (const auto& linha : arteDoMapa) {
                buffer << margem << corRGB << linha << "\033[0m\n";
            }
        } else {
            int espacos = std::max(0, (larguraTerminal - (int)tituloDoMapa.length() - 10)) / 2;
            std::string tracos(tituloDoMapa.length() + 2, '=');
            buffer << "\n" << std::string(espacos, ' ') << corRGB << "  " << tracos << "  \n";
            buffer << std::string(espacos, ' ') << corRGB << "|| " << tituloDoMapa << " ||\n";
            buffer << std::string(espacos, ' ') << corRGB << "  " << tracos << "  \033[0m\n";
        }

        if (!arteTransicao.empty()) {
            int espacosTrans = std::max(0, (larguraTerminal - maxLarguraTrans) / 2);
            std::string margemTrans(espacosTrans, ' ');
            buffer << "\n";
            for (const auto& linha : arteTransicao) {
                buffer << margemTrans << corRGB << linha << "\033[0m\n";
            }
        }

        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });

    if (acaoAposFadeInArte) {
        acaoAposFadeInArte();
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    // 2. A tela desce (Scroll down)
    int linhasParaDescer = 5;
    if (!arteDoMapa.empty()) linhasParaDescer += arteDoMapa.size();
    if (!arteTransicao.empty()) linhasParaDescer += arteTransicao.size();

    for (int i = 0; i < linhasParaDescer / 2; ++i) {
        std::cout << "\n\n" << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    // 3. Fade In do Mapa (1.5s = 15 frames x 100ms)
    Aparencia::limparTela();
    
    Aparencia::exibirPainelTexto(tituloDoMapa, corTema);

    int linhaInicialMapa = Aparencia::obterPosicaoCursorY();

    Aparencia::animarFadeIn(15, 100, [&](int frame, int intensidade) {
        auto formatadorFade = [&](char celula, int x, int y) -> std::string {
            if (frame == 15) return formatadorCelula(celula, x, y);
            
            std::string stringFormatada = formatadorCelula(celula, x, y);
            std::string caractereLimpo = Aparencia::removerCoresANSI(stringFormatada);
            if (caractereLimpo == " " || caractereLimpo.empty()) return " ";
            
            std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
            return corRGB + caractereLimpo + "\033[0m";
        };

        renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorFade);
    });

    ControleDeInput::limparBuffer();
    
    return linhaInicialMapa;
    }

    Aparencia::exibirPainelTexto(tituloDoMapa, corTema);
    int linhaInicialMapa = Aparencia::obterPosicaoCursorY();
    renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
    return linhaInicialMapa;
}

void ControleMapa::calcularCameraVertical(int alturaDoTerminal, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = std::max(5, alturaDoTerminal - linhaInicial - 4);
    calcularCameraAxis(maxLinhasVisiveis, posicaoYDoJogador, tamanhoDoMapa, startY, endY);
}

void ControleMapa::calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX) {
    int maxColunasVisiveis = std::max(10, larguraDoTerminal - 1); // -1 para evitar quebras de linha acidentais
    calcularCameraAxis(maxColunasVisiveis, posicaoXDoJogador, larguraDoMapa, startX, endX);
}

std::string ControleMapa::calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto) {
    int espacos = (larguraDoTerminal - larguraDoTexto) / 2;
    return std::string(espacos > 0 ? espacos : 0, ' ');
}

void ControleMapa::padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa) {
    size_t maxLength = 0;
    for (const auto& linha : matrizDoMapa) {
        if (linha.length() > maxLength) maxLength = linha.length();
    }
    for (auto& linha : matrizDoMapa) {
        if (linha.length() < maxLength) linha.append(maxLength - linha.length(), ' ');
    }
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
    float anguloCamera3D = 0.0f; // Persiste a direcao da visao do jogador enquanto ele estiver no mapa
    float posCamera3DX = -1.0f;
    float posCamera3DY = -1.0f;

    while (exploracaoEstaAtiva && jogadorAtual->obterVida() > 0)
    {
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
            bool recemEntrouEm3D = !s_exploracao3DAtiva;
            s_exploracao3DAtiva = true;

                if (posCamera3DX == -1.0f || static_cast<int>(posCamera3DX) != posicaoXDoJogador || static_cast<int>(posCamera3DY) != posicaoYDoJogador) {
                    posCamera3DX = static_cast<float>(posicaoXDoJogador) + 0.5f;
                    posCamera3DY = static_cast<float>(posicaoYDoJogador) + 0.5f;
                }
                
                int hitX = -1, hitY = -1;
                char acaoPendente = Raycaster::iniciarExploracao3D(matrizDoMapaAtual, posCamera3DX, posCamera3DY, anguloCamera3D, tituloDoMapaAtual, jogadorAtual, hitX, hitY, recemEntrouEm3D);
                
                posicaoXDoJogador = static_cast<int>(posCamera3DX);
                posicaoYDoJogador = static_cast<int>(posCamera3DY);
                
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
                    s_exploracao3DAtiva = false; // Pausa a visao 3D para exibir o Mapa Mundial
                    restaurarTela();
                    precisaRenderizar = true;
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
                restaurarTela();
                precisaRenderizar = true;
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
