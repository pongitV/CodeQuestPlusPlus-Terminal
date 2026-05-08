#pragma once

#include "ClasseBase.h"

class Item;

class Guerreiro : public ClasseBase
{
public:
    // INFORMACOES DA CLASSE
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Guerreiro; } 
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) override;

protected:
    // PROCESSAMENTO DE DANO 
    int processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) override;
};  
