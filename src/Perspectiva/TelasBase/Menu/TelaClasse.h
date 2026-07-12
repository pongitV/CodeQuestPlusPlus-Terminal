#pragma once
#include <string>
#include "../../../Entidades/Classes/ClasseBase.h"

class TelaClasse {
public:
    struct Resultado {
        int indice = 0;
        bool voltou = false;
        std::string nome;
        TipoClasse classeSelecionada = TipoClasse::Nenhum;
    };
    static Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca);
};
