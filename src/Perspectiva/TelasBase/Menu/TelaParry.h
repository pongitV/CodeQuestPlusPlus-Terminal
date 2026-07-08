#pragma once
#include <string>

class TelaParry {
public:
    struct Resultado {
        enum class Modo { Movimento, Digitacao, Desligado };
        Modo modo = Modo::Desligado;
        bool voltou = false;
    };
    static Resultado exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse);
};
