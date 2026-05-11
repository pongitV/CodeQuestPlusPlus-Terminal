#include "GerenciadorEfeitosStatus.h"

#include <iostream>

#include "../Classes/ClasseBase.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

namespace {
    void notificarEfeito(Cor cor, const std::string& texto) {
        std::string msg = Aparencia::margemCombate() + Aparencia::cor(cor) + texto + Aparencia::cor(Cor::RESET) + "\n";
        TelaCombate::adicionarMensagemFixa(msg);
        Aparencia::registrarLogBatalha(Aparencia::cor(cor) + texto + Aparencia::cor(Cor::RESET));
    }
}

void EfeitoSugaSangue::aplicarInicioTurno(SistemaPersonagem* alvo) {
    if (!SistemaPersonagem::isValido(atacante) || atacante->obterVida() <= 0) return;
    int danoRaizes = alvo->obterVida() / 5;
    if (danoRaizes > 0) 
    {
        alvo->modificarVida(-danoRaizes);
        atacante->modificarVida(danoRaizes);
        notificarEfeito(Cor::VERDE, ">> [" + nome + "]: Drenou " + std::to_string(danoRaizes) + " de HP de " + alvo->obterNome() + " e curou " + atacante->obterNome() + "!");
    }
}

void EfeitoLentidao::aoEntrar(SistemaPersonagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->aplicarPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza /= 2;
}

void EfeitoLentidao::aoSair(SistemaPersonagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->reverterPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza *= 2;
    notificarEfeito(Cor::MAGENTA, "[EFEITO]: " + alvo->obterNome() + " se livrou da gosma e recuperou sua agilidade.");
}

void EfeitoFraqueza::aoEntrar(SistemaPersonagem* alvo) {
    forcaPerdida = alvo->obterAtributosFinais().forca / 4;
    alvo->obterAtributosFinais().forca -= forcaPerdida;
}

void EfeitoFraqueza::aoSair(SistemaPersonagem* alvo) {
    alvo->obterAtributosFinais().forca += forcaPerdida;
    notificarEfeito(Cor::VERMELHO, "[EFEITO]: " + alvo->obterNome() + " recuperou sua forca original.");
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
    notificarEfeito(Cor::CIANO, "[EFEITO]: " + alvo->obterNome() + " continua enfraquecido pelo po magico! (-" + std::to_string(resistenciaPerdida) + " Res, -" + std::to_string(constituicaoPerdida) + " Con)");
}

void EfeitoSangramento::aplicarInicioTurno(SistemaPersonagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    Cor corSangramento = (alvo->obterNomeClasse() != "Monstro") ? Cor::VERMELHO_CLARO : Cor::VERMELHO;
    notificarEfeito(corSangramento, "[EFEITO]: " + alvo->obterNome() + " sofreu " + std::to_string(danoPorTurno) + " de dano por sangramento!");
}

int EfeitoMetadeDano::processarDanoRecebido(int dano) {
    int danoReduzido = dano / 2;
    notificarEfeito(Cor::CIANO, ">> [EFEITO]: O dano foi reduzido pela metade! (Through the wire)");
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
    std::string msg = "\n" + Aparencia::margemCombate() + Aparencia::cor(Cor::VERDE_CLARO) + "[SISTEMA]: O efeito da habilidade expirou!" + Aparencia::cor(Cor::RESET) + "\n";
    TelaCombate::adicionarMensagemFixa(msg);
    Aparencia::registrarLogBatalha(Aparencia::cor(Cor::VERDE_CLARO) + "[SISTEMA]: O efeito da habilidade expirou!" + Aparencia::cor(Cor::RESET));
}
