#pragma once

#include <string>
#include <set>
#include <mutex>
#include <fstream>
#include <vector>

class Diario {
public:
    static Diario& instancia();

    void registrarItem(const std::string& nomeItem);
    void registrarNPC(const std::string& nomeNPC);
    void registrarRaca(const std::string& nomeRaca);
    void registrarClasse(const std::string& nomeClasse);
    void registrarMissaoAceita(const std::string& idMissao);
    void registrarMissaoConcluida(const std::string& idMissao);

    bool itemDescoberto(const std::string& nomeItem) const;
    bool npcDescoberto(const std::string& nomeNPC) const;
    bool racaDescoberta(const std::string& nomeRaca) const;
    bool classeDescoberta(const std::string& nomeClasse) const;
    bool missaoAceita(const std::string& idMissao) const;
    bool missaoConcluida(const std::string& idMissao) const;

    std::vector<std::string> obterItensDescobertos() const;
    std::vector<std::string> obterNPCsDescobertos() const;
    std::vector<std::string> obterRacasDescobertas() const;
    std::vector<std::string> obterClassesDescobertas() const;
    std::vector<std::string> obterMissoesAceitas() const;
    std::vector<std::string> obterMissoesConcluidas() const;

    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);

private:
    Diario();
    
    std::set<std::string> itensDescobertos;
    std::set<std::string> npcsDescobertos;
    std::set<std::string> racasDescobertas;
    std::set<std::string> classesDescobertas;
    std::set<std::string> missoesAceitas;
    std::set<std::string> missoesConcluidas;

    mutable std::mutex mtx;
};
