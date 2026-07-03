#pragma once

#include <string>
#include <vector>

class Personagem;

struct DadosProgresso {
    std::vector<std::string> linhasNPC;
    std::vector<std::string> linhasMonstro;
    std::vector<std::string> linhasItem;
    int totalConquistas = 0;
};

struct ItensCategorizados {
    std::vector<std::string> armas;
    std::vector<std::string> escudos;
    std::vector<std::string> armaduras;
    std::vector<std::string> consumiveis;
    std::vector<std::string> materiais;
    std::vector<std::string> missoes;
    std::vector<std::string> outros;
};

struct MissoesCategorizadas {
    std::vector<std::string> emAndamento;
    std::vector<std::string> prontas;
    std::vector<std::string> completas;
};

class TelaDiarioLogic {
public:
    static DadosProgresso obterProgresso();
    static ItensCategorizados categorizarItens(Personagem* jogador);
    static MissoesCategorizadas categorizarMissoes(Personagem* jogador);
};
