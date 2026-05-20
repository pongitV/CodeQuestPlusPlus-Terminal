#include "Bestiario.h"
#include <algorithm>
#include "../../Entidades/Personagem.h"
#include "../../Entidades/Inimigos/Goblin.h"
#include "../../Entidades/Inimigos/Slime.h"
#include "../../Entidades/Inimigos/Fada.h"
#include "../../Entidades/Inimigos/OrcExilado.h"
#include "../../Entidades/Inimigos/AbominacaoFloresta.h"
#include "../../Entidades/Inimigos/Troll.h"
#include "../../Entidades/Inimigos/Mimico.h"
#include "../../Entidades/Inimigos/Mahoraga.h"
#include "../../Entidades/Inimigos/ClasseBaseInimigo.h"

Bestiario& Bestiario::instancia() {
    static Bestiario inst;
    return inst;
}

Bestiario::Bestiario() {
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

void Bestiario::inicializarInimigos() {
    registrarNoBestiario<Goblin>(inimigosBase);
    registrarNoBestiario<Slime>(inimigosBase);
    registrarNoBestiario<Fada>(inimigosBase);
    registrarNoBestiario<OrkExilado>(inimigosBase);
    registrarNoBestiario<AbominacaoFloresta>(inimigosBase);
    registrarNoBestiario<Troll>(inimigosBase);
    registrarNoBestiario<Mimico>(inimigosBase);
    registrarNoBestiario<Mahoraga>(inimigosBase);
}

void Bestiario::registrarPrimeiraVista(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) vistos.insert(nomeInimigo);
}

void Bestiario::registrarDerrota(const std::string& nomeInimigo) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) {
        vistos.insert(nomeInimigo);
        derrotados.insert(nomeInimigo);
        quantidadeDerrotas[nomeInimigo]++;
    }
}

void Bestiario::registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) habilidadesVistas[nomeInimigo].insert(habilidade);
}

void Bestiario::registrarDrop(const std::string& nomeInimigo, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (inimigosBase.count(nomeInimigo)) dropsColetados[nomeInimigo].insert(drop);
}

bool Bestiario::estaDescoberto(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return vistos.count(nomeInimigo) > 0;
}

bool Bestiario::jaDerrotado(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    return derrotados.count(nomeInimigo) > 0;
}

int Bestiario::obterQuantidadeDerrotas(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = quantidadeDerrotas.find(nomeInimigo);
    if (it != quantidadeDerrotas.end()) {
        return it->second;
    }
    return 0;
}

bool Bestiario::jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = habilidadesVistas.find(nomeInimigo);
    if (it != habilidadesVistas.end()) return it->second.count(habilidade) > 0;
    return false;
}

bool Bestiario::jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = dropsColetados.find(nomeInimigo);
    if (it != dropsColetados.end()) return it->second.count(drop) > 0;
    return false;
}

const SistemaBestiarioEnemyInfo* Bestiario::obterInfo(const std::string& nomeInimigo) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = inimigosBase.find(nomeInimigo);
    if (it != inimigosBase.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiario::obterInimigosOrdenadosPorDificuldade() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> nomes;
    nomes.reserve(inimigosBase.size());
    for (const auto& par : inimigosBase) nomes.push_back(par.first);
    
    std::sort(nomes.begin(), nomes.end(), [this](const std::string& a, const std::string& b) {
        return inimigosBase.at(a).dificuldade < inimigosBase.at(b).dificuldade;
    });
    
    return nomes;
}

void Bestiario::salvar(std::ofstream& out) const {
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

void Bestiario::carregar(std::ifstream& in) {
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






