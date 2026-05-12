#pragma once

#include <string>
#include <memory>

#include "Item.h"
#include "../Sistemas/SistemaPersonagem.h"

class EquipamentoArmadura : public Item 
{
private:
    std::string nome;
    int reducaoFixa;
    int reqResistencia;
    int reqConstituicao;
    int penalidadeDestreza;

public:
    EquipamentoArmadura(std::string nome, int reducaoFixa, int reqResistencia, int reqConstituicao, int preco = 3);
    
    int obterReqResistencia() const;
    int obterReqConstituicao() const;

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const override;
    void definirPenalidadeDestreza(int pen) { penalidadeDestreza = pen; }

    std::string obterInfoStatus() const override;

    bool podeSerEquipadoPor(SistemaPersonagem* personagem) const override;
    bool isEquipavel() const override { return true; }
    std::vector<std::string> obterDetalhesInspecao(SistemaPersonagem* personagem = nullptr) const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};

std::unique_ptr<Item> fabricarEquipamentoArmadura(ItemID id);
