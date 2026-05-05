#include "ClasseBase.h"

#pragma once

class Item;

class Arqueiro : public ClasseBase 
{
public:
    // Metodos específicos para a classe Arqueiro
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Arqueiro; } 
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const override;
    int aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const override;
    int reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const override;

    // Criação e Definição da habilidade do Arqueiro: "Retirada com Pontaria"
    void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;

    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;

};