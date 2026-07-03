#include "TelaDerrotaIDE.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaIDE.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaDerrotaIDE::exibir(Personagem* jogadorAtual, int, int, int totalDeDanoCausado, int, int, int turnosCombate) {
    Aparencia::limparTela();
    
    std::string tituloStr = TemaIDE::palavraChave("class") + " " + TemaIDE::tipo("TelaDerrota") + " {";
    
    // Fade in padronizado do titulo em Y=2
    {
        int targetY = 2;
        std::string linhaTitulo = Aparencia::espacosParaCentralizar(Aparencia::obterComprimentoVisual(tituloStr)) + tituloStr;
        
        Aparencia::animarFadeIn(30, 40, [&](int /*frame*/, int intensidade) {
            float pct = intensidade / 255.0f;
            int c = static_cast<int>(255 * pct);
            std::string corFade = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
            std::cout << "\033[" << (targetY + 1) << ";1H" << corFade
                      << Aparencia::removerCoresANSI(linhaTitulo) << "\033[0m\033[J" << std::flush;
        });
    }
    
    Aparencia::imprimirCentralizado(tituloStr);
    
    std::vector<std::string> codigo = {
        TemaIDE::palavraChave("public:"),
        "    " + TemaIDE::tipo("void") + " " + TemaIDE::funcao("exibirResultado") + "() {",
        "        " + TemaIDE::tipo("bool") + " " + TemaIDE::variavel("sobreviveu") + " = " + TemaIDE::palavraChave("false") + ";",
        "        " + TemaIDE::tipo("if") + " (!sobreviveu) {",
        "            " + TemaIDE::variavel("std::cout") + " << " + TemaIDE::stringLiteral(jogadorAtual->obterNome() + " pereceu em batalha...") + " << " + TemaIDE::variavel("std::endl") + ";",
        "            " + TemaIDE::variavel("estatisticas") + "->" + TemaIDE::funcao("imprimir") + "(" + TemaIDE::numero(std::to_string(turnosCombate)) + ", " + TemaIDE::numero(std::to_string(totalDeDanoCausado)) + ");",
        "            " + TemaIDE::palavraChave("throw") + " " + TemaIDE::tipo("std::runtime_error") + "(" + TemaIDE::stringLiteral("GAME OVER") + ");",
        "        }",
        "    }",
        "};"
    };

    std::cout << "\n";
    Aparencia::imprimirBlocoCentralizadoDigitando(codigo, 15);
    
    std::cout << "\n\n";
    std::string erroMsg = "\033[41;37m FATAL ERROR \033[0m: Unhandled exception em tempo de execucao: GAME OVER";
    Aparencia::imprimirCentralizado(erroMsg);
    
    std::cout << "\n\n";
    Aparencia::imprimirCentralizado(TemaIDE::comentario("Pressione [ENTER] para reiniciar a sessao..."));

    ControleDeInput::aguardarEnter();
}
