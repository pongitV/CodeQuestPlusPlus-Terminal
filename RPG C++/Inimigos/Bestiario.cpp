#include "Bestiario.h"
#include <algorithm>
#include "../Sistema/Personagem.h"
#include "../Inimigos/RacaGoblin.h"
#include "../Inimigos/RacaSlime.h"
#include "../Inimigos/RacaFada.h"
#include "../Inimigos/RacaOrkExilado.h"
#include "../Inimigos/RacaAbominacaoFloresta.h"
#include "../Inimigos/ClasseInimigoPadrao.h"

Bestiario& Bestiario::instancia() {
    static Bestiario inst;
    return inst;
}

Bestiario::Bestiario() {
    inicializarInimigos();
}

void Bestiario::inicializarInimigos() {
    ClasseInimigoPadrao classePadrao;

    RacaGoblin racaGoblin;
    Atributos attrGoblin = racaGoblin.obterAtributosRaca();
    inimigosBase[racaGoblin.obterNomeRaca()] = {
        racaGoblin.obterNomeRaca(), "Vila Inicial", "Arredores",
        racaGoblin.obterAparenciaRaca(),
        "Pequenas criaturas verdes e astutas que costumam viver em bandos perto de vilarejos.",
        "Goblins adoram itens brilhantes e guardam dentes como trofeus.",
        {
            " > Vida           : " + std::to_string(attrGoblin.vida),
            " > Forca          : " + std::to_string(attrGoblin.forca),
            " > Destreza       : " + std::to_string(attrGoblin.destreza),
            " > Resistencia    : " + std::to_string(attrGoblin.resistencia),
            " > Constituicao   : " + std::to_string(attrGoblin.constituicao),
            " > Inteligencia   : " + std::to_string(attrGoblin.inteligencia),
            " > Sabedoria      : " + std::to_string(attrGoblin.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaGoblin.obterNomeHabilidadeRaca() + " | " + racaGoblin.obterDescricaoHabilidadeRaca(),
        {"Dente de goblin", "Adaga artesanal de pedra", "Ouro"},
        1
    };

    RacaSlime racaSlime;
    Atributos attrSlime = racaSlime.obterAtributosRaca();
    inimigosBase[racaSlime.obterNomeRaca()] = {
        racaSlime.obterNomeRaca(), "Floresta", "Superficie",
        racaSlime.obterAparenciaRaca(),
        "Massas gelatinosas que absorvem tudo o que tocam.",
        "Slimes podem digerir materiais em dias, mas detestam sal.",
        {
            " > Vida           : " + std::to_string(attrSlime.vida),
            " > Forca          : " + std::to_string(attrSlime.forca),
            " > Destreza       : " + std::to_string(attrSlime.destreza),
            " > Resistencia    : " + std::to_string(attrSlime.resistencia),
            " > Constituicao   : " + std::to_string(attrSlime.constituicao),
            " > Inteligencia   : " + std::to_string(attrSlime.inteligencia),
            " > Sabedoria      : " + std::to_string(attrSlime.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaSlime.obterNomeHabilidadeRaca() + " | " + racaSlime.obterDescricaoHabilidadeRaca(),
        {"Gosma acida", "Nucleo pegajoso", "Ouro"},
        2
    };

    RacaFada racaFada;
    Atributos attrFada = racaFada.obterAtributosRaca();
    inimigosBase[racaFada.obterNomeRaca()] = {
        racaFada.obterNomeRaca(), "Floresta", "Superficie",
        racaFada.obterAparenciaRaca(),
        "Fadas corrompidas pela energia negra da floresta.",
        "Elas costumavam guiar viajantes, agora os perdem.",
        {
            " > Vida           : " + std::to_string(attrFada.vida),
            " > Forca          : " + std::to_string(attrFada.forca),
            " > Destreza       : " + std::to_string(attrFada.destreza),
            " > Resistencia    : " + std::to_string(attrFada.resistencia),
            " > Constituicao   : " + std::to_string(attrFada.constituicao),
            " > Inteligencia   : " + std::to_string(attrFada.inteligencia),
            " > Sabedoria      : " + std::to_string(attrFada.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaFada.obterNomeHabilidadeRaca() + " | " + racaFada.obterDescricaoHabilidadeRaca(),
        {"Po magico", "Ouro"},
        3
    };

    RacaOrkExilado racaOrk;
    Atributos attrOrk = racaOrk.obterAtributosRaca();
    inimigosBase[racaOrk.obterNomeRaca()] = {
        racaOrk.obterNomeRaca(), "Vila Inicial", "Caverna do Ork",
        racaOrk.obterAparenciaRaca(),
        "Um Ork expulso de seu cla, agora vive em cavernas escuras planejando vinganca.",
        "Orks exilados pintam suas armaduras com o sangue de suas vitimas.",
        {
            " > Vida           : " + std::to_string(attrOrk.vida),
            " > Forca          : " + std::to_string(attrOrk.forca),
            " > Destreza       : " + std::to_string(attrOrk.destreza),
            " > Resistencia    : " + std::to_string(attrOrk.resistencia),
            " > Constituicao   : " + std::to_string(attrOrk.constituicao),
            " > Inteligencia   : " + std::to_string(attrOrk.inteligencia),
            " > Sabedoria      : " + std::to_string(attrOrk.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaOrk.obterNomeHabilidadeRaca() + " | " + racaOrk.obterDescricaoHabilidadeRaca(),
        {"Machado de guerra danificado", "Armadura de trapos e sucata", "Ouro"},
        4
    };

    RacaAbominacaoFloresta racaAbominacao;
    Atributos attrAbom = racaAbominacao.obterAtributosRaca();
    inimigosBase[racaAbominacao.obterNomeRaca()] = {
        racaAbominacao.obterNomeRaca(), "Floresta", "Coracao da Arvore",
        racaAbominacao.obterAparenciaRaca(),
        "A essencia corrompida da propria floresta, manifestada em uma criatura horripilante.",
        "Sua presenca apodrece a vida ao seu redor.",
        {
            " > Vida           : " + std::to_string(attrAbom.vida),
            " > Forca          : " + std::to_string(attrAbom.forca),
            " > Destreza       : " + std::to_string(attrAbom.destreza),
            " > Resistencia    : " + std::to_string(attrAbom.resistencia),
            " > Constituicao   : " + std::to_string(attrAbom.constituicao),
            " > Inteligencia   : " + std::to_string(attrAbom.inteligencia),
            " > Sabedoria      : " + std::to_string(attrAbom.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaAbominacao.obterNomeHabilidadeRaca() + " | " + racaAbominacao.obterDescricaoHabilidadeRaca(),
        {"Coracao da floresta", "Madeira enfeiticada", "Ouro"},
        5
    };
}

void Bestiario::registrarPrimeiraVista(const std::string& nomeInimigo) {
    if (inimigosBase.count(nomeInimigo)) vistos.insert(nomeInimigo);
}

void Bestiario::registrarDerrota(const std::string& nomeInimigo) {
    if (inimigosBase.count(nomeInimigo)) {
        vistos.insert(nomeInimigo);
        derrotados.insert(nomeInimigo);
    }
}

void Bestiario::registrarHabilidadeVista(const std::string& nomeInimigo, const std::string& habilidade) {
    if (inimigosBase.count(nomeInimigo)) habilidadesVistas[nomeInimigo].insert(habilidade);
}

void Bestiario::registrarDrop(const std::string& nomeInimigo, const std::string& drop) {
    if (inimigosBase.count(nomeInimigo)) dropsColetados[nomeInimigo].insert(drop);
}

bool Bestiario::estaDescoberto(const std::string& nomeInimigo) const {
    return vistos.count(nomeInimigo) > 0;
}

bool Bestiario::jaDerrotado(const std::string& nomeInimigo) const {
    return derrotados.count(nomeInimigo) > 0;
}

bool Bestiario::jaViuHabilidade(const std::string& nomeInimigo, const std::string& habilidade) const {
    auto it = habilidadesVistas.find(nomeInimigo);
    if (it != habilidadesVistas.end()) return it->second.count(habilidade) > 0;
    return false;
}

bool Bestiario::jaColetouDrop(const std::string& nomeInimigo, const std::string& drop) const {
    auto it = dropsColetados.find(nomeInimigo);
    if (it != dropsColetados.end()) return it->second.count(drop) > 0;
    return false;
}

const BestiarioEnemyInfo* Bestiario::obterInfo(const std::string& nomeInimigo) const {
    auto it = inimigosBase.find(nomeInimigo);
    if (it != inimigosBase.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiario::obterInimigosOrdenadosPorDificuldade() const {
    std::vector<std::string> nomes;
    for (const auto& par : inimigosBase) nomes.push_back(par.first);
    
    std::sort(nomes.begin(), nomes.end(), [this](const std::string& a, const std::string& b) {
        return inimigosBase.at(a).dificuldade < inimigosBase.at(b).dificuldade;
    });
    
    return nomes;
}

void Bestiario::salvar(std::ofstream& out) const {
    out << vistos.size() << "\n";
    for (const auto& v : vistos) out << v << "\n";

    out << derrotados.size() << "\n";
    for (const auto& d : derrotados) out << d << "\n";

    out << habilidadesVistas.size() << "\n";
    for (const auto& [ini, habs] : habilidadesVistas) {
        out << ini << "\n" << habs.size() << "\n";
        for (const auto& h : habs) out << h << "\n";
    }

    out << dropsColetados.size() << "\n";
    for (const auto& [ini, drops] : dropsColetados) {
        out << ini << "\n" << drops.size() << "\n";
        for (const auto& d : drops) out << d << "\n";
    }
}

void Bestiario::carregar(std::ifstream& in) {
    vistos.clear();
    derrotados.clear();
    habilidadesVistas.clear();
    dropsColetados.clear();

    size_t size;
    std::string str;

    if (!(in >> size)) return; // Protecao para saves antigos que nao tinham bestiario
    std::getline(in, str); // Limpa o \n
    for (size_t i = 0; i < size; ++i) {
        std::getline(in, str);
        vistos.insert(str);
    }

    in >> size;
    std::getline(in, str);
    for (size_t i = 0; i < size; ++i) {
        std::getline(in, str);
        derrotados.insert(str);
    }

    in >> size;
    std::getline(in, str);
    for (size_t i = 0; i < size; ++i) {
        std::string ini;
        std::getline(in, ini);
        size_t habSize;
        in >> habSize;
        std::getline(in, str);
        for (size_t j = 0; j < habSize; ++j) {
            std::getline(in, str);
            habilidadesVistas[ini].insert(str);
        }
    }

    in >> size;
    std::getline(in, str);
    for (size_t i = 0; i < size; ++i) {
        std::string ini;
        std::getline(in, ini);
        size_t dropSize;
        in >> dropSize;
        std::getline(in, str);
        for (size_t j = 0; j < dropSize; ++j) {
            std::getline(in, str);
            dropsColetados[ini].insert(str);
        }
    }
}