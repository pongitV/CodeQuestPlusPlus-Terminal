#include "SistemaBestiario.h"
#include <algorithm>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inimigos/Goblin.h"
#include "../Inimigos/Slime.h"
#include "../Inimigos/Fada.h"
#include "../Inimigos/OrkExilado.h"
#include "../Inimigos/AbominacaoFloresta.h"
#include "../Inimigos/Troll.h"
#include "../Inimigos/ClasseBaseInimigo.h"

SistemaBestiario& SistemaBestiario::instancia() {
    static SistemaBestiario inst;
    return inst;
}

SistemaBestiario::SistemaBestiario() {
    inicializarInimigos();
}

namespace {
    template<typename T>
    void registrarNoBestiario(std::map<std::string, SistemaBestiarioEnemyInfo>& inimigosBase) {
        T raca;
        ClasseBaseInimigo classePadrao;
        Atributos attr = raca.obterAtributosRaca();
        InfoBestiario info = raca.obterInfoBestiario();
        
        std::vector<std::string> attrTexto = {
            " > Vida           : " + std::to_string(attr.vida),
            " > Forca          : " + std::to_string(attr.forca),
            " > Destreza       : " + std::to_string(attr.destreza),
            " > Resistencia    : " + std::to_string(attr.resistencia),
            " > Constituicao   : " + std::to_string(attr.constituicao),
            " > Inteligencia   : " + std::to_string(attr.inteligencia),
            " > Sabedoria      : " + std::to_string(attr.sabedoria)
        };

        inimigosBase[raca.obterNomeRaca()] = {
            raca.obterNomeRaca(), info.mapa, info.habitat,
            raca.obterAparenciaRaca(),
            info.lore,
            info.fatoCurioso,
            attrTexto,
            {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
            raca.obterNomeHabilidadeRaca() + " | " + raca.obterDescricaoHabilidadeRaca(),
            info.drops,
            info.dificuldade
        };
    }
}

void SistemaBestiario::inicializarInimigos() {
    registrarNoBestiario<Goblin>(inimigosBase);
    registrarNoBestiario<Slime>(inimigosBase);
    registrarNoBestiario<Fada>(inimigosBase);
    registrarNoBestiario<OrkExilado>(inimigosBase);
    registrarNoBestiario<AbominacaoFloresta>(inimigosBase);
    registrarNoBestiario<Troll>(inimigosBase);
}

void SistemaBestiario::registrarPrimeiraVista(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) vistos.insert(nomeInimigo);
}

void SistemaBestiario::registrarDerrota(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) {
        vistos.insert(nomeInimigo);
        derrotados.insert(nomeInimigo);
    }
}

void SistemaBestiario::registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) habilidadesVistas[nomeInimigo].insert(habilidade);
}

void SistemaBestiario::registrarDrop(const std::string& nomeInimigo, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) dropsColetados[nomeInimigo].insert(drop);
}

bool SistemaBestiario::estaDescoberto(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return vistos.count(nomeInimigo) > 0;
}

bool SistemaBestiario::jaDerrotado(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return derrotados.count(nomeInimigo) > 0;
}

bool SistemaBestiario::jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = habilidadesVistas.find(nomeInimigo);
    if (it != habilidadesVistas.end()) return it->second.count(habilidade) > 0;
    return false;
}

bool SistemaBestiario::jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = dropsColetados.find(nomeInimigo);
    if (it != dropsColetados.end()) return it->second.count(drop) > 0;
    return false;
}

const SistemaBestiarioEnemyInfo* SistemaBestiario::obterInfo(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = inimigosBase.find(nomeInimigo);
    if (it != inimigosBase.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> SistemaBestiario::obterInimigosOrdenadosPorDificuldade() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> nomes;
    nomes.reserve(inimigosBase.size());
    for (const auto& par : inimigosBase) nomes.push_back(par.first);
    
    std::sort(nomes.begin(), nomes.end(), [this](const std::string& a, const std::string& b) {
        return inimigosBase.at(a).dificuldade < inimigosBase.at(b).dificuldade;
    });
    
    return nomes;
}

void SistemaBestiario::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    out << vistos.size() << "\n";
    for (const auto& inimigoVisto : vistos) out << inimigoVisto << "\n";

    out << derrotados.size() << "\n";
    for (const auto& inimigoDerrotado : derrotados) out << inimigoDerrotado << "\n";

    out << habilidadesVistas.size() << "\n";
    for (const auto& [nomeInimigo, conjuntoHabilidades] : habilidadesVistas) {
        out << nomeInimigo << "\n" << conjuntoHabilidades.size() << "\n";
        for (const auto& habilidade : conjuntoHabilidades) out << habilidade << "\n";
    }

    out << dropsColetados.size() << "\n";
    for (const auto& [nomeInimigo, conjuntoDrops] : dropsColetados) {
        out << nomeInimigo << "\n" << conjuntoDrops.size() << "\n";
        for (const auto& drop : conjuntoDrops) out << drop << "\n";
    }
}

void SistemaBestiario::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    vistos.clear();
    derrotados.clear();
    habilidadesVistas.clear();
    dropsColetados.clear();

    size_t size;
    std::string linhaLida;

    if (!(in >> size)) return; // Protecao para saves antigos que nao tinham bestiario
    std::getline(in, linhaLida); // Limpa o \n
    for (size_t i = 0; i < size; ++i) {
        std::getline(in, linhaLida);
        vistos.insert(linhaLida);
    }

    in >> size;
    std::getline(in, linhaLida);
    for (size_t i = 0; i < size; ++i) {
        std::getline(in, linhaLida);
        derrotados.insert(linhaLida);
    }

    in >> size;
    std::getline(in, linhaLida);
    for (size_t i = 0; i < size; ++i) {
        std::string nomeInimigoLido;
        std::getline(in, nomeInimigoLido);
        size_t habSize;
        in >> habSize;
        std::getline(in, linhaLida);
        for (size_t j = 0; j < habSize; ++j) {
            std::getline(in, linhaLida);
            habilidadesVistas[nomeInimigoLido].insert(linhaLida);
        }
    }

    in >> size;
    std::getline(in, linhaLida);
    for (size_t i = 0; i < size; ++i) {
        std::string nomeInimigoLido;
        std::getline(in, nomeInimigoLido);
        size_t dropSize;
        in >> dropSize;
        std::getline(in, linhaLida);
        for (size_t j = 0; j < dropSize; ++j) {
            std::getline(in, linhaLida);
            dropsColetados[nomeInimigoLido].insert(linhaLida);
        }
    }
}