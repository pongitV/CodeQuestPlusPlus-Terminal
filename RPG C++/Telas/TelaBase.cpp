#include "TelaBase.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Racas/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/TiposInventarios/Inventario.h"

std::vector<std::string> TelaBase::comporEstatisticasBatalha(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) 
{
    return {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "",
        "═══ ESTATISTICAS DA BATALHA ═══",
        "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G",
        "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP",
        "DANO TOTAL CAUSADO:   " + std::to_string(totalDeDanoCausado),
        "DANO TOTAL RECEBIDO:  " + std::to_string(totalDeDanoRecebido),
        "CURA TOTAL RECEBIDA:  " + std::to_string(curaTotalRecebida),
        "NUMERO DE TURNOS:         " + std::to_string(turnosCombate)
    };
}

bool TelaBase::deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos) {
    auto agora = std::chrono::steady_clock::now();
    bool animar = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoAcesso).count() > delayMilissegundos;
    ultimoAcesso = agora;
    return animar;
}
