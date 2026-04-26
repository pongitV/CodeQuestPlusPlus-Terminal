#pragma once

#include <string>
#include <memory>

#include "Item.h"

class Armadura : public Item 
{
private:
    std::string nome;
    int reducaoFixa;

public:
    Armadura(std::string nome, int reducaoFixa, int preco = 3);
    
    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const override;

    std::string obterInfoStatus() const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};
