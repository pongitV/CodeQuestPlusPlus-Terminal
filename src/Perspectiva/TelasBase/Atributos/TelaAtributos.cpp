#include "TelaAtributos.h"
#include "../../GerenciadorPerspectiva.h"
#include "../TelaBase.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Sistemas/Inventario/Item.h"

PoderCombate TelaAtributos::calcularPoderCombate(Personagem* jogadorAtual, double multiplicador) {
    int danoFis = 1, danoMag = 0;
    if (jogadorAtual->obterArma()) {
        danoFis = jogadorAtual->obterArma()->obterDanoFisico();
        danoMag = jogadorAtual->obterArma()->obterDanoMagico();
    }
    int forca = jogadorAtual->obterForca();
    int destreza = jogadorAtual->obterDestreza();
    int inteli = jogadorAtual->obterInteligencia();
    int sabedoria = jogadorAtual->obterSabedoria();

    if (danoFis == 0 && danoMag > 0) { forca /= 10; destreza /= 10; }
    else if (danoFis > 0 && danoMag == 0) { inteli /= 10; sabedoria /= 10; }

    PoderCombate p;
    p.danoFisEst = std::max(0, static_cast<int>((danoFis + forca) * (1.0 + (destreza / 100.0)) * multiplicador));
    p.danoMagEst = std::max(0, static_cast<int>((danoMag + inteli) * (1.0 + (sabedoria / 100.0)) * multiplicador));
    p.defFixa = jogadorAtual->obterResistencia();
    p.mitigacao = std::min(50.0, jogadorAtual->obterConstituicao() / 2.0);
    return p;
}

DebuffInfo TelaAtributos::calcularDebuff(Personagem* jogadorAtual) {
    DebuffInfo d;
    d.temBuff = (jogadorAtual->obterTurnosEfeito(EfeitoID::BuffAtributos) > 0 &&
                 jogadorAtual->obterMultiplicador() > 1.0);
    d.forcaPerdida    = jogadorAtual->possuiEfeito(EfeitoID::Fraqueza)     ? (jogadorAtual->obterForca() / 3)      : 0;
    d.destrezaPerdida = jogadorAtual->possuiEfeito(EfeitoID::Lentidao)     ? jogadorAtual->obterDestreza()       : 0;
    d.resPerdida      = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? jogadorAtual->obterResistencia() : 0;
    d.constPerdida    = jogadorAtual->possuiEfeito(EfeitoID::QuebraResistencia) ? (jogadorAtual->obterConstituicao() / 2) : 0;
    return d;
}

void TelaAtributos::exibir(Personagem* jogadorAtual)
{
    GerenciadorPerspectiva::obterAtributosUI().exibir(jogadorAtual);
}

void TelaAtributos::gerenciarFichaDoJogador(Personagem* jogadorAtual)
{
    if (jogadorAtual == nullptr) return;

    GerenciadorPerspectiva::obterAtributosUI().gerenciarFichaDoJogador(jogadorAtual);
}
