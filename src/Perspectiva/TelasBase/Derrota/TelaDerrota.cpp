#include "TelaDerrota.h"
#include "../../GerenciadorPerspectiva.h"

void TelaDerrota::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    GerenciadorPerspectiva::obterDerrotaUI().exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
}
