#include "TelaDerrota.h"
#include "../../GerenciadorVisao.h"
#include "../../Terminal/TelasIDE/TelaDerrotaTerminal.h"
#include "../../Raycaster/Telas3D/TelaDerrota3D.h"

void TelaDerrota::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    if (GerenciadorVisao::obterInstancia().isVisao3DAtiva()) {
        TelaDerrota3D::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    } else {
        TelaDerrotaTerminal::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    }
}
