// Inventario/Itens.h
#pragma once
#include <string>

enum class Raridade { COMUM, RARO, MITICO };
enum class TipoEquipamento { NENHUM, ARMA, ESCUDO, ARMADURA, CONSUMIVEL, MISSAO };

class Item 
{
public:
    virtual ~Item() {}
    virtual std::string obterNomeItem() const = 0;
    virtual Raridade obterRaridade() const = 0;
    virtual TipoEquipamento obterTipo() const { return TipoEquipamento::NENHUM; }
    virtual int obterBonusDano() const { return 0; }
    virtual double obterReducaoPercentual() const { return 0.0; }
    virtual int obterReducaoFixa() const { return 0; }

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

class Arma : public Item {
    std::string nome; int bonus;
public:
    Arma(std::string n, int b) : nome(n), bonus(b) {}
    std::string obterNomeItem() const override { return nome; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::ARMA; }
    int obterBonusDano() const override { return bonus; }
};

class Escudo : public Item {
    std::string nome; double red;
public:
    Escudo(std::string n, double r) : nome(n), red(r) {}
    std::string obterNomeItem() const override { return nome; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::ESCUDO; }
    double obterReducaoPercentual() const override { return red; }
};

class Armadura : public Item {
    std::string nome; int red;
public:
    Armadura(std::string n, int r) : nome(n), red(r) {}
    std::string obterNomeItem() const override { return nome; }
    Raridade obterRaridade() const override { return Raridade::COMUM; }
    TipoEquipamento obterTipo() const override { return TipoEquipamento::ARMADURA; }
    int obterReducaoFixa() const override { return red; }
};