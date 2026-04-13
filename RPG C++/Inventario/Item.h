#include <string>

#pragma once

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

    virtual ~Item() {}
    virtual std::string obterNomeItem() const = 0;
    virtual Raridade obterRaridade() const = 0;
    virtual TipoEquipamento obterTipo() const { return TipoEquipamento::NENHUM; }
    virtual int obterDanoFisico() const { return 0; }
    virtual int obterDanoMagico() const { return 0; }
    virtual double obterReducaoPercentual() const { return 0.0; }
    virtual int obterReducaoFixa() const { return 0; }
    virtual int obterReducaoDanoFixaEscudo() const { return 0; }
    virtual int obterDurabilidadeAtualEscudo() const { return 0; }
    
    virtual void alterarNome(std::string n) {}
    virtual bool possuiEfeitoSangramento() const { return false; }
    virtual bool possuiEfeitoLentidao() const { return false; }
    virtual void aplicarEfeitoSangramento() {}
    virtual void aplicarEfeitoLentidao() {}

    virtual void reduzirDurabilidade(int qtd) {}
    virtual void aumentarDurabilidade(int qtd) {}
    std::string raridadeParaString() const 
    {
        if (obterRaridade() == Raridade::MITICO) return "Mitico";
        if (obterRaridade() == Raridade::RARO) return "Raro";
        return "Comum";
    }
};

class PocaoCura : public Item 
{
public:
    std::string obterNomeItem() const override { return "Pocao de Cura (30%)"; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::CONSUMIVEL; }
};

class Arma : public Item 
{
private:
    std::string nome;
    int danoFisico;
    int danoMagico;
    bool efeitoSangramento = false;
    bool efeitoLentidao = false;

public:
    Arma(std::string n, int dFisico, int dMagico) : nome(n), danoFisico(dFisico), danoMagico(dMagico) {}
    std::string obterNomeItem() const override { return nome; }
    void alterarNome(std::string n) override { nome = n; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::ARMA; }
    int obterDanoFisico() const override { return danoFisico; }
    int obterDanoMagico() const override { return danoMagico; }
    
    bool possuiEfeitoSangramento() const override { return efeitoSangramento; }
    bool possuiEfeitoLentidao() const override { return efeitoLentidao; }
    void aplicarEfeitoSangramento() override { efeitoSangramento = true; }
    void aplicarEfeitoLentidao() override { efeitoLentidao = true; }
};

class Escudo : public Item 
{
private:
    std::string nome;        
    int reducaoFixa;         
    int durabilidade;   

public:
    Escudo(std::string n, int r, int d) : nome(n), reducaoFixa(r), durabilidade(d) {}
    std::string obterNomeItem() const override { return nome; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::ESCUDO; }

    int obterReducaoDanoFixaEscudo() const override { return reducaoFixa; }

    int obterDurabilidadeAtualEscudo() const override { return durabilidade; }

    void reduzirDurabilidade(int qtd) override { durabilidade -= qtd; }

    void aumentarDurabilidade(int qtd) override { durabilidade += qtd; }
};

class Armadura : public Item 
{
private:
    std::string nome; 
    int red;          

public:

    Armadura(std::string n, int r) : nome(n), red(r) {}

    std::string obterNomeItem() const override { return nome; }

    Raridade obterRaridade() const override { return Raridade::COMUM; }

    TipoEquipamento obterTipo() const override { return TipoEquipamento::ARMADURA; }

    int obterReducaoFixa() const override { return red; }
};

class Material : public Item 
{
private:
    std::string nome;

public:
    Material(std::string n) : nome(n) {}
    std::string obterNomeItem() const override { return nome; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::MATERIAL; }
};