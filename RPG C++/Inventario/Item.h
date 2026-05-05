#pragma once

#include <string>
#include <set>
#include <memory>
#include <functional>
#include <iostream>

enum class TipoEquipamento 
{
    NENHUM,     
    ARMA,       
    ESCUDO,     
    ARMADURA,   
    CONSUMIVEL, 
    MISSAO,
    MATERIAL
};

enum class Propriedade 
{
    Nenhuma,
    Magica,
    Penetrante,
    ViolaoBase,
    ViolaoMagico,
    CipoPrisao,
    Melhorado,
    MelhoradoMaterial,
    ConsumivelCura,
    ConsumivelBuff,
    ConsumivelDebuffLentidao,
    ConsumivelDebuffFraqueza,
    TalismaForca,
    TalismaInteligencia,
    TalismaDestreza,
    TalismaSabedoria,
    ConsumivelPoderTroll
};

class SistemaPersonagem;

class Item 
{
protected:
    std::set<Propriedade> propriedades;
    int precoVenda;
    std::function<void(SistemaPersonagem*, SistemaPersonagem*)> acaoUsar;
public:
    Item(int preco = 3) : precoVenda(preco) {}
    virtual ~Item() = default;
    virtual std::string obterNomeItem() const = 0;
    virtual TipoEquipamento obterTipo() const { return TipoEquipamento::NENHUM; }
    virtual int obterDanoFisico() const { return 0; }
    virtual int obterDanoMagico() const { return 0; }
    virtual double obterReducaoPercentual() const { return 0.0; }
    virtual int obterReducaoFixa() const { return 0; }
    virtual int obterReducaoDanoFixaEscudo() const { return 0; }
    virtual int obterDurabilidadeAtualEscudo() const { return 0; }
    
    virtual bool podeSerEquipadoPor(SistemaPersonagem* personagem) const { return true; }
    virtual std::string obterMensagemRequisito() const { return ""; }
    
    virtual void exibirInspecao() const {
        std::cout << "\n === " << obterNomeItem() << " ===\n\n";
        TipoEquipamento tipo = obterTipo();
        std::cout << " > Tipo: " << (tipo == TipoEquipamento::CONSUMIVEL ? "Consumivel" : 
                                  tipo == TipoEquipamento::MATERIAL ? "Material" :
                                  tipo == TipoEquipamento::MISSAO ? "Item de Missao" : "Desconhecido") << "\n";
        std::cout << " > Preco de Venda: " << precoVenda << "G\n";
        std::cout << " > Descricao: " << (tipo == TipoEquipamento::MATERIAL ? "Material usado em forjas ou encantamentos." :
                                        tipo == TipoEquipamento::CONSUMIVEL ? "Pode ser consumido para aplicar efeitos." : 
                                        "Pode ser importante para o seu progresso.") << "\n";
    }

    virtual void alterarNome(const std::string& n) {}
    virtual bool possuiEfeitoSangramento() const { return false; }
    virtual bool possuiEfeitoLentidao() const { return false; }
    virtual void aplicarEfeitoSangramento() {}
    virtual void aplicarEfeitoLentidao() {}

    virtual void reduzirDurabilidade(int qtd) {}
    virtual void aumentarDurabilidade(int qtd) {}
    
    virtual void antesDeCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo) {}
    virtual void aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {}
    virtual int garantirDanoMinimo(int danoFinal) { return danoFinal; }

    virtual int obterPrecoVenda() const { return precoVenda; }
    virtual std::string obterInfoStatus() const { return ""; } // Vazio por padrão para itens sem status extra
    
    virtual void usar(SistemaPersonagem* usuario, SistemaPersonagem* alvo) {
        if (usuario == nullptr || alvo == nullptr) return;
        if (acaoUsar) acaoUsar(usuario, alvo);
    }
    virtual void definirAcaoUsar(std::function<void(SistemaPersonagem*, SistemaPersonagem*)> acao) { acaoUsar = acao; }

    virtual bool temPropriedade(Propriedade prop) const { return propriedades.find(prop) != propriedades.end(); }
    virtual void adicionarPropriedade(Propriedade prop) { propriedades.insert(prop); }
    virtual void removerPropriedade(Propriedade prop) { propriedades.erase(prop); }
    virtual std::set<Propriedade> obterPropriedades() const { return propriedades; }
    virtual std::unique_ptr<Item> gerarCopiaMelhorada() const { return nullptr; }
};