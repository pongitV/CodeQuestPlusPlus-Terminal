#include "SistemaDiario.h"

SistemaDiario& SistemaDiario::instancia() {
    static SistemaDiario inst;
    return inst;
}

SistemaDiario::SistemaDiario() {}

void SistemaDiario::registrarItem(const std::string& nomeItem) {
    std::lock_guard<std::mutex> lock(mtx);
    itensDescobertos.insert(nomeItem);
}

void SistemaDiario::registrarNPC(const std::string& nomeNPC) {
    std::lock_guard<std::mutex> lock(mtx);
    npcsDescobertos.insert(nomeNPC);
}

void SistemaDiario::registrarRaca(const std::string& nomeRaca) {
    std::lock_guard<std::mutex> lock(mtx);
    racasDescobertas.insert(nomeRaca);
}

void SistemaDiario::registrarClasse(const std::string& nomeClasse) {
    std::lock_guard<std::mutex> lock(mtx);
    classesDescobertas.insert(nomeClasse);
}

bool SistemaDiario::itemDescoberto(const std::string& nomeItem) const {
    std::lock_guard<std::mutex> lock(mtx);
    return itensDescobertos.count(nomeItem) > 0;
}

bool SistemaDiario::npcDescoberto(const std::string& nomeNPC) const {
    std::lock_guard<std::mutex> lock(mtx);
    return npcsDescobertos.count(nomeNPC) > 0;
}

bool SistemaDiario::racaDescoberta(const std::string& nomeRaca) const {
    std::lock_guard<std::mutex> lock(mtx);
    return racasDescobertas.count(nomeRaca) > 0;
}

bool SistemaDiario::classeDescoberta(const std::string& nomeClasse) const {
    std::lock_guard<std::mutex> lock(mtx);
    return classesDescobertas.count(nomeClasse) > 0;
}

std::vector<std::string> SistemaDiario::obterItensDescobertos() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(itensDescobertos.begin(), itensDescobertos.end());
}

std::vector<std::string> SistemaDiario::obterNPCsDescobertos() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(npcsDescobertos.begin(), npcsDescobertos.end());
}

std::vector<std::string> SistemaDiario::obterRacasDescobertas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(racasDescobertas.begin(), racasDescobertas.end());
}

std::vector<std::string> SistemaDiario::obterClassesDescobertas() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(classesDescobertas.begin(), classesDescobertas.end());
}

void SistemaDiario::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto escreverConjunto = [&](const std::set<std::string>& conjunto) {
        out << conjunto.size() << "\n";
        for (const auto& item : conjunto) out << item << "\n";
    };

    escreverConjunto(itensDescobertos);
    escreverConjunto(npcsDescobertos);
    escreverConjunto(racasDescobertas);
    escreverConjunto(classesDescobertas);
}

void SistemaDiario::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    
    itensDescobertos.clear();
    npcsDescobertos.clear();
    racasDescobertas.clear();
    classesDescobertas.clear();

    auto lerConjunto = [&](std::set<std::string>& conjunto) {
        size_t size;
        if (!(in >> size)) return false;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, linha);
            conjunto.insert(linha);
        }
        return true;
    };
    
    lerConjunto(itensDescobertos);
    lerConjunto(npcsDescobertos);
    lerConjunto(racasDescobertas);
    lerConjunto(classesDescobertas);
}
