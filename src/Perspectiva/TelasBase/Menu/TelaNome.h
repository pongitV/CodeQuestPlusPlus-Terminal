#pragma once
#include <string>

class TelaNome {
public:
    struct Resultado {
        std::string nome;
        bool voltou = false;
    };
    static Resultado exibir();
};
