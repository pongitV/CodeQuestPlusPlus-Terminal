#pragma once
#include <string>
#include "../../../TelasBase/Menu/TelaClasse.h"

class TelaClasseIDE {
public:
    static TelaClasse::Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca);
};
