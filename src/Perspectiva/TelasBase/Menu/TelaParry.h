#pragma once
#include <string>

class TelaParry {
public:
    struct Resultado {
        bool parryAtivado = false;
        bool fazerTutorial = false;
        bool voltou = false;
    };
    static Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
