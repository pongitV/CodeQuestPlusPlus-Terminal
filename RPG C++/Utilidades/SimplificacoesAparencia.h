#pragma once
#include <string>
#include <vector>

namespace Cor {
    constexpr int RESET = 0;
    constexpr int NEGRITO = 1;
    constexpr int VERMELHO = 31;
    constexpr int VERDE = 32;
    constexpr int AMARELO = 33;
    constexpr int AZUL = 34;
    constexpr int MAGENTA = 35;
    constexpr int CIANO = 36;
    constexpr int BRANCO = 37;
    constexpr int CINZA = 90;
    constexpr int FUNDO_VERMELHO = 41;
    constexpr int FUNDO_VERDE = 42;
    constexpr int FUNDO_AMARELO = 43;
    constexpr int FUNDO_AZUL = 44;
}

class SimplificacoesAparencia {
public:
    static std::string cor(int codigo);
    static std::string cor(int estilo, int codigo);
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
