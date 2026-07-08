#pragma once

#include <string>
#include <vector>
#include <map>

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

struct GrupoCategorizado {
    std::string nome;
    std::vector<std::string> itens;
};

class TelaDiarioLogic {
public:
    static DadosProgresso obterProgresso();
    static ItensCategorizados categorizarItens(Personagem* jogador);
    static MissoesCategorizadas categorizarMissoes(Personagem* jogador);
    static std::vector<GrupoCategorizado> categorizarBestiario();
    static std::vector<GrupoCategorizado> categorizarNPCs();
    static std::vector<GrupoCategorizado> categorizarRacas(const std::vector<std::string>& racasDescobertas);
    static std::vector<std::string> obterTodasClasses();
    static std::vector<std::string> quebrarTexto(const std::string& texto, int larguraMax);
};
