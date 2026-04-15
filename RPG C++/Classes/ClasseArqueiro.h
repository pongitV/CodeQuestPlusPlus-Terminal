#include "ClasseBase.h"

#pragma once

class Item;

class ClasseArqueiro : public ClasseBase 
{
public:
    // Metodos específicos para a classe Arqueiro
    std::string obterNomeClasse() const override; 
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // Criação e Definição da habilidade do Arqueiro: "Retirada com Pontaria"
    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;

    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;

};