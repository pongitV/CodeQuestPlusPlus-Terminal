#include <string>
#include <vector>

#include "../Sistema/Personagem.h"

#pragma once

class GeradorInimigos
{
public:
    static Personagem* criarInimigoGoblinPadrao();
    static std::vector<std::string> obterArteAsciiDoGoblin();
    static std::vector<Personagem*> criarHordaDeGoblins(int quantidadeDeGoblins);

    static Personagem* criarInimigoSlime();
    static std::vector<std::string> obterArteAsciiDoSlime();
    static std::vector<Personagem*> criarHordaDeSlimes(int quantidadeDeSlimes);

    static Personagem* criarInimigoOrkExilado();
    static Personagem* criarInimigoAbominacaoFloresta();
};
