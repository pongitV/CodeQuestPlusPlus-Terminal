#include "SistemaBestiario.h"
#include <algorithm>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inimigos/Mapa1Vila/Goblin.h"
#include "../Inimigos/Mapa2Floresta/Slime.h"
#include "../Inimigos/Mapa2Floresta/Fada.h"
#include "../Inimigos/Mapa1Vila/OrkExilado.h"
#include "../Inimigos/Mapa2Floresta/AbominacaoFloresta.h"
#include "../Inimigos/Mapa3Reino/Troll.h"
#include "../Inimigos/Mapa2Floresta/Mimico.h"
#include "../Inimigos/Mapa2Floresta/Mahoraga.h"
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
    registrarNoBestiario<Mimico>(inimigosBase);
    registrarNoBestiario<Mahoraga>(inimigosBase);
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
        quantidadeDerrotas[nomeInimigo]++;
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

int SistemaBestiario::obterQuantidadeDerrotas(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = quantidadeDerrotas.find(nomeInimigo);
    if (it != quantidadeDerrotas.end()) {
        return it->second;
    }
    return 0;
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
    
    auto escreverConjunto = [&](const auto& conjunto) {
        out << conjunto.size() << "\n";
        for (const auto& item : conjunto) out << item << "\n";
    };

    escreverConjunto(vistos);
    escreverConjunto(derrotados);

    out << quantidadeDerrotas.size() << "\n";
    for (const auto& [nome, qtd] : quantidadeDerrotas) out << nome << "\n" << qtd << "\n";

    auto escreverMapaConjuntos = [&](const auto& mapa) {
        out << mapa.size() << "\n";
        for (const auto& [nome, conjunto] : mapa) {
            out << nome << "\n";
            escreverConjunto(conjunto);
        }
    };

    escreverMapaConjuntos(habilidadesVistas);
    escreverMapaConjuntos(dropsColetados);
}

void SistemaBestiario::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    vistos.clear();
    derrotados.clear();
    quantidadeDerrotas.clear();
    habilidadesVistas.clear();
    dropsColetados.clear();

    auto lerConjunto = [&](auto& conjunto) {
        size_t size;
        if (!(in >> size)) return false;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, linha);
            conjunto.insert(linha);
        }
        return true;
    };
    
    if (!lerConjunto(vistos)) return; // Failsafe para saves antigos
    lerConjunto(derrotados);
    
    size_t qtdDerrotasSize;
    if (in >> qtdDerrotasSize) {
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < qtdDerrotasSize; ++i) {
            std::string nome; std::getline(in, nome);
            int qtd; in >> qtd; std::getline(in, linha);
            quantidadeDerrotas[nome] = qtd;
        }
    }

    auto lerMapa = [&](auto& mapa) {
        size_t size;
        if (!(in >> size)) return;
        std::string linha; std::getline(in, linha);
        for (size_t i = 0; i < size; ++i) {
            std::string chave; std::getline(in, chave);
            lerConjunto(mapa[chave]);
        }
    };

    lerMapa(habilidadesVistas);
    lerMapa(dropsColetados);
}