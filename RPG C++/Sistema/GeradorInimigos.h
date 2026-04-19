#pragma once
#include <string>
#include <vector>
#include <memory>

#include "../Sistema/Personagem.h"

class GeradorInimigos
{
public:
    static std::vector<std::unique_ptr<Personagem>> criarInimigoGoblin(int quantidade = 3);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoSlime(int quantidade = 3);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoFada(int quantidade = 5);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoOrkExilado(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoAbominacaoFloresta(int quantidade = 1);

private:
    template<typename RacaType, typename ClasseType>
    static std::vector<std::unique_ptr<Personagem>> criarInimigosGenericos(int quantidade);
};
