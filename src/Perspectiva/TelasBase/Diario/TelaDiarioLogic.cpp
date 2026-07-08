#include "TelaDiarioLogic.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../../Sistemas/Progresso/Diario.h"
#include "../../../Sistemas/Progresso/Bestiario.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"

#include <algorithm>
#include <map>
#include <functional>
#include <sstream>

namespace {
    enum class CategoriaProgresso { NPC, MONSTRO, ITEM };

    struct ItemProgresso {
        const char* flag;
        const char* nome;
        const char* descricao;
        CategoriaProgresso categoria;
    };

    const std::vector<ItemProgresso> itensDeProgresso = {
        {Flags::Vila_BjornResgatado, "O Salvador da Forja", "Resgatou o ferreiro Bjorn encurralado por um Orc.", CategoriaProgresso::NPC},
        {Flags::Vila_ConviteReal, "Passe Real", "Ajudou os cavaleiros a se livrarem dos Trolls e recebeu um convite para o Reino.", CategoriaProgresso::NPC},
        {Flags::Floresta_MissaoMorgana, "Pacto com a Bruxa", "Entregou os Coracoes da Floresta para Morgana e recebeu a chave para o Labirinto.", CategoriaProgresso::NPC},
        {Flags::Floresta_MahoragaDerrotado, "Ritual concluido", "Derrotou Mahoraga pela primeira vez.", CategoriaProgresso::MONSTRO},
        {Flags::PonteReino_TrollDerrotado, "Pacificador do Reino", "Derrotou todos os Trolls que invadiram a entrada do Reino.", CategoriaProgresso::MONSTRO}
    };

    struct MissaoRegistro {
        std::string id;
        std::string nome;
        std::function<bool(Personagem*)> checarRequisitos;
    };

    const std::vector<MissaoRegistro> registroDeMissoes = {
        {
            "morgana_coracoes",
            "Consiga 3x Coracoes da floresta (Morgana)",
            [](Personagem* p) { return p->obterInventario()->contarItem("Coracao da floresta") >= 3; }
        },
        {
            "cavaleiro_trolls",
            "Reportar Trolls derrotados (Cavaleiro Real)",
            [](Personagem*) { return Progressao::instancia().obterFlag(Flags::PonteReino_TrollDerrotado); }
        }
    };
}

DadosProgresso TelaDiarioLogic::obterProgresso() {
    DadosProgresso dados;
    int conquistasExibidas = 0;

    auto processarCategoria = [&](const std::string& titulo, CategoriaProgresso categoria) -> int {
        std::vector<std::string>* alvo = nullptr;
        if (categoria == CategoriaProgresso::NPC) alvo = &dados.linhasNPC;
        else if (categoria == CategoriaProgresso::MONSTRO) alvo = &dados.linhasMonstro;
        else if (categoria == CategoriaProgresso::ITEM) alvo = &dados.linhasItem;

        int count = 0;
        for (const auto& item : itensDeProgresso) {
            if (item.categoria == categoria && Progressao::instancia().obterFlag(item.flag)) {
                if (alvo->empty()) {
                    alvo->push_back("  " + Aparencia::cor(Cor::CIANO) + "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 " + titulo + " \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90" + Aparencia::cor(Cor::RESET));
                    alvo->push_back("");
                }
                std::string corNome = Aparencia::cor(Cor::AMARELO);
                std::string corDesc = Aparencia::cor(Cor::BRANCO);
                std::string status = Aparencia::cor(Cor::VERDE) + "[CONCLUIDO]";

                alvo->push_back("    " + status + " " + corNome + item.nome + Aparencia::cor(Cor::RESET));
                alvo->push_back("      " + corDesc + "  " + item.descricao + Aparencia::cor(Cor::RESET));
                alvo->push_back("");
                count++;
            }
        }
        return count;
    };

    conquistasExibidas += processarCategoria("NPCs", CategoriaProgresso::NPC);
    conquistasExibidas += processarCategoria("Monstros", CategoriaProgresso::MONSTRO);
    conquistasExibidas += processarCategoria("Itens", CategoriaProgresso::ITEM);

    dados.totalConquistas = conquistasExibidas;
    return dados;
}

ItensCategorizados TelaDiarioLogic::categorizarItens(Personagem* jogador) {
    (void)jogador;
    ItensCategorizados resultado;
    std::vector<std::string> itens = Diario::instancia().obterItensDescobertos();

    for (const auto& itemNome : itens) {
        auto tempItem = FabricaItens::criarItem(itemNome);
        if (tempItem) {
            TipoEquipamento tipo = tempItem->obterTipo();
            std::string prefixo = " - " + itemNome;
            if (tipo == TipoEquipamento::ARMA) resultado.armas.push_back(prefixo);
            else if (tipo == TipoEquipamento::ESCUDO) resultado.escudos.push_back(prefixo);
            else if (tipo == TipoEquipamento::ARMADURA) resultado.armaduras.push_back(prefixo);
            else if (tipo == TipoEquipamento::CONSUMIVEL) resultado.consumiveis.push_back(prefixo);
            else if (tipo == TipoEquipamento::MATERIAL) resultado.materiais.push_back(prefixo);
            else if (tipo == TipoEquipamento::MISSAO) resultado.missoes.push_back(prefixo);
            else resultado.outros.push_back(prefixo);
        } else {
            resultado.outros.push_back(" - " + itemNome);
        }
    }

    Aparencia::ordenarAlfabeticamente(resultado.armas);
    Aparencia::ordenarAlfabeticamente(resultado.escudos);
    Aparencia::ordenarAlfabeticamente(resultado.armaduras);
    Aparencia::ordenarAlfabeticamente(resultado.consumiveis);
    Aparencia::ordenarAlfabeticamente(resultado.materiais);
    Aparencia::ordenarAlfabeticamente(resultado.missoes);
    Aparencia::ordenarAlfabeticamente(resultado.outros);

    return resultado;
}

MissoesCategorizadas TelaDiarioLogic::categorizarMissoes(Personagem* jogador) {
    MissoesCategorizadas resultado;

    for (const auto& missao : registroDeMissoes) {
        if (Diario::instancia().missaoConcluida(missao.id)) {
            resultado.completas.push_back("[V] " + missao.nome);
        } else if (Diario::instancia().missaoAceita(missao.id)) {
            if (missao.checarRequisitos(jogador)) {
                resultado.prontas.push_back("[X] " + missao.nome);
            } else {
                resultado.emAndamento.push_back("[ ] " + missao.nome);
            }
        }
    }

    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarBestiario() {
    std::map<std::string, std::vector<std::string>> grupos;
    auto ordem = Bestiario::instancia().obterInimigosOrdenadosPorDificuldade();
    for (const auto& nome : ordem) {
        if (!Bestiario::instancia().estaDescoberto(nome)) continue;
        const auto* info = Bestiario::instancia().obterInfo(nome);
        std::string mapa = info ? info->mapa : "Desconhecido";
        grupos[mapa].push_back(nome);
    }

    std::vector<GrupoCategorizado> resultado;
    for (auto& par : grupos) {
        Aparencia::ordenarAlfabeticamente(par.second);
        resultado.push_back({par.first, par.second});
    }
    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarNPCs() {
    std::map<std::string, std::vector<std::string>> grupos;
    auto npcs = Diario::instancia().obterNPCsDescobertos();
    for (const auto& nome : npcs) {
        std::string area = "Viajante";
        if (nome.find("Bjorn") != std::string::npos || nome.find("Cavaleiro Real") != std::string::npos) {
            area = "Vila/Reino";
        } else if (nome.find("Morgana") != std::string::npos) {
            area = "Floresta";
        } else if (nome.find("Franchesco") != std::string::npos) {
            area = "Viajante";
        }
        grupos[area].push_back(nome);
    }

    std::vector<GrupoCategorizado> resultado;
    for (auto& par : grupos) {
        Aparencia::ordenarAlfabeticamente(par.second);
        resultado.push_back({par.first, par.second});
    }
    return resultado;
}

std::vector<GrupoCategorizado> TelaDiarioLogic::categorizarRacas(const std::vector<std::string>& racasDescobertas) {
    std::vector<std::string> jogaveis, monstros;
    for (const auto& raca : racasDescobertas) {
        if (raca == "Humano" || raca == "Dwarf" || raca == "Elfo" || raca == "Ork") {
            jogaveis.push_back(raca);
        } else {
            monstros.push_back(raca);
        }
    }
    Aparencia::ordenarAlfabeticamente(jogaveis);
    Aparencia::ordenarAlfabeticamente(monstros);

    std::vector<GrupoCategorizado> resultado;
    if (!jogaveis.empty()) resultado.push_back({"Racas Jogaveis", jogaveis});
    if (!monstros.empty()) resultado.push_back({"Monstros e Inimigos", monstros});
    return resultado;
}

std::vector<std::string> TelaDiarioLogic::obterTodasClasses() {
    return {"Guerreiro", "Mago", "Arqueiro", "Bardo", "Necromante"};
}

std::vector<std::string> TelaDiarioLogic::quebrarTexto(const std::string& texto, int larguraMax) {
    std::vector<std::string> resultado;
    std::istringstream stream(texto);
    std::string linhaAtual;
    std::string palavra;
    while (stream >> palavra) {
        if (linhaAtual.length() + palavra.length() + (linhaAtual.empty() ? 0 : 1) > (size_t)larguraMax) {
            if (!linhaAtual.empty()) resultado.push_back(linhaAtual);
            linhaAtual = palavra;
        } else {
            if (!linhaAtual.empty()) linhaAtual += ' ';
            linhaAtual += palavra;
        }
    }
    if (!linhaAtual.empty()) resultado.push_back(linhaAtual);
    if (resultado.empty() && !texto.empty()) resultado.push_back(texto);
    return resultado;
}
