#include "ClasseBase.h"

#pragma once

class Item;

class Guerreiro : public ClasseBase
{
public:
    // Metodos específicos para a classe Guerreiro
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Guerreiro; } 
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    // Criação e Definição da habilidade do Guerreiro: "Determinação no Combate"
    void usarHabilidadeClasse(SistemaPersonagem* personagemUsuario, std::vector<SistemaPersonagem*>& listaDeInimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;

protected:
    int processarDanoPreAtaque(SistemaPersonagem* atacante, SistemaPersonagem* defensor, int danoBase, bool isAtacanteJogador, size_t qtdInimigos) override;
};  
