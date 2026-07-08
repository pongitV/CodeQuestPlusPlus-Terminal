#include "TelaNomeIDE.h"
#include "TelaMenuIDE.h"
#include <iostream>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

TelaNome::Resultado TelaNomeIDE::exibir() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::string nome;
    
    while (true) {
        std::cout << "\033[?25l";
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// O mundo clama por um novo destino..." + reset);
        blocoCentral.push_back(colorComment + "// E todas as lendas possuem um nome." + reset);
        blocoCentral.push_back(colorType + "std" + colorPunct + "::" + colorType + "string " + colorPunct + "playerName;");
        
        if (!nome.empty() && nome.length() > 20) {
            blocoCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "err" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"Erro: Nome muito longo (max 20 chars).\"" + colorPunct + ");");
        }
        
        blocoCentral.push_back(colorType + "std" + colorPunct + "::" + colorKeyword + "cin " + colorPunct + ">> playerName; " + colorComment + "// (Digite o nome ou '0' para voltar)" + reset);
        
        int espacosY = TelaMenuIDE::calcularEspacoY(blocoCentral.size() + 1); // +1 pro input
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";
        
        TelaMenuIDE::imprimirBlocoCentralizadoIDE(blocoCentral);
        
        std::string promptStr = colorPunct + "> " + reset;
        int espacosX = std::max(0, (Aparencia::obterLarguraTerminal() - Aparencia::obterComprimentoVisual(promptStr)) / 2);
        std::string pad(espacosX, ' ');
        std::cout << pad << promptStr;
        
        std::cout << "\033[?25h"; // Mostra cursor
        nome = ControleDeInput::lerEntradaProtegida();
        
        if (nome == "0") {
            TelaNome::Resultado r;
            r.voltou = true;
            return r;
        }
        
        if (!nome.empty() && nome.length() <= 20) {
            break;
        }
    }

    std::cout << "\033[?25l"; // Esconde cursor
    
    std::vector<std::string> info = { "O nome " + nome + " sera registrado." };
    bool confirmou = TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado("NOME", nome, info, {});
    
    if (!confirmou) {
        return exibir(); // Chama recursivo se voltar
    }

    TelaNome::Resultado r;
    r.nome = nome;
    return r;
}