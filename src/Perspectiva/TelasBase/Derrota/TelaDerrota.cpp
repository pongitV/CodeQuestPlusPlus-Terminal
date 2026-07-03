#include "TelaDerrota.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaDerrotaIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaDerrotaRaycaster.h"

void TelaDerrota::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaDerrotaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    } else {
        TelaDerrotaIDE::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    }
}
