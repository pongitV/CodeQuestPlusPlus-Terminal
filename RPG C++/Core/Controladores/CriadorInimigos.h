#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../Entidades/Personagem.h"

class CriadorInimigos
{
public:
    static std::vector<std::unique_ptr<Personagem>> criarInimigoGoblin(int quantidade = 3);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoSlime(int quantidade = 3);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoFada(int quantidade = 5);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoOrkExilado(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoAbominacaoFloresta(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoTroll(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoMimico(int quantidade = 1);
    static std::vector<std::unique_ptr<Personagem>> criarInimigoMahoraga(int quantidade = 1);

private:
    template<typename RacaType, typename ClasseType>
    static std::vector<std::unique_ptr<Personagem>> criarInimigosGenericos(int quantidade, int variacaoMaxima = 10);
};
