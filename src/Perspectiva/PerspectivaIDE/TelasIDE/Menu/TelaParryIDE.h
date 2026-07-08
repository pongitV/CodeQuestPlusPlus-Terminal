#pragma once
#include <string>
#include "../../../TelasBase/Menu/TelaParry.h"

class TelaParryIDE {
public:
    static TelaParry::Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};