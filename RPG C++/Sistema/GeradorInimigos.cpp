#include <string>
#include <vector>
#include <memory>

#include "GeradorInimigos.h"
#include "../Raças/RacaBase.h"
#include "../Raças/RacaOrk.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inimigos/ClasseInimigoPadrao.h"
#include "../Inimigos/RacaGoblin.h"
#include "../Inimigos/RacaSlime.h"
#include "../Inimigos/RacaFada.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "../Inimigos/RacaAbominacaoFloresta.h"

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoGoblin(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    for (int i = 0; i < quantidade; ++i) {
        auto goblin = std::make_unique<Personagem>("Goblin", std::make_unique<RacaGoblin>(), std::make_unique<ClasseInimigoPadrao>());
        auto adagaGoblin = std::make_unique<Arma>("Adaga artesanal de pedra", 6, 0, 5);
        Item* ptrAdaga = adagaGoblin.get();
        goblin->obterInventario()->adicionarItem(std::move(adagaGoblin));
        goblin->equiparItem(ptrAdaga);
        horda.push_back(std::move(goblin));
    }
    return horda;
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoSlime(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    for (int i = 0; i < quantidade; ++i) horda.push_back(std::make_unique<Personagem>("Slime", std::make_unique<RacaSlime>(), std::make_unique<ClasseInimigoPadrao>()));
    return horda;
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoFada(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    for (int i = 0; i < quantidade; ++i) horda.push_back(std::make_unique<Personagem>("Fada", std::make_unique<RacaFada>(), std::make_unique<ClasseInimigoPadrao>()));
    return horda;
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoOrkExilado(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    for (int i = 0; i < quantidade; ++i) horda.push_back(std::make_unique<Personagem>("Ork Exilado", std::make_unique<RacaOrkExilado>(), std::make_unique<ClasseInimigoPadrao>()));
    return horda;
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoAbominacaoFloresta(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    for (int i = 0; i < quantidade; ++i) horda.push_back(std::make_unique<Personagem>("Abominacao da Floresta", std::make_unique<RacaAbominacaoFloresta>(), std::make_unique<ClasseInimigoPadrao>()));
    return horda;
}
