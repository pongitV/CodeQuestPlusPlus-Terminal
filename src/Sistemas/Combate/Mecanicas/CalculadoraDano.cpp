#include "CalculadoraDano.h"
#include "../../../Entidades/Personagem.h"
#include "../../Inventario/Equipamentos/EquipamentoArma.h"
#include "../../../Entidades/Inimigos/Mahoraga.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include <algorithm>

std::pair<int, int> CalculadoraDano::calcularDanoOfensivoBase(Personagem* atacante) {
    double multiplicadorDeAtributos = atacante->obterMultiplicador();

    int danoFisicoDaArma = 1;
    int danoMagicoDaArma = 0;
    int perfuranteAtual = 0;

    if (atacante->obterArma()) 
    {
        danoFisicoDaArma = atacante->obterArma()->obterDanoFisico();
        danoMagicoDaArma = atacante->obterArma()->obterDanoMagico();

        if (atacante->obterArma()->temPropriedade(Propriedade::Magica)) {
            int bonusMagico = danoFisicoDaArma / 2;
            double bonusEscalado = bonusMagico * (1.0 + (atacante->obterSabedoria() / 100.0));
            perfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    int forcaEfetiva = atacante->obterForca();
    int destrezaEfetiva = atacante->obterDestreza();
    int inteligenciaEfetiva = atacante->obterInteligencia();
    int sabedoriaEfetiva = atacante->obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva /= 10; destrezaEfetiva /= 10;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        inteligenciaEfetiva /= 10; sabedoriaEfetiva /= 10;
    }

    int danoFisicoCalculado = std::max(0, static_cast<int>((danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0))));
    int danoMagicoCalculado = std::max(0, static_cast<int>((danoMagicoDaArma + inteligenciaEfetiva) * (1.0 + (sabedoriaEfetiva / 100.0))));
    
    int total = std::max(1, danoFisicoCalculado + danoMagicoCalculado);
    int totalFinal = static_cast<int>(total * multiplicadorDeAtributos);
    int perfuranteFinal = perfuranteAtual;

    if (atacante->obterArma() && atacante->obterArma()->temPropriedade(Propriedade::IgnoraDefesa)) {
        perfuranteFinal = totalFinal;
    }

    // Logica de adaptacao do Mahoraga para ignorar escudos
    if (atacante->obterTipoRaca() == TipoRaca::Mahoraga) {
        auto* mahoraga = dynamic_cast<Mahoraga*>(atacante->obterRaca());
        if (mahoraga && mahoraga->ignoraEscudo()) {
            perfuranteFinal = totalFinal;
        }
    }

    // Aplica o bonus da Mira Certeira diretamente no dano base (sem mexer nos atributos)
    if (atacante->possuiEfeito(EfeitoID::MiraCerteira)) {
        totalFinal *= 2;
        perfuranteFinal *= 2;
        atacante->removerEfeito(EfeitoID::MiraCerteira);
    }

    return { totalFinal, perfuranteFinal };
}

int CalculadoraDano::calcularMitigacaoDefensiva(Personagem* alvo, int danoBruto, int danoPerfurante) {
    return alvo->calcularDefesaBase(danoBruto, danoPerfurante);
}
