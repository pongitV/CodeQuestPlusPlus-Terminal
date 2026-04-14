#include <string>
#include <vector>

#include "GeradorInimigos.h"
#include "../Raças/RacaBase.h"
#include "../Raças/RacaOrk.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"
#include "../Inimigos/ClasseInimigoPadrao.h"
#include "../Inimigos/RacaGoblin.h"
#include "../Inimigos/RacaSlime.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "../Inimigos/RacaAbominacaoFloresta.h"

Personagem* GeradorInimigos::criarInimigoGoblinPadrao()
{
    Personagem* goblin = new Personagem("Goblin", new RacaGoblin(), new ClasseInimigoPadrao());
    Item* adagaGoblin = new Arma("Adaga artesanal de pedra", 6, 0);
    goblin->obterInventario()->adicionarItem(adagaGoblin);
    goblin->equiparItem(adagaGoblin);
    goblin->definirXpRecompensa(40);
    goblin->definirOuroRecompensa(20);
    return goblin;
}

std::vector<std::string> GeradorInimigos::obterArteAsciiDoGoblin()
{
    RacaGoblin temp;
    return temp.obterAparenciaRaca();
}

std::vector<Personagem*> GeradorInimigos::criarHordaDeGoblins(int quantidadeDeGoblins)
{
    std::vector<Personagem*> listaDeGoblinsGerados;
    for (int indiceAtual = 0; indiceAtual < quantidadeDeGoblins; indiceAtual++) listaDeGoblinsGerados.push_back(criarInimigoGoblinPadrao());
    return listaDeGoblinsGerados;
}

Personagem* GeradorInimigos::criarInimigoSlime()
{
    Personagem* slime = new Personagem("Slime", new RacaSlime(), new ClasseInimigoPadrao());
    slime->definirXpRecompensa(35);
    slime->definirOuroRecompensa(15);
    return slime;
}

std::vector<std::string> GeradorInimigos::obterArteAsciiDoSlime()
{
    RacaSlime temp;
    return temp.obterAparenciaRaca();
}

std::vector<Personagem*> GeradorInimigos::criarHordaDeSlimes(int quantidadeDeSlimes)
{
    std::vector<Personagem*> listaDeSlimesGerados;
    for (int indiceAtual = 0; indiceAtual < quantidadeDeSlimes; indiceAtual++) listaDeSlimesGerados.push_back(criarInimigoSlime());
    return listaDeSlimesGerados;
}

Personagem* GeradorInimigos::criarInimigoOrkMiniBoss()
{
    Personagem* ork = new Personagem("Ork [mini-boss]", new RacaOrkExilado(), new ClasseInimigoPadrao());
    ork->definirXpRecompensa(120);
    ork->definirOuroRecompensa(100);
    return ork;
}

Personagem* GeradorInimigos::criarInimigoAbominacaoFloresta()
{
    Personagem* abominacao = new Personagem("Abominacao da Floresta", new RacaAbominacaoFloresta(), new ClasseInimigoPadrao());
    abominacao->definirXpRecompensa(250);
    abominacao->definirOuroRecompensa(200);
    return abominacao;
}