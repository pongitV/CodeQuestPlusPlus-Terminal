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
    FUNDO_VERMELHO = 41,
    FUNDO_VERDE = 42,
    FUNDO_AMARELO = 43,
    FUNDO_AZUL = 44
};

class SimplificacoesAparencia {
public:
    static void inicializarConsole();
    static std::string cor(Cor codigo);
    static std::string cor(Cor estilo, Cor codigo);
    static std::string corRGBFundo(int idANSI);
    static std::string corRGBTexto(int idANSI);

    static void maximizarJanelaTerminal();
    static void limparTela();
    static void aguardarEnter();
    static int obterLarguraTerminal();
    
    static std::string removerCoresANSI(const std::string& texto);
    static void imprimirCentralizado(const std::string& texto, const std::string& corAnsi = "");
    static void imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual = 0, const std::string& corAnsi = "");
    static void imprimirDigitando(const std::string& texto, int atrasoMs);
};
