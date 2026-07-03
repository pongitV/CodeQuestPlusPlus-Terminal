#include "TelaDiarioLogic.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../../Sistemas/Progresso/Diario.h"
#include "../../../Sistemas/Inventario/FabricaItens.h"
#include "../../../Sistemas/Inventario/Item.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"

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
