#include "GerenciadorEfeitosStatus.h"

#include <iostream>

#include "../Classes/ClasseBase.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void EfeitoSugaSangue::aplicarInicioTurno(SistemaPersonagem* alvo) {
    if (!SistemaPersonagem::isValido(atacante) || atacante->obterVida() <= 0) return;
    int danoRaizes = alvo->obterVida() / 5;
    if (danoRaizes > 0) 
    {
        alvo->modificarVida(-danoRaizes);
        atacante->modificarVida(danoRaizes);
        std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << ">> [" << nome << "]: Drenou " << danoRaizes << " de HP de " << alvo->obterNome() << " e curou " << atacante->obterNome() << "!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }
}

void EfeitoLentidao::aoEntrar(SistemaPersonagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->aplicarPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza /= 2;
}

void EfeitoLentidao::aoSair(SistemaPersonagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->reverterPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza *= 2;
    std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << "[EFEITO]: " << alvo->obterNome() << " se livrou da gosma e recuperou sua agilidade." << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

void EfeitoFraqueza::aoEntrar(SistemaPersonagem* alvo) {
    forcaPerdida = alvo->obterAtributosFinais().forca / 4;
    alvo->obterAtributosFinais().forca -= forcaPerdida;
}

void EfeitoFraqueza::aoSair(SistemaPersonagem* alvo) {
    alvo->obterAtributosFinais().forca += forcaPerdida;
    std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[EFEITO]: " << alvo->obterNome() << " recuperou sua forca original." << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

void EfeitoQuebraResistencia::aoEntrar(SistemaPersonagem* alvo) {
    resistenciaPerdida = static_cast<int>(alvo->obterAtributosFinais().resistencia * 0.20);
    constituicaoPerdida = static_cast<int>(alvo->obterAtributosFinais().constituicao * 0.10);
    alvo->obterAtributosFinais().resistencia -= resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao -= constituicaoPerdida;
}

void EfeitoQuebraResistencia::aoSair(SistemaPersonagem* alvo) {
    alvo->obterAtributosFinais().resistencia += resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao += constituicaoPerdida;
}

void EfeitoQuebraResistencia::aplicarInicioTurno(SistemaPersonagem* alvo) {
    std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << "[EFEITO]: " << alvo->obterNome() << " continua enfraquecido pelo po magico! (-" << resistenciaPerdida << " Res, -" << constituicaoPerdida << " Con)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

void EfeitoSangramento::aplicarInicioTurno(SistemaPersonagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[EFEITO]: " << alvo->obterNome() << " sofreu " << danoPorTurno << " de dano por sangramento!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
}

int EfeitoMetadeDano::processarDanoRecebido(int dano) {
    int danoReduzido = dano / 2;
    std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << ">> [EFEITO]: O dano foi reduzido pela metade! (Through the wire)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    return danoReduzido;
}

void EfeitoGritoGuerra::aoEntrar(SistemaPersonagem* alvo) {
    alvo->obterAtributosFinais().forca += bonusForca;
    alvo->obterAtributosFinais().destreza += bonusDestreza;
}

void EfeitoGritoGuerra::aoSair(SistemaPersonagem* alvo) {
    alvo->obterAtributosFinais().forca -= bonusForca;
    alvo->obterAtributosFinais().destreza -= bonusDestreza;
}

void EfeitoBuffAtributos::aoSair(SistemaPersonagem* alvo) {
    if (alvo->obterMultiplicador() != 1.0) {
        alvo->definirMultiplicador(1.0);
    }
    std::cout << "\n[SISTEMA]: O efeito da habilidade expirou!\n";
}
