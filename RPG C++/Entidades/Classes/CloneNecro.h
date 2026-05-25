#pragma once

#include "ClasseBase.h"
#include "../Racas/RacaBase.h"
#include <string>
#include <vector>
#include <memory>

class Item;
class Combate;
class Personagem;

class RacaClone : public RacaBase {
private:
    std::string nomeOriginal;
    std::vector<std::string> aparenciaOriginal;
public:
    RacaClone(const std::string& n, const std::vector<std::string>& a);
    std::string obterNomeRaca() const override;
    TipoRaca obterTipoRaca() const override;
    const std::vector<std::string>& obterAparenciaRaca() const override;
    Atributos obterAtributosRaca() const override;
    std::string obterNomeHabilidadeRaca() const override;
    std::string obterDescricaoHabilidadeRaca() const override;
};

class ClasseClone : public ClasseBase {
public:
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override;
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Atributos obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;
    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    void usarHabilidadeClasse(Combate*, Personagem*, std::vector<Personagem*>&) override;
};