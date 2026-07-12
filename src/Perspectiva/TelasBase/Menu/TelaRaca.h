#pragma once
#include <string>
#include "../../../Entidades/Racas/RacaBase.h"

class TelaRaca {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
        std::string nome;
        TipoRaca racaSelecionada = TipoRaca::Nenhum;
    };
    static Resultado exibir(const std::string& nomeJogador);
};
