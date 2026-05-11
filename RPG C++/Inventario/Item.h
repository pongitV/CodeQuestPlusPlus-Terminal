#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include "../Utilidades/Aparencia.h"

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

enum class ItemID {
    Nenhum = 0,
    // Armas
    AdagaPedra, ArcoMadeira, CajadoCristal, VarinhaCorroida, ViolaoEncantado, EspadaFerro, MachadoGuerra, GosmaAcidaArma, TroncoAmarrotado, EspadaCavaleiro,
    
    // Escudos
    EscudoMetal, BarreiraMagica, CapaMagica, BracedeirasPrata,
    
    // Armaduras
    ArmaduraMalha, ArmaduraCouro, Tunica, TrajeNobre, ArmaduraTrapos, ArmaduraCavaleiro, ArmaduraBau,
    
    // Consumiveis
    PocaoCura30, PocaoFuria, ElixirArcano, FrascoGosma, FrascoFraqueza, OrgaoRegenerador,
    TalismaUrso, TalismaCorvo, TalismaLeopardo, TalismaCoruja,
    
    // Materiais
    GosmaAcida, DenteGoblin, NucleoPegajoso, PoMagico, MadeiraEnfeiticada, CoracaoFloresta, PedraUpgrade, ConviteReal,
    
    // Missoes
    DispositivoLinguagem
};

class SistemaPersonagem;

class Item 
{
protected:
    std::vector<Propriedade> propriedades;
    int precoVenda;
    std::function<void(SistemaPersonagem*, SistemaPersonagem*)> acaoUsar;
    std::function<bool(Item*, SistemaPersonagem*, bool*)> acaoInventario;
    std::vector<std::string> descricaoInspecao;
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
    
    virtual void definirDescricaoInspecao(const std::vector<std::string>& desc) { descricaoInspecao = desc; }
    virtual void definirDescricaoInspecao(const std::string& desc) { descricaoInspecao = {desc}; }

    virtual bool podeSerEquipadoPor(SistemaPersonagem* personagem) const { return true; }
    virtual bool isEquipavel() const { return false; }
    virtual std::string obterMensagemRequisito() const { return "\n[SISTEMA]: Atributos insuficientes para equipar " + obterNomeItem() + "!\n"; }
    
    virtual std::vector<std::string> obterDetalhesInspecao() const {
        std::vector<std::string> detalhes;
        detalhes.push_back(" > Tipo: Desconhecido");
        detalhes.push_back(" > Descricao: Nenhuma informacao disponivel.");
        return detalhes;
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
    virtual int garantirDanoMinimo(int danoFinal) { return std::max(danoFinal, 1); }

    virtual int obterPrecoVenda() const { return precoVenda; }
    virtual std::string obterInfoStatus() const { return ""; } // Vazio por padrão para itens sem status extra
    
    virtual void usar(SistemaPersonagem* usuario, SistemaPersonagem* alvo) {
        if (usuario == nullptr || alvo == nullptr) return;
        if (acaoUsar) acaoUsar(usuario, alvo);
    }
    virtual void definirAcaoUsar(std::function<void(SistemaPersonagem*, SistemaPersonagem*)> acao) { acaoUsar = acao; }
    
    virtual void definirAcaoInventario(std::function<bool(Item*, SistemaPersonagem*, bool*)> acao) { acaoInventario = acao; }
    virtual bool usarDoInventario(SistemaPersonagem* usuario, bool* turnoFoiConsumido) {
        if (acaoInventario) return acaoInventario(this, usuario, turnoFoiConsumido);
        return false;
    }

    virtual bool temPropriedade(Propriedade prop) const { 
        return std::find(propriedades.begin(), propriedades.end(), prop) != propriedades.end(); 
    }
    virtual void adicionarPropriedade(Propriedade prop) { 
        if (!temPropriedade(prop)) propriedades.push_back(prop); 
    }
    virtual void removerPropriedade(Propriedade prop) { 
        auto it = std::find(propriedades.begin(), propriedades.end(), prop);
        if (it != propriedades.end()) propriedades.erase(it); 
    }
    virtual const std::vector<Propriedade>& obterPropriedades() const { return propriedades; }
    virtual std::unique_ptr<Item> gerarCopiaMelhorada() const { return nullptr; }
};