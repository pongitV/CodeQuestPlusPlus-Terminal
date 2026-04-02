#include <string>
#include <vector>

#include "GeradorInimigos.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

class ClasseInimigoPadrao : public ClasseBase 
{
public:
    std::string obterNomeClasse() const override { return "Monstro"; }
    Atributos obterAtributosClasse() const override { return { 0, 0, 0, 0, 0, 0, 0 }; }
    void usarHabilidadeClasse(Personagem* u, Personagem* a) override {}
    std::vector<Item*> gerarKitInicial() const override { return std::vector<Item*>(); }
};

class RacaGoblin : public RacaBase
{
public:
    std::string obterNomeRaca() const override { return "Goblin"; }
    Atributos obterAtributosRaca() const override { return { -50, 0, 0, 0, 0, 0, 0 }; }
    void usarHabilidadeRaca(Personagem* u, Personagem* a) override {}

    // Implementação obrigatória para a classe não ser abstrata
    std::vector<std::string> obterAparencia() const override 
    {
        return 
        {
            "   /\\__/\\   ", 
            "  ( o  o )  ", 
            "  (  ww  )  ", 
            "   /|__|\\)=->", 
            "  /|====|\\  ", 
            "  _||  ||_  ", 
            " (___)(___) "  
        };
    }
};

Personagem* GeradorInimigos::gerarGoblin()
{
    // Agora o compilador permite o 'new'
    return new Personagem("Goblin", new RacaGoblin(), new ClasseInimigoPadrao());
}

std::vector<std::string> GeradorInimigos::obterGoblinASCII()
{
    RacaGoblin temp;
    return temp.obterAparencia(); // Centralizado na raça
}

std::vector<Personagem*> GeradorInimigos::gerarHordaGoblins(int quantidade)
{
    std::vector<Personagem*> horda;
    for (int i = 0; i < quantidade; i++) horda.push_back(gerarGoblin());
    return horda;
}