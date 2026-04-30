#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

struct SistemaBestiarioEnemyInfo {
    std::string nome;
    std::string mapa;
    std::string habitat;
    std::vector<std::string> aparencia;
    std::string lore;
    std::string fatoCurioso;
    std::vector<std::string> atributosTexto;
    std::vector<std::string> habilidadesAtivas;
    std::string habilidadePassiva;
    std::vector<std::string> drops;
    int dificuldade; // Dificuldade base para ordenar no menu
};

class SistemaBestiario {
public:
    static SistemaBestiario& instancia();

    void inicializarInimigos();

    void registrarPrimeiraVista(const std::string& nomeInimigo);
    void registrarDerrota(const std::string& nomeInimigo);
    void registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade);
    void registrarDrop(const std::string& nomeInimigo, const std::string& drop);

    bool estaDescoberto(const std::string& nomeInimigo) const;
    bool jaDerrotado(const std::string& nomeInimigo) const;
    bool jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const;
    bool jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const;

    const SistemaBestiarioEnemyInfo* obterInfo(const std::string& nomeInimigo) const;
    std::vector<std::string> obterInimigosOrdenadosPorDificuldade() const;

    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);

private:
    SistemaBestiario();
    std::map<std::string, SistemaBestiarioEnemyInfo> inimigosBase;
    
    std::set<std::string> vistos;
    std::set<std::string> derrotados;
    std::map<std::string, std::set<std::string>> habilidadesVistas;
    std::map<std::string, std::set<std::string>> dropsColetados;
};