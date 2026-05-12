#include <iostream>
#include <vector>

#include "TelaDerrota.h"
#include "../Racas/RacaBase.h"
#include "TelaBase.h"
#include "../Utilidades/Aparencia.h"

void TelaDerrota::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    Aparencia::limparTela();
    int largura = Aparencia::obterLarguraTerminal();

    std::vector<std::string> logoDerrota = 
    {
        " ██████████   ██████████ ███████████   ███████████      ███████    ███████████   █████████           ",
        "░░███░░░░███ ░░███░░░░░█░░███░░░░░███ ░░███░░░░░███   ███░░░░░███ ░█░░░███░░░█  ███░░░░░███          ",
        " ░███   ░░███ ░███  █ ░  ░███    ░███  ░███    ░███  ███     ░░███░   ░███  ░  ░███    ░███          ",
        " ░███    ░███ ░██████    ░██████████   ░██████████  ░███      ░███    ░███     ░███████████          ",
        " ░███    ░███ ░███░░█    ░███░░░░░███  ░███░░░░░███ ░███      ░███    ░███     ░███░░░░░███          ",
        " ░███    ███  ░███ ░   █ ░███    ░███  ░███    ░███ ░░███     ███     ░███     ░███    ░███          ",
        " ██████████   ██████████ █████   █████ █████   █████ ░░░███████░      █████    █████   █████ ██ ██ ██",
        "░░░░░░░░░░   ░░░░░░░░░░ ░░░░░   ░░░░░ ░░░░░   ░░░░░    ░░░░░░░       ░░░░░    ░░░░░   ░░░░░ ░░ ░░ ░░ "
    };

    Aparencia::exibirLogoAscii(logoDerrota, 101, Cor::VERMELHO, "", 0);

    auto centralizarTomb = [](const std::string& txt, int width) {
        int pad = width - txt.length();
        if (pad < 0) return txt.substr(0, width);
        int padL = pad / 2;
        int padR = pad - padL;
        return std::string(padL, ' ') + txt + std::string(padR, ' ');
    };

    std::vector<std::string> lapideBuild = {
        "       _       ",
        "     /   \\     ",
        "   /       \\   ",
        "  |   RIP   |  ",
        "  |         |  ",
        "  | " + centralizarTomb(jogadorAtual->obterNome(), 7) + " |  ",
        "  | " + centralizarTomb("Nv " + std::to_string(jogadorAtual->obterNivel()), 7) + " |  ",
        "  |_________|  "
    };

    Aparencia::imprimirBlocoCentralizado(lapideBuild, Aparencia::cor(Cor::CINZA), 40);
    std::cout << "\n";
    
    std::string msg1 = "Voce sobreviveu por " + std::to_string(turnosCombate) + " turnos e causou " + std::to_string(totalDeDanoCausado) + " de dano.";
    std::string msg2 = "Mas nao foi o suficiente...";
    
    Aparencia::imprimirCentralizadoDigitando(msg1, 30);
    Aparencia::imprimirCentralizadoDigitando(msg2, 50);
    std::cout << "\n\n";

    std::vector<std::string> dicas = {
        " Lembre-se de utilizar suas habilidades e itens estrategicamente.",
        " Se o inimigo for muito forte, tente melhorar seus equipamentos",
        " na Forja do Bjorn ou ganhar mais niveis antes de tentar novamente!"
    };
    
    std::vector<std::string> caixaDica = Aparencia::criarCaixa(dicas, "DICA DE COMBATE", 70, Cor::CINZA);
    Aparencia::imprimirBlocoCentralizado(caixaDica);
    std::cout << "\n";

    Aparencia::aguardarEnter();
}