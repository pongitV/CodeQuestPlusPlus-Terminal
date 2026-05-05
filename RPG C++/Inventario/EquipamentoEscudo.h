#pragma once

#include "Item.h"
#include <string>
#include <memory>
#include "../Sistemas/SistemaPersonagem.h"


class EquipamentoEscudo : public Item 
{
private:
    std::string nome;
    int reducaoFixa;
    int durabilidade;
    int reqResistencia;
    int reqSecundario;
    TipoAtributo tipoSecundario;

public:
    EquipamentoEscudo(std::string nome, int reducaoFixa, int durabilidade, int reqResistencia, int reqSecundario, TipoAtributo tipoSecundario, int preco = 3);
    
    int obterReqResistencia() const;
    int obterReqSecundario() const;
    TipoAtributo obterTipoSecundario() const;

    std::string obterNomeItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterDurabilidadeAtualEscudo() const override;
    int obterReducaoDanoFixaEscudo() const override;
    void reduzirDurabilidade(int qtd) override;
    void aumentarDurabilidade(int qtd) override;

    std::string obterInfoStatus() const override;

    bool podeSerEquipadoPor(SistemaPersonagem* personagem) const override;
    std::string obterMensagemRequisito() const override;
    void exibirInspecao() const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};