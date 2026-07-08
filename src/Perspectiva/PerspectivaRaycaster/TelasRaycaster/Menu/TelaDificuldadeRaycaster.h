#pragma once
#include <string>
#include "../../../TelasBase/Menu/TelaDificuldade.h"

class TelaDificuldadeRaycaster {
public:
    static TelaDificuldade::Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
