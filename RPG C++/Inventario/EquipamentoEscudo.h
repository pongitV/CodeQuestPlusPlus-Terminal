#pragma once

#include "Item.h"
#include <string>


class EquipamentoEscudo : public Item 
{
private:
    std::string nome;
    int reducaoFixa;
    int durabilidade;

public:
    EquipamentoEscudo(std::string nome, int reducaoFixa, int durabilidade, int preco = 3);
    
    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterDurabilidadeAtualEscudo() const override;
    int obterReducaoDanoFixaEscudo() const override;
    void reduzirDurabilidade(int qtd) override;
    void aumentarDurabilidade(int qtd) override;

    std::string obterInfoStatus() const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};