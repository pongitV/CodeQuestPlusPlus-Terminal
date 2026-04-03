#include <vector> 

#include "ClasseBase.h"

#pragma once

class Item;

class ClasseArqueiro : public ClasseBase 
{
public:
    std::string obterNomeClasse() const override; 
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<Item*> gerarKitInicial() const override;

    void usarHabilidadeClasse(Personagem* usuario, Personagem* alvo) override;
};