#pragma once

#include "ClasseBase.h"

class Item;
class Combate;

class Bardo : public ClasseBase
{
public:
    // INFORMACOES DA CLASSE
    std::string obterNomeClasse() const override; 
    TipoClasse obterTipoClasse() const override { return TipoClasse::Bardo; } 
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    int processarCuraPassivaBardo(int curaBase) const override;
    double processarMultiplicadorBuffPassivaBardo(double multBase) const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    void usarHabilidadeClasse(Combate* combate, Personagem* personagemUsuario, std::vector<Personagem*>& listaDeInimigos) override;
};
