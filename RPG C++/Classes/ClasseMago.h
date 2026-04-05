#include "ClasseBase.h"

#pragma once

class Item;

class ClasseMago : public ClasseBase
{
public:
    std::string obterNomeClasse() const override;
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<Item*> gerarKitInicial() const override;

    void usarHabilidadeClasseAtiva(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasseAtiva() const override;
    std::string obterDescricaoHabilidadeClasseAtiva() const override;
};