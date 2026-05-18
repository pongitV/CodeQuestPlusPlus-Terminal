#pragma once

#include <string>
#include <set>
#include <mutex>
#include <fstream>
#include <vector>

class SistemaIndex {
public:
    static SistemaIndex& instancia();

    // --- ARSENAL ---
    void registrarItemVisto(const std::string& nomeItem);
    bool jaViuItem(const std::string& nomeItem) const;
    std::vector<std::string> obterItensVistos() const;

    // --- CLASSES ---
    void registrarClasseVista(const std::string& nomeClasse);
    bool jaViuClasse(const std::string& nomeClasse) const;

    // --- RACAS ---
    void registrarRacaVista(const std::string& nomeRaca);
    bool jaViuRaca(const std::string& nomeRaca) const;

    // --- SERIALIZACAO ---
    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);

private:
    SistemaIndex() = default;

    std::set<std::string> itensVistos;
    std::set<std::string> classesVistas;
    std::set<std::string> racasVistas;

    mutable std::mutex mtx;
};
