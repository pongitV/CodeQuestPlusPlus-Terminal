#include <string>
#include <vector>
#include <memory>

#include "../Sistema/Personagem.h"

#pragma once

class GeradorInimigos
{
public:
    static std::vector<std::unique_ptr<Personagem>> criarInimigoGoblin(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoSlime(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoFada(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoOrkExilado(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoAbominacaoFloresta(int quantidade = 1);
};
