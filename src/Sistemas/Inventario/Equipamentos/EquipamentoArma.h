#pragma once

#include "../Item.h"
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
    EquipamentoArma(const std::string& nome, int danoFisico, int danoMagico, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria, int preco = 3);
    
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

    bool podeSerEquipadoPor(Personagem* personagem) const override;
    bool isEquipavel() const override { return true; }
    std::vector<std::string> obterDetalhesInspecao(Personagem* personagem = nullptr) const override;

    void aplicarEfeitoSangramento() override;
    void aplicarEfeitoLentidao() override;
    
    void antesDeCausarDano(Personagem* atacante, Personagem* alvo) override;
    void aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) override;
    int garantirDanoMinimo(int danoFinal) override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};

std::unique_ptr<Item> fabricarEquipamentoArma(ItemID id);
