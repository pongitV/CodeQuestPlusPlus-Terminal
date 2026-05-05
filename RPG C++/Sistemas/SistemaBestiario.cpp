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

void SistemaBestiario::inicializarInimigos() {
    ClasseBaseInimigo classePadrao;

    Goblin racaGoblin;
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

    Slime racaSlime;
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

    Fada racaFada;
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
        {"Po magico", "Varinha corroida", "Ouro"},
        3
    };

    OrkExilado racaOrk;
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

    AbominacaoFloresta racaAbominacao;
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

    Troll racaTroll;
    Atributos attrTroll = racaTroll.obterAtributosRaca();
    inimigosBase[racaTroll.obterNomeRaca()] = {
        racaTroll.obterNomeRaca(), "Montanhas", "Caverna Profunda",
        racaTroll.obterAparenciaRaca(),
        "Uma criatura gigantesca, incrivelmente forte e resistente, capaz de empunhar arvores inteiras como clavas.",
        "A lenda diz que as feridas de um Troll se fecham em segundos.",
        {
            " > Vida           : " + std::to_string(attrTroll.vida),
            " > Forca          : " + std::to_string(attrTroll.forca),
            " > Destreza       : " + std::to_string(attrTroll.destreza),
            " > Resistencia    : " + std::to_string(attrTroll.resistencia),
            " > Constituicao   : " + std::to_string(attrTroll.constituicao),
            " > Inteligencia   : " + std::to_string(attrTroll.inteligencia),
            " > Sabedoria      : " + std::to_string(attrTroll.sabedoria)
        },
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaTroll.obterNomeHabilidadeRaca() + " | " + racaTroll.obterDescricaoHabilidadeRaca(),
        {"Tronco de arvore amarrotado", "Orgao regenerador", "Ouro"},
        6
    };
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
    for (const auto& par : inimigosBase) nomes.push_back(par.first);
    
    std::sort(nomes.begin(), nomes.end(), [this](const std::string& a, const std::string& b) {
        return inimigosBase.at(a).dificuldade < inimigosBase.at(b).dificuldade;
    });
    
    return nomes;
}

void SistemaBestiario::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
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

void SistemaBestiario::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
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