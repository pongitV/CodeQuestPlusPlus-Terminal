#include "Status.h"

#include <iostream>

#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Entidades/Personagem.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/FuncoesDialogo.h"
#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"

void EfeitoSugaSangue::aplicarInicioTurno(Personagem* alvo) {
    if (!Personagem::isValido(atacante) || atacante->obterVida() <= 0) return;
    int danoRaizes = alvo->obterVida() / 5;
    if (danoRaizes > 0) 
    {
        alvo->modificarVida(-danoRaizes);
        atacante->modificarVida(danoRaizes);
        Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus("Drenou " + std::to_string(danoRaizes) + " de HP de " + alvo->obterNome() + " e curou " + atacante->obterNome() + "!", Cor::VERDE));
    }
}

void EfeitoNecrose::aplicarInicioTurno(Personagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    Cor corNecrose = (alvo->obterNomeClasse() != "Monstro") ? Cor::VERMELHO_CLARO : Cor::VERMELHO;
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus(alvo->obterNome() + " sofreu " + std::to_string(danoPorTurno) + " de dano por necrose!", corNecrose));
}

void EfeitoLentidao::aoEntrar(Personagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->aplicarPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza /= 2;
}

void EfeitoLentidao::aoSair(Personagem* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().destreza = alvo->obterClasse()->reverterPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().destreza);
    else alvo->obterAtributosFinais().destreza *= 2;
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus(alvo->obterNome() + " se livrou da gosma e recuperou sua agilidade.", Cor::MAGENTA));
}

void EfeitoFraqueza::aoEntrar(Personagem* alvo) {
    forcaPerdida = alvo->obterAtributosFinais().forca / 4;
    alvo->obterAtributosFinais().forca -= forcaPerdida;
}

void EfeitoFraqueza::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().forca += forcaPerdida;
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus(alvo->obterNome() + " recuperou sua forca original.", Cor::VERMELHO));
}

void EfeitoQuebraResistencia::aoEntrar(Personagem* alvo) {
    resistenciaPerdida = static_cast<int>(alvo->obterAtributosFinais().resistencia * 0.20);
    constituicaoPerdida = static_cast<int>(alvo->obterAtributosFinais().constituicao * 0.10);
    alvo->obterAtributosFinais().resistencia -= resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao -= constituicaoPerdida;
}

void EfeitoQuebraResistencia::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().resistencia += resistenciaPerdida;
    alvo->obterAtributosFinais().constituicao += constituicaoPerdida;
}

void EfeitoQuebraResistencia::aplicarInicioTurno(Personagem* alvo) {
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus(alvo->obterNome() + " continua enfraquecido pelo po magico! (-" + std::to_string(resistenciaPerdida) + " Res, -" + std::to_string(constituicaoPerdida) + " Con)", Cor::CIANO));
}

void EfeitoSangramento::aplicarInicioTurno(Personagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    Cor corSangramento = (alvo->obterNomeClasse() != "Monstro") ? Cor::VERMELHO_CLARO : Cor::VERMELHO;
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus(alvo->obterNome() + " sofreu " + std::to_string(danoPorTurno) + " de dano por sangramento!", corSangramento));
}

int EfeitoMetadeDano::processarDanoRecebido(int dano) {
    int danoReduzido = dano / 2;
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgStatus("O dano foi reduzido pela metade! (Through the wire)", Cor::CIANO));
    return danoReduzido;
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
    Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgSistema("O efeito da habilidade expirou!", Cor::VERDE_CLARO));
}

void EfeitoRodaAdaptacao::aplicarInicioTurno(Personagem* alvo) {
    if (alvo->obterVida() <= 0) return;
    if (!alvo->obterArmadura() || !alvo->obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) return;
    
    int cura = alvo->obterVidaMaxima() * 0.05;
    if (cura > 0) {
        alvo->modificarVida(cura);
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + Aparencia::cor(Cor::VERDE) + ">> A Roda gira... Regenerou " + std::to_string(cura) + " HP!" + Aparencia::cor(Cor::RESET) + "\n");
        Aparencia::registrarLogBatalha(FuncoesDialogo::formatarMsgHabilidade("A Roda gira... Regenerou " + std::to_string(cura) + " HP!", Cor::VERDE));
    }
}

void EfeitoRodaAdaptacao::aoSair(Personagem* alvo) {
    alvo->obterAtributosFinais().forca -= bForca;
    alvo->obterAtributosFinais().destreza -= bDestreza;
    alvo->obterAtributosFinais().resistencia -= bResistencia;
    alvo->obterAtributosFinais().constituicao -= bConstituicao;
    alvo->obterAtributosFinais().inteligencia -= bInteligencia;
    alvo->obterAtributosFinais().sabedoria -= bSabedoria;
    alvo->forcarRecalculoCache();
}

void EfeitoRodaAdaptacao::adaptar(Personagem* alvo, Personagem* inimigo) {
    if (!inimigo) return;
    
    // --- 1. Adaptacao Defensiva (Baseada no inimigo) ---
    int forcaFisicaInimigo = inimigo->obterForca() + inimigo->obterDestreza();
    int forcaMagicaInimigo = inimigo->obterInteligencia() + inimigo->obterSabedoria();
    
    std::string msgDefesa;
    if (forcaFisicaInimigo >= forcaMagicaInimigo) {
        alvo->alterarAtributoEstatico(TipoAtributo::Resistencia, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Constituicao, 2);
        bResistencia += 2; bConstituicao += 2;
        msgDefesa = "defesa fisica";
    } else {
        alvo->alterarAtributoEstatico(TipoAtributo::Sabedoria, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Constituicao, 2);
        bSabedoria += 2; bConstituicao += 2;
        msgDefesa = "defesa magica";
    }

    // --- 2. Adaptacao Ofensiva (Baseada na arma do jogador) ---
    int danoFisicoArma = 1;
    int danoMagicoArma = 0;
    if (alvo->obterArma()) {
        danoFisicoArma = alvo->obterArma()->obterDanoFisico();
        danoMagicoArma = alvo->obterArma()->obterDanoMagico();
    }

    std::string msgAtaque;
    if (danoMagicoArma > danoFisicoArma) {
        alvo->alterarAtributoEstatico(TipoAtributo::Inteligencia, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Sabedoria, 2);
        bInteligencia += 2; bSabedoria += 2;
        msgAtaque = "poder magico";
    } else if (danoFisicoArma > danoMagicoArma) {
        alvo->alterarAtributoEstatico(TipoAtributo::Forca, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Destreza, 2);
        bForca += 2; bDestreza += 2;
        msgAtaque = "poder fisico";
    } else {
        // Armas hibridas (ex: Espada de Exterminio)
        alvo->alterarAtributoEstatico(TipoAtributo::Forca, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Destreza, 2);
        alvo->alterarAtributoEstatico(TipoAtributo::Inteligencia, 2);
        bForca += 2; bDestreza += 2; bInteligencia += 2;
        msgAtaque = "poder hibrido";
    }

    TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + "\033[5m" + Aparencia::cor(Cor::AMARELO) + "* KLINK! *" + Aparencia::cor(Cor::RESET) + " A Roda adapta " + msgDefesa + " e " + msgAtaque + " (+2)!\n");
}
void EfeitoInviolavel::aoSair(Personagem* alvo) { alvo->adicionarEfeito(std::make_unique<EfeitoMiraCerteira>(99)); }
