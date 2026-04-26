#include "EfeitosStatus.h"
#include "Personagem.h"
#include <iostream>

void EfeitoSugaSangue::aplicarInicioTurno(Personagem* alvo) {
    if (!atacante || atacante->obterVida() <= 0) return;
    int danoRaizes = alvo->obterVida() / 5;
    if (danoRaizes > 0) 
    {
        alvo->modificarVida(-danoRaizes);
        atacante->modificarVida(danoRaizes);
        std::cout << "\033[32m>> [" << nome << "]: Drenou " << danoRaizes << " de HP de " << alvo->obterNome() << " e curou " << atacante->obterNome() << "!\033[0m\n";
    }
}

void EfeitoLentidao::aoEntrar(Personagem* alvo) {
    if (alvo->obterTipoClasse() == TipoClasse::Arqueiro) {
        alvo->obterAtributosFinais().destreza = (alvo->obterAtributosFinais().destreza * 3) / 4;
    } else {
        alvo->obterAtributosFinais().destreza /= 2;
    }
}

void EfeitoLentidao::aoSair(Personagem* alvo) {
    if (alvo->obterTipoClasse() == TipoClasse::Arqueiro) {
        alvo->obterAtributosFinais().destreza = (alvo->obterAtributosFinais().destreza * 4) / 3;
    } else {
        alvo->obterAtributosFinais().destreza *= 2;
    }
    std::cout << "\033[35m[EFEITO]: " << alvo->obterNome() << " se livrou da gosma e recuperou sua agilidade.\033[0m\n";
}

void EfeitoFraqueza::aoEntrar(Personagem* alvo) {
    forcaPerdida = alvo->obterAtributosFinais().forca / 4;
    alvo->obterAtributosFinais().forca -= forcaPerdida;
}

void EfeitoFraqueza::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().forca += forcaPerdida;
    std::cout << "\033[31m[EFEITO]: " << alvo->obterNome() << " recuperou sua forca original.\033[0m\n";
}

void EfeitoQuebraResistencia::aoEntrar(Personagem* alvo) {
    resistenciaPerdida = alvo->obterAtributosFinais().resistencia / 2;
    constituicaoPerdida = alvo->obterAtributosFinais().constituicao / 3;
    alvo->obterAtributosFinais().resistencia -= resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao -= constituicaoPerdida;
}

void EfeitoQuebraResistencia::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().resistencia += resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao += constituicaoPerdida;
}

void EfeitoSangramento::aplicarInicioTurno(Personagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    std::cout << "\033[31m[EFEITO]: " << alvo->obterNome() << " sofreu " << danoPorTurno << " de dano por sangramento!\033[0m\n";
}

void EfeitoGritoGuerra::aoEntrar(Personagem* alvo) {
    alvo->obterAtributosFinais().forca += bonusForca;
    alvo->obterAtributosFinais().destreza += bonusDestreza;
}

void EfeitoGritoGuerra::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().forca -= bonusForca;
    alvo->obterAtributosFinais().destreza -= bonusDestreza;
}

void EfeitoBuffAtributos::aoSair(Personagem* alvo) {
    if (alvo->obterMultiplicador() != 1.0) {
        alvo->definirMultiplicador(1.0);
    }
    std::cout << "\n[SISTEMA]: O efeito da habilidade expirou!\n";
}
