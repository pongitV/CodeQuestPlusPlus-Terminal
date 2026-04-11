#include "ClasseBase.h"

#pragma once

class Item;

class ClasseGuerreiro : public ClasseBase
{
public:
    // Metodos específicos para a classe Guerreiro
    std::string obterNomeClasse() const override; 
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<Item*> obterEquipamentoClasse() const override;

    // Criação e Definição da habilidade do Guerreiro: "Determinação no Combate"
    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
};  
