#include "CriadorInimigos.h"
#include <type_traits>
#include "../../Entidades/Racas/Orc.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Sistemas/Inventario/Item.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArma.h"
#include "../../Sistemas/Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../../Entidades/Inimigos/ClasseBaseInimigo.h"
#include "../../Entidades/Inimigos/Goblin.h"
#include "../../Entidades/Inimigos/Slime.h"
#include "../../Entidades/Inimigos/Fada.h"
#include "../../Entidades/Inimigos/OrcExilado.h"
#include "../../Entidades/Inimigos/AbominacaoFloresta.h"
#include "../../Entidades/Inimigos/Mimico.h"
#include "../../Entidades/Inimigos/Troll.h"
#include "../Utilidades/GeradorAleatorio.h"

template<typename RacaType, typename ClasseType>
std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigosGenericos(int quantidade, int variacaoMaxima)
{
    std::vector<std::unique_ptr<Personagem>> horda;
    horda.reserve(quantidade); 

    for (auto i{0}; i < quantidade; ++i) 
    {
        auto raca{std::make_unique<RacaType>()};
        auto nomeRaca{raca->obterNomeRaca()};
        auto inimigo = std::make_unique<Personagem>(
            nomeRaca,
            std::move(raca),
            std::make_unique<ClasseType>()
        );

        // Aplica uma pequena variacao nos atributos para que cada monstro da horda seja unico
        int variacaoVida = GeradorAleatorio::obterInteiro(-variacaoMaxima, variacaoMaxima);
        inimigo->obterAtributosFinais().vida += (inimigo->obterAtributosFinais().vida * variacaoVida) / 100;
        inimigo->definirVida(inimigo->obterAtributosFinais().vida); // Sincroniza a vida atual com a nova vida maxima
        
        int variacaoForca = GeradorAleatorio::obterInteiro(-variacaoMaxima, variacaoMaxima);
        inimigo->obterAtributosFinais().forca += (inimigo->obterAtributosFinais().forca * variacaoForca) / 100;
        
        int variacaoDestreza = GeradorAleatorio::obterInteiro(-variacaoMaxima, variacaoMaxima);
        inimigo->obterAtributosFinais().destreza += (inimigo->obterAtributosFinais().destreza * variacaoDestreza) / 100;

        horda.push_back(std::move(inimigo));
    }
    return horda;
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoGoblin(int quantidade)
{
    return criarInimigosGenericos<Goblin, ClasseBaseInimigo>(quantidade, 10);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoSlime(int quantidade)
{
    return criarInimigosGenericos<Slime, ClasseBaseInimigo>(quantidade, 10);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoFada(int quantidade)
{
    return criarInimigosGenericos<Fada, ClasseBaseInimigo>(quantidade, 10);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoOrkExilado(int quantidade)
{
    return criarInimigosGenericos<OrkExilado, ClasseBaseInimigo>(quantidade, 5);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoAbominacaoFloresta(int quantidade)
{
    return criarInimigosGenericos<AbominacaoFloresta, ClasseBaseInimigo>(quantidade, 5);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoTroll(int quantidade)
{
    return criarInimigosGenericos<Troll, ClasseBaseInimigo>(quantidade, 5);
}

std::vector<std::unique_ptr<Personagem>> CriadorInimigos::criarInimigoMimico(int quantidade)
{
    return criarInimigosGenericos<Mimico, ClasseBaseInimigo>(quantidade, 10);
}







