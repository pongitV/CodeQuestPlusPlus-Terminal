#pragma once
#include <string>
#include <vector>
#include <memory>

#include "../Sistemas/SistemaPersonagem.h"

class GerenciadorInimigos
{
public:
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigoGoblin(int quantidade = 3);
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigoSlime(int quantidade = 3);
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigoFada(int quantidade = 5);
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigoOrkExilado(int quantidade = 1);
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigoAbominacaoFloresta(int quantidade = 1);

private:
    template<typename RacaType, typename ClasseType>
    static std::vector<std::unique_ptr<SistemaPersonagem>> criarInimigosGenericos(int quantidade);
};
