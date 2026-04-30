#include "GerenciadorInimigos.h"
#include "../Racas/Ork.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"
#include "../Inventario/EquipamentoArma.h"
#include "../Inventario/EquipamentoArmadura.h"
#include "../Inimigos/ClasseBaseInimigo.h"
#include "../Inimigos/Goblin.h"
#include "../Inimigos/Slime.h"
#include "../Inimigos/Fada.h"
#include "../Inimigos/OrkExilado.h"
#include "../Inimigos/AbominacaoFloresta.h"

template<typename RacaType, typename ClasseType>
std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigosGenericos(int quantidade)
{
    std::vector<std::unique_ptr<SistemaPersonagem>> horda;
    horda.reserve(quantidade); 
    for (auto i{0}; i < quantidade; ++i) 
    {
        auto raca{std::make_unique<RacaType>()};
        auto nomeRaca{raca->obterNomeRaca()};
        horda.push_back(std::make_unique<SistemaPersonagem>(
            nomeRaca,
            std::move(raca),
            std::make_unique<ClasseType>()
        ));
    }
    return horda;
}

std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigoGoblin(int quantidade)
{
    return criarInimigosGenericos<Goblin, ClasseBaseInimigo>(quantidade);
}

std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigoSlime(int quantidade)
{
    return criarInimigosGenericos<Slime, ClasseBaseInimigo>(quantidade);
}

std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigoFada(int quantidade)
{
    return criarInimigosGenericos<Fada, ClasseBaseInimigo>(quantidade);
}

std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigoOrkExilado(int quantidade)
{
    return criarInimigosGenericos<OrkExilado, ClasseBaseInimigo>(quantidade);
}

std::vector<std::unique_ptr<SistemaPersonagem>> GerenciadorInimigos::criarInimigoAbominacaoFloresta(int quantidade)
{
    return criarInimigosGenericos<AbominacaoFloresta, ClasseBaseInimigo>(quantidade);
}
