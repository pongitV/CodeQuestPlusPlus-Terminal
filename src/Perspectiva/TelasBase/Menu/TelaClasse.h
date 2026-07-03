#pragma once
#include <string>

class TelaClasse {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
    };
    static Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca);
};
