#pragma once

#include <string>
#include <vector>

enum class Cor {
    RESET = 0,
    NEGRITO = 1,
    VERMELHO = 31,
    VERDE = 32,
    AMARELO = 33,
    AZUL = 34,
    MAGENTA = 35,
    CIANO = 36,
    BRANCO = 37,
    CINZA = 90,
    LARANJA = 208,
    FUNDO_VERMELHO = 41,
    FUNDO_VERDE = 42,
    FUNDO_AMARELO = 43,
    FUNDO_AZUL = 44,
    FUNDO_MAGENTA = 45,
    FUNDO_CIANO = 46,
    FUNDO_BRANCO = 47
};

class SimplificacoesAparencia {
public:
    static void inicializarConsole();
    static std::string cor(Cor codigo);
    static std::string cor(Cor estilo, Cor codigo);

    static void maximizarJanelaTerminal();
    static void ocultarCursor();
    static void limparTela();
    static void aguardarEnter();
    static int obterLarguraTerminal();
    static int obterAlturaTerminal();
    static void moverCursor(int x, int y);
    static int obterPosicaoCursorY();
    
    static std::string removerCoresANSI(const std::string& texto);
    static std::string espacosParaCentralizar(int comprimentoTexto);
    static void imprimirLinhaDivisoria(char caractere = '=');
    static void imprimirCentralizado(const std::string& texto, const std::string& corAnsi = "");
    static void imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual = 0, const std::string& corAnsi = "");
    static void imprimirDigitando(const std::string& texto, int atrasoMs);
    static void exibirCabecalho(const std::string& titulo, Cor corDoCabecalho = Cor::BRANCO);
    static int imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda = 0, int espacamento = 0, Cor corEsquerda = Cor::RESET, Cor corDireita = Cor::RESET);
};
