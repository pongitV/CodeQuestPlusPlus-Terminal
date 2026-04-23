#include "GeradorInimigos.h"
#include "../Raças/RacaOrk.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Armadura.h"
#include "../Inimigos/ClasseInimigoPadrao.h"
#include "../Inimigos/RacaGoblin.h"
#include "../Inimigos/RacaSlime.h"
#include "../Inimigos/RacaFada.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "../Inimigos/RacaAbominacaoFloresta.h"

template<typename RacaType, typename ClasseType>
std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigosGenericos(int quantidade)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    horda.reserve(quantidade); 
    for (auto i{0}; i < quantidade; ++i) 
    {
        auto raca{std::make_unique<RacaType>()};
        auto nomeRaca{raca->obterNomeRaca()};
        horda.push_back(std::make_unique<Personagem>(
            nomeRaca,
            std::move(raca),
            std::make_unique<ClasseType>()
        ));
    }
    return horda;
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoGoblin(int quantidade)
{
    return criarInimigosGenericos<RacaGoblin, ClasseInimigoPadrao>(quantidade);
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoSlime(int quantidade)
{
    return criarInimigosGenericos<RacaSlime, ClasseInimigoPadrao>(quantidade);
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoFada(int quantidade)
{
    return criarInimigosGenericos<RacaFada, ClasseInimigoPadrao>(quantidade);
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoOrkExilado(int quantidade)
{
    return criarInimigosGenericos<RacaOrkExilado, ClasseInimigoPadrao>(quantidade);
}

std::vector<std::unique_ptr<Personagem>> GeradorInimigos::criarInimigoAbominacaoFloresta(int quantidade)
{
    return criarInimigosGenericos<RacaAbominacaoFloresta, ClasseInimigoPadrao>(quantidade);
}
