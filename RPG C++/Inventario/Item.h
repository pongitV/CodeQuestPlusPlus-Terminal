#pragma once

#include <string>

enum class Raridade 
{
    COMUM,   
    RARO,    
    MITICO   
};

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

class Item 
{
public:

    virtual ~Item() = default;
    virtual std::string obterNomeItem() const = 0;
    virtual Raridade obterRaridade() const = 0;
    virtual TipoEquipamento obterTipo() const { return TipoEquipamento::NENHUM; }
    virtual int obterDanoFisico() const { return 0; }
    virtual int obterDanoMagico() const { return 0; }
    virtual double obterReducaoPercentual() const { return 0.0; }
    virtual int obterReducaoFixa() const { return 0; }
    virtual int obterReducaoDanoFixaEscudo() const { return 0; }
    virtual int obterDurabilidadeAtualEscudo() const { return 0; }
    
    virtual void alterarNome(const std::string& n) {}
    virtual bool possuiEfeitoSangramento() const { return false; }
    virtual bool possuiEfeitoLentidao() const { return false; }
    virtual void aplicarEfeitoSangramento() {}
    virtual void aplicarEfeitoLentidao() {}

    virtual void reduzirDurabilidade(int qtd) {}
    virtual void aumentarDurabilidade(int qtd) {}
    
    virtual int obterPrecoVenda() const { return 3; } // Preço padrão para a maioria dos itens
    virtual std::string obterInfoStatus() const { return ""; } // Vazio por padrão para itens sem status extra

    std::string raridadeParaString() const 
    {
        if (obterRaridade() == Raridade::MITICO) return "Mitico";
        if (obterRaridade() == Raridade::RARO) return "Raro";
        return "Comum";
    }
};