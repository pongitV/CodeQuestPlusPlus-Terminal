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
    ArmaduraMalha, ArmaduraCouro, Tunica, TrajeNobre, ArmaduraTrapos, ArmaduraCavaleiro,
    
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
    virtual std::string obterMensagemRequisito() const { return "\n[SISTEMA]: Atributos insuficientes para equipar " + obterNomeItem() + "!\n"; }
    
    virtual std::vector<std::string> obterDetalhesInspecao() const {
        TipoEquipamento tipo = obterTipo();
        std::vector<std::string> detalhes;

        if (tipo == TipoEquipamento::CONSUMIVEL) {
            detalhes.push_back(" > Tipo: Consumivel");
            std::string efeito = "Pode ser consumido para aplicar efeitos.";
            if (temPropriedade(Propriedade::ConsumivelCura)) efeito = "Restaura 30% da sua Vida Maxima.";
            else if (temPropriedade(Propriedade::ConsumivelBuff)) efeito = "Aumenta seus atributos em 1.5x por 2 turnos.";
            else if (temPropriedade(Propriedade::ConsumivelDebuffLentidao)) efeito = "Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).";
            else if (temPropriedade(Propriedade::ConsumivelDebuffFraqueza)) efeito = "Aplica Fraqueza no alvo por 3 turnos (-25% Forca).";
            else if (temPropriedade(Propriedade::ConsumivelPoderTroll)) efeito = "Concede a regeneracao do Troll permanentemente (cura 100% HP apos batalhas).";
            else if (temPropriedade(Propriedade::TalismaForca)) efeito = "Concede +5 Forca e -5 Inteligencia permanentemente.";
            else if (temPropriedade(Propriedade::TalismaInteligencia)) efeito = "Concede +5 Inteligencia e -5 Forca permanentemente.";
            else if (temPropriedade(Propriedade::TalismaDestreza)) efeito = "Concede +5 Destreza e -5 Sabedoria permanentemente.";
            else if (temPropriedade(Propriedade::TalismaSabedoria)) efeito = "Concede +5 Sabedoria e -5 Destreza permanentemente.";
            detalhes.push_back(" > Efeitos: " + efeito);
        } else if (tipo == TipoEquipamento::MISSAO) {
            detalhes.push_back(" > Tipo: Item de Missao");
            if (obterNomeItem() == "Dispositivo de teclas de linguagem desconhecida") {
                detalhes.push_back(" > Lore: Um estranho artefato de plastico com teclas.");
                detalhes.push_back("   Nao parece pertencer a este mundo, mas emana");
                detalhes.push_back("   uma energia peculiar...");
            } else {
                detalhes.push_back(" > Lore: Um item misterioso e importante para sua jornada.");
            }
        } else {
            detalhes.push_back(" > Tipo: " + std::string(tipo == TipoEquipamento::MATERIAL ? "Material" : "Desconhecido"));
            detalhes.push_back(" > Descricao: " + std::string(tipo == TipoEquipamento::MATERIAL ? "Material usado em forjas ou encantamentos." : "Pode ser importante para o seu progresso."));
        }
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