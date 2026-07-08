#include "TelaAberturaIDE.h"
#include <iostream>
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "TelaMenuIDE.h"

void TelaAberturaIDE::exibir() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorNum = "\033[38;2;181;206;168m"; // Light Green
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::vector<std::string> blocoCentral;
    blocoCentral.push_back(colorComment + "// Iniciando boot da Engine..." + reset);
    blocoCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"Carregando modulos principais...\"" + colorPunct + ");");
    blocoCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Modulo Grafico Inicializado\"" + colorPunct + ");");
    blocoCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Sistema de Input Pronto\"" + colorPunct + ");");
    blocoCentral.push_back(colorType + "System" + colorPunct + "::" + colorKeyword + "out" + colorPunct + "." + colorKeyword + "println" + colorPunct + "(" + colorString + "\"[OK] Motor de Perspectiva (IDE) Ativo\"" + colorPunct + ");");
    blocoCentral.push_back("");
    blocoCentral.push_back(colorType + "GameEngine" + colorPunct + "::" + colorKeyword + "run" + colorPunct + "();");
    blocoCentral.push_back("");
    blocoCentral.push_back(colorComment + "// -> PRESSIONE QUALQUER TECLA PARA INICIAR <-" + reset);

    int espacosY = TelaMenuIDE::calcularEspacoY(blocoCentral.size());
    for (int i = 0; i < espacosY; ++i) std::cout << "\n";
    
    TelaMenuIDE::imprimirBlocoCentralizadoIDE(blocoCentral);

    std::cout << std::flush;

    ControleDeInput::limparBuffer();
    ControleDeInput::lerTecla(); // Sincrono e imediato
    ControleDeInput::limparBuffer();
}