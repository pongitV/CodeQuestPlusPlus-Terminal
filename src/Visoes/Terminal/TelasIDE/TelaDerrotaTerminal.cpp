#include "TelaDerrotaTerminal.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaTerminal.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"

void TelaDerrotaTerminal::exibir(Personagem* jogadorAtual, int, int, int totalDeDanoCausado, int, int, int turnosCombate) {
    Aparencia::limparTela();
    
    std::string tituloStr = TemaTerminal::palavraChave("class") + " " + TemaTerminal::tipo("TelaDerrota") + " {";
    
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
        TemaTerminal::palavraChave("public:"),
        "    " + TemaTerminal::tipo("void") + " " + TemaTerminal::funcao("exibirResultado") + "() {",
        "        " + TemaTerminal::tipo("bool") + " " + TemaTerminal::variavel("sobreviveu") + " = " + TemaTerminal::palavraChave("false") + ";",
        "        " + TemaTerminal::tipo("if") + " (!sobreviveu) {",
        "            " + TemaTerminal::variavel("std::cout") + " << " + TemaTerminal::stringLiteral(jogadorAtual->obterNome() + " pereceu em batalha...") + " << " + TemaTerminal::variavel("std::endl") + ";",
        "            " + TemaTerminal::variavel("estatisticas") + "->" + TemaTerminal::funcao("imprimir") + "(" + TemaTerminal::numero(std::to_string(turnosCombate)) + ", " + TemaTerminal::numero(std::to_string(totalDeDanoCausado)) + ");",
        "            " + TemaTerminal::palavraChave("throw") + " " + TemaTerminal::tipo("std::runtime_error") + "(" + TemaTerminal::stringLiteral("GAME OVER") + ");",
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
    Aparencia::imprimirCentralizado(TemaTerminal::comentario("Pressione [ENTER] para reiniciar a sessao..."));

    ControleDeInput::aguardarEnter();
}
