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

    bool itemDescoberto(const std::string& nomeItem) const;
    bool npcDescoberto(const std::string& nomeNPC) const;
    bool racaDescoberta(const std::string& nomeRaca) const;
    bool classeDescoberta(const std::string& nomeClasse) const;

    std::vector<std::string> obterItensDescobertos() const;
    std::vector<std::string> obterNPCsDescobertos() const;
    std::vector<std::string> obterRacasDescobertas() const;
    std::vector<std::string> obterClassesDescobertas() const;

    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);

private:
    Diario();
    
    std::set<std::string> itensDescobertos;
    std::set<std::string> npcsDescobertos;
    std::set<std::string> racasDescobertas;
    std::set<std::string> classesDescobertas;

    mutable std::mutex mtx;
};
