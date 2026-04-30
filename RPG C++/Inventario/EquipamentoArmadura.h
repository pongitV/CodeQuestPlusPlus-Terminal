#pragma once

#include <string>
#include <memory>

#include "Item.h"

class EquipamentoArmadura : public Item 
{
private:
    std::string nome;
    int reducaoFixa;

public:
    EquipamentoArmadura(std::string nome, int reducaoFixa, int preco = 3);
    
    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const override;

    std::string obterInfoStatus() const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};
