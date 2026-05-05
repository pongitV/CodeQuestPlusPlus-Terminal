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
#include "../Inventario/FabricaItens.h"

SistemaBestiario& SistemaBestiario::instancia() {
    static SistemaBestiario inst;
    return inst;
}

SistemaBestiario::SistemaBestiario() {
    inicializarInimigos();
}

void SistemaBestiario::inicializarInimigos() {
    ClasseBaseInimigo classePadrao;

    auto formatarAtributos = [](const Atributos& attr) -> std::vector<std::string> {
        return {
            " > Vida           : " + std::to_string(attr.vida),
            " > Forca          : " + std::to_string(attr.forca),
            " > Destreza       : " + std::to_string(attr.destreza),
            " > Resistencia    : " + std::to_string(attr.resistencia),
            " > Constituicao   : " + std::to_string(attr.constituicao),
            " > Inteligencia   : " + std::to_string(attr.inteligencia),
            " > Sabedoria      : " + std::to_string(attr.sabedoria)
        };
    };

    Goblin racaGoblin;
    Atributos attrGoblin = racaGoblin.obterAtributosRaca();
    inimigosBase[racaGoblin.obterNomeRaca()] = {
        racaGoblin.obterNomeRaca(), "Vila Inicial", "Arredores",
        racaGoblin.obterAparenciaRaca(),
        "Pequenas criaturas verdes e astutas que costumam viver em bandos perto de vilarejos.",
        "Goblins adoram itens brilhantes e guardam dentes como trofeus.",
        formatarAtributos(attrGoblin),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaGoblin.obterNomeHabilidadeRaca() + " | " + racaGoblin.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::DenteGoblin), FabricaItens::obterNomeDeID(ItemID::AdagaPedra), "Ouro"},
        1
    };

    Slime racaSlime;
    Atributos attrSlime = racaSlime.obterAtributosRaca();
    inimigosBase[racaSlime.obterNomeRaca()] = {
        racaSlime.obterNomeRaca(), "Floresta", "Superficie",
        racaSlime.obterAparenciaRaca(),
        "Massas gelatinosas que absorvem tudo o que tocam.",
        "Slimes podem digerir materiais em dias, mas detestam sal.",
        formatarAtributos(attrSlime),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaSlime.obterNomeHabilidadeRaca() + " | " + racaSlime.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::GosmaAcida), FabricaItens::obterNomeDeID(ItemID::NucleoPegajoso), "Ouro"},
        2
    };

    Fada racaFada;
    Atributos attrFada = racaFada.obterAtributosRaca();
    inimigosBase[racaFada.obterNomeRaca()] = {
        racaFada.obterNomeRaca(), "Floresta", "Superficie",
        racaFada.obterAparenciaRaca(),
        "Fadas corrompidas pela energia negra da floresta.",
        "Elas costumavam guiar viajantes, agora os perdem.",
        formatarAtributos(attrFada),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaFada.obterNomeHabilidadeRaca() + " | " + racaFada.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::PoMagico), FabricaItens::obterNomeDeID(ItemID::VarinhaCorroida), "Ouro"},
        3
    };

    OrkExilado racaOrk;
    Atributos attrOrk = racaOrk.obterAtributosRaca();
    inimigosBase[racaOrk.obterNomeRaca()] = {
        racaOrk.obterNomeRaca(), "Vila Inicial", "Caverna do Ork",
        racaOrk.obterAparenciaRaca(),
        "Um Ork expulso de seu cla, agora vive em cavernas escuras planejando vinganca.",
        "Orks exilados pintam suas armaduras com o sangue de suas vitimas.",
        formatarAtributos(attrOrk),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaOrk.obterNomeHabilidadeRaca() + " | " + racaOrk.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::MachadoGuerra), FabricaItens::obterNomeDeID(ItemID::ArmaduraTrapos), "Ouro"},
        4
    };

    AbominacaoFloresta racaAbominacao;
    Atributos attrAbom = racaAbominacao.obterAtributosRaca();
    inimigosBase[racaAbominacao.obterNomeRaca()] = {
        racaAbominacao.obterNomeRaca(), "Floresta", "Coracao da Arvore",
        racaAbominacao.obterAparenciaRaca(),
        "A essencia corrompida da propria floresta, manifestada em uma criatura horripilante.",
        "Sua presenca apodrece a vida ao seu redor.",
        formatarAtributos(attrAbom),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaAbominacao.obterNomeHabilidadeRaca() + " | " + racaAbominacao.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::CoracaoFloresta), FabricaItens::obterNomeDeID(ItemID::MadeiraEnfeiticada), "Ouro"},
        5
    };

    Troll racaTroll;
    Atributos attrTroll = racaTroll.obterAtributosRaca();
    inimigosBase[racaTroll.obterNomeRaca()] = {
        racaTroll.obterNomeRaca(), "Montanhas", "Caverna Profunda",
        racaTroll.obterAparenciaRaca(),
        "Uma criatura gigantesca, incrivelmente forte e resistente, capaz de empunhar arvores inteiras como clavas.",
        "A lenda diz que as feridas de um Troll se fecham em segundos.",
        formatarAtributos(attrTroll),
        {classePadrao.obterNomeHabilidadeClasse() + " | " + classePadrao.obterDescricaoHabilidadeClasse()},
        racaTroll.obterNomeHabilidadeRaca() + " | " + racaTroll.obterDescricaoHabilidadeRaca(),
        {FabricaItens::obterNomeDeID(ItemID::TroncoAmarrotado), FabricaItens::obterNomeDeID(ItemID::OrgaoRegenerador), "Ouro"},
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