#include "SistemaIndex.h"

SistemaIndex& SistemaIndex::instancia() {
    static SistemaIndex inst;
    return inst;
}

void SistemaIndex::registrarItemVisto(const std::string& nomeItem) {
    std::lock_guard<std::mutex> lock(mtx);
    itensVistos.insert(nomeItem);
}

bool SistemaIndex::jaViuItem(const std::string& nomeItem) const {
    std::lock_guard<std::mutex> lock(mtx);
    return itensVistos.count(nomeItem) > 0;
}

std::vector<std::string> SistemaIndex::obterItensVistos() const {
    std::lock_guard<std::mutex> lock(mtx);
    return std::vector<std::string>(itensVistos.begin(), itensVistos.end());
}

void SistemaIndex::registrarClasseVista(const std::string& nomeClasse) {
    std::lock_guard<std::mutex> lock(mtx);
    classesVistas.insert(nomeClasse);
}

bool SistemaIndex::jaViuClasse(const std::string& nomeClasse) const {
    std::lock_guard<std::mutex> lock(mtx);
    return classesVistas.count(nomeClasse) > 0;
}

void SistemaIndex::registrarRacaVista(const std::string& nomeRaca) {
    std::lock_guard<std::mutex> lock(mtx);
    racasVistas.insert(nomeRaca);
}

bool SistemaIndex::jaViuRaca(const std::string& nomeRaca) const {
    std::lock_guard<std::mutex> lock(mtx);
    return racasVistas.count(nomeRaca) > 0;
}

void SistemaIndex::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto escreverConjunto = [&](const std::set<std::string>& conjunto) {
        out << conjunto.size() << "\n";
        for (const auto& item : conjunto) out << item << "\n";
    };

    escreverConjunto(itensVistos);
    escreverConjunto(classesVistas);
    escreverConjunto(racasVistas);
}

void SistemaIndex::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    itensVistos.clear();
    classesVistas.clear();
    racasVistas.clear();

    auto lerConjunto = [&](std::set<std::string>& conjunto) {
        size_t size;
        if (!(in >> size)) return false;
        std::string linha; std::getline(in, linha); // Consome o \n
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, linha);
            conjunto.insert(linha);
        }
        return true;
    };
    
    if (!lerConjunto(itensVistos)) return; // Se for um save velho sem Index, ele cancela a leitura aqui de forma segura
    lerConjunto(classesVistas);
    lerConjunto(racasVistas);
}
