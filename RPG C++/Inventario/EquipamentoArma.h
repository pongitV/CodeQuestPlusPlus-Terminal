#pragma once

#include "Item.h"
#include <string>
#include <set>
#include <memory>

class EquipamentoArma : public Item 
{
private:
    std::string nome;
    int danoFisico;
    int danoMagico;
    int reqForca;
    int reqDestreza;
    int reqInteligencia;
    int reqSabedoria;
    bool efeitoSangramento;
    bool efeitoLentidao;

public:
    EquipamentoArma(std::string nome, int danoFisico, int danoMagico, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria, int preco = 3);
    
    int obterReqForca() const;
    int obterReqDestreza() const;
    int obterReqInteligencia() const;
    int obterReqSabedoria() const;

    std::string obterNomeItem() const override;
    void alterarNome(const std::string& n) override;
    TipoEquipamento obterTipo() const override;

    int obterDanoFisico() const override;
    int obterDanoMagico() const override;
    
    bool possuiEfeitoSangramento() const override;
    bool possuiEfeitoLentidao() const override;

    std::string obterInfoStatus() const override;

    bool podeSerEquipadoPor(SistemaPersonagem* personagem) const override;
    std::string obterMensagemRequisito() const override;
    void exibirInspecao() const override;

    void aplicarEfeitoSangramento() override;
    void aplicarEfeitoLentidao() override;
    
    void antesDeCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo) override;
    void aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) override;
    int garantirDanoMinimo(int danoFinal) override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};
