#include <iostream>
#include <vector>

#include "TelaDerrota.h"
#include "../Racas/RacaBase.h"
#include "TelaBase.h"
#include "../Utilidades/Aparencia.h"
#include "TelaDerrotaLayouts.h"
#include "../Utilidades/ControleDeInput.h"

void TelaDerrota::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    Aparencia::limparTela();
    int largura = Aparencia::obterLarguraTerminal();

    Aparencia::exibirLogoAscii(ArtesDerrota::logoDerrota, 101, Cor::VERMELHO, "", true);

    Aparencia::imprimirBlocoCentralizado(ArtesDerrota::arteDerrota, Aparencia::cor(Cor::CINZA));
    std::cout << "\n";

    std::string msgMorte = jogadorAtual->obterNome() + " pereceu em batalha, lutando ate seu ultimo suspiro...";
    std::string msg1 = "Voce sobreviveu por " + std::to_string(turnosCombate) + " turnos e causou " + std::to_string(totalDeDanoCausado) + " de dano.";
    std::string msg2 = "Mas nao foi o suficiente...";
    
    Aparencia::imprimirCentralizadoDigitando(msgMorte, 40);
    std::cout << "\n";
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

    ControleDeInput::aguardarEnter();
}