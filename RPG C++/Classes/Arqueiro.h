#pragma once

#include "ClasseBase.h"

class Item;

class Arqueiro : public ClasseBase 
{
public:
    // INFORMACOES DA CLASSE
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Arqueiro; } 
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const override;
    int aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const override;
    int reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) override;
};