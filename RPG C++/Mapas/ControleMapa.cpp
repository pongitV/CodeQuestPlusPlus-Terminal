#include "ControleMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Gerenciadores/GerenciadorDebug.h"
#include "../Utilidades/ControleDeInput.h"
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
        TelaBestiario::exibirLista(jogador);
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
    Aparencia::exibirCabecalho(tituloDoCombate, Cor::VERMELHO);
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

    std::string textoDeControles = "W,A,S,D: Mover | I: Inventario | C: Ficha | B: Bestiario";
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
