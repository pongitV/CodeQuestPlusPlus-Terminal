#include "ClasseBase.h"

#pragma once

class Item;

class ClasseBardo : public ClasseBase
{
public:
    // Metodos específicos para a classe Bardo
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Bardo; } 
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    int processarCuraPassivaBardo(int curaBase) const override;
    double processarMultiplicadorBuffPassivaBardo(double multBase) const override;

    // Criação e Definição da habilidade do Bardo: "Flashing Lights"
    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;
};