#pragma once

#include <string>
#include <vector>

#include "../Classes/ClasseBase.h"

class ClasseInimigoPadrao : public ClasseBase
{
public:
    std::string obterNomeClasse() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;
};