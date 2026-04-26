#include "ClasseBase.h"

#pragma once

class Item;

class ClasseMago : public ClasseBase
{
private:
    TipoAtaque tipoAtaqueAtual = TipoAtaque::UNICO;

public:
    // Metodos específicos para a classe Mago
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override { return TipoClasse::Mago; }
    std::vector<std::string> obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    // Criação e Definição da habilidade do Mago: "Estratégia Arcana"
    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;

    // Sobrescreve para processar a passiva Foco Arcano
    void executarAtaqueComPassivaDaClasse(Personagem* atacante, Personagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<Personagem>>& inimigos, std::function<void(Personagem*, Personagem*, int, int)> aplicarDano) override;

};