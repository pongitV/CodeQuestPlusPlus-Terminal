#include "ControleMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaDiario.h"
#include "../Telas/TelaMenu.h"
#include "../Telas/TelaPause.h"
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Gerenciadores/GerenciadorDebug.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Utilidades/GeradorAleatorio.h"
#include <iostream>
#include <algorithm>

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

bool ControleMapa::processarInputEComandos(char tecla, SistemaPersonagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
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
        GerenciadorDebug::exibirMenuDebug(jogador);
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
    SistemaPersonagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<SistemaPersonagem>> inimigosParaBatalha, 
    int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    Aparencia::limparTela();
    Aparencia::exibirPainelTexto(tituloDoCombate, Cor::VERMELHO);
    int espacosParaCentralizarMensagem = std::max(0, (larguraDoTerminal - static_cast<int>(mensagemDeAviso.length())) / 2);
    std::string margemEsquerdaMensagem(espacosParaCentralizarMensagem, ' ');
    std::cout << "\n" << margemEsquerdaMensagem << Aparencia::cor(Cor::AMARELO) << "[!] " << mensagemDeAviso << Aparencia::cor(Cor::RESET) << "\n\n";
    
    std::vector<std::string> opcoesCombate = { "Nao, recuar", "Sim, batalha!" };
    int opcaoEscolhidaPeloJogador = ControleDeInput::lerSelecaoMenuComSetas(opcoesCombate, false, margemEsquerdaMensagem);

    if (opcaoEscolhidaPeloJogador == 1) {
        GerenciadorCombate combate(jogadorAtual, std::move(inimigosParaBatalha));
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < quantidadeDeCelulasOcupadas; ++i) matrizDoMapaAtual[posicaoYAposCombate][posicaoXInicialDoInimigo + i] = '.';
            posicaoXDoJogador = posicaoXAposCombate;
            posicaoYDoJogador = posicaoYAposCombate;
        }
    }

    if (exploracaoEstaAtiva) restaurarTela();
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
    restaurarTela();
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

void ControleMapa::calcularCameraVertical(int alturaDoTerminal, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = std::max(5, alturaDoTerminal - 7);
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

    std::string textoDeControles = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Diario";
    std::string margemEsquerdaControles = calcularMargemCentralizada(larguraDoTerminal, textoDeControles.length());

    Aparencia::moverCursor(0, linhaInicial);

    int startY, endY;
    calcularCameraVertical(alturaDoTerminal, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);

    for (int y = startY; y < endY; y++) {
        std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
        linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            linhaSendoRenderizada += formatadorCelula(matrizDoMapa[y][x], x, y);
        }
        std::cout << linhaSendoRenderizada << "\n";
    }
    std::cout << "\n" << margemEsquerdaControles << textoDeControles << std::flush;
}
