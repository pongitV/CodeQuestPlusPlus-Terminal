#pragma once
#include <string>

class TelaDificuldade {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
    };
    static Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
