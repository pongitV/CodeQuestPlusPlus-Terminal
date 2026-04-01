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
    
    std::vector<Item*> gerarKitInicial() const override 
    {
        return std::vector<Item*>(); 
    }
};

class RacaGoblin : public RacaBase
{
public:
    std::string obterNomeRaca() const override { return "Goblin"; }
    
    Atributos obterAtributosRaca() const override 
    { 
        return { -50, 0, 0, 0, 0, 0, 0 };
    }
    
    void usarHabilidadeRaca(Personagem* usuario, Personagem* alvo) override 
    {
    }
};

Personagem* GeradorInimigos::gerarGoblin()
{
    Personagem* g = new Personagem("Goblin", new RacaGoblin(), new ClasseInimigoPadrao());
    g->definirOuroRecompensa(15);
    return g;
}

std::vector<Personagem*> GeradorInimigos::gerarHordaGoblins(int quantidade)
{
    std::vector<Personagem*> horda;
    for (int i = 0; i < quantidade; i++)
    {
        horda.push_back(gerarGoblin());
    }
    return horda;
}
