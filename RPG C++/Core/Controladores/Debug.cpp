#include "Debug.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "../../Entidades/Personagem.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"
#include "../../Core/Controladores/CriadorInimigos.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../Utilidades/FuncoesDialogo.h"
#include "../../Interface/Telas/TelaBase.h"
#include "../../Sistemas/Progresso/Progressao.h"
#include "../../Sistemas/Progresso/ProgressaoFlags.h"

namespace {
    void ativarGodMode(Personagem* jogador) {
        jogador->obterAtributosFinais().vida += 999999;
        jogador->obterAtributosFinais().forca += 99999;
        jogador->obterAtributosFinais().destreza += 99999;
        jogador->obterAtributosFinais().resistencia += 99999;
        jogador->obterAtributosFinais().constituicao += 99999;
        jogador->obterAtributosFinais().inteligencia += 99999;
        jogador->obterAtributosFinais().sabedoria += 99999;
        jogador->forcarRecalculoCache();
        jogador->definirVida(jogador->obterVidaMaxima());
        
        std::cout << "\n";
        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("God Mode ativado! Voce agora e um deus intocavel.", Cor::AMARELO));
        std::cout << "\n";
        ControleDeInput::aguardarEnter();
    }

    void menuDefinirAtributos(Personagem* jogador) {
        TelaBase::executarLoopPadrao(
            "DEFINIR ATRIBUTOS (CHEAT)", Cor::AMARELO,
            nullptr,
            [jogador]() {
                auto& attrs = jogador->obterAtributosFinais();
                return std::vector<std::string>{
                    "Vida Maxima  : " + std::to_string(attrs.vida), "Forca        : " + std::to_string(attrs.forca),
                    "Destreza     : " + std::to_string(attrs.destreza), "Resistencia  : " + std::to_string(attrs.resistencia),
                    "Constituicao : " + std::to_string(attrs.constituicao), "Inteligencia : " + std::to_string(attrs.inteligencia),
                    "Sabedoria    : " + std::to_string(attrs.sabedoria), "Voltar"
                };
            },
            [jogador](int escolhaAtr) {
                if (escolhaAtr == 7 || escolhaAtr == -1) return false;
                
                auto& attrs = jogador->obterAtributosFinais();
                std::string nomeAtr; int* ptrAtr = nullptr;
                if (escolhaAtr == 0) { nomeAtr = "Vida Maxima"; ptrAtr = &attrs.vida; }
                else if (escolhaAtr == 1) { nomeAtr = "Forca"; ptrAtr = &attrs.forca; }
                else if (escolhaAtr == 2) { nomeAtr = "Destreza"; ptrAtr = &attrs.destreza; }
                else if (escolhaAtr == 3) { nomeAtr = "Resistencia"; ptrAtr = &attrs.resistencia; }
                else if (escolhaAtr == 4) { nomeAtr = "Constituicao"; ptrAtr = &attrs.constituicao; }
                else if (escolhaAtr == 5) { nomeAtr = "Inteligencia"; ptrAtr = &attrs.inteligencia; }
                else if (escolhaAtr == 6) { nomeAtr = "Sabedoria"; ptrAtr = &attrs.sabedoria; }
                
                std::cout << "\n";
                int novoValor = ControleDeInput::lerInteiroComLimites("Defina o novo valor para " + nomeAtr + ": ", 0, 999999, true);
                *ptrAtr = novoValor;
                jogador->forcarRecalculoCache();
                if (escolhaAtr == 0) jogador->definirVida(jogador->obterVidaMaxima());
                return true;
            }
        );
    }

    std::map<std::string, std::vector<ItemID>> obterTodosItensCategorizados() {
        std::map<std::string, std::vector<ItemID>> categorias;
        
        // Varredura automatica de IDs de itens (ate 200 para garantir que pegue todos os novos itens que foram ou serao criados)
        for (int i = 1; i <= 200; ++i) {
            ItemID id = static_cast<ItemID>(i);
            std::string nome = FabricaItens::obterNomeDeID(id);
            if (!nome.empty() && nome != "Desconhecido") {
                auto tempItem = FabricaItens::criarItem(id);
                if (tempItem) {
                    TipoEquipamento tipo = tempItem->obterTipo();
                    if (tipo == TipoEquipamento::ARMA) categorias["Armas"].push_back(id);
                    else if (tipo == TipoEquipamento::ARMADURA) categorias["Armaduras"].push_back(id);
                    else if (tipo == TipoEquipamento::ESCUDO) categorias["Escudos"].push_back(id);
                    else {
                        // Classifica os demais itens pelos nomes ou definicoes base
                        if (nome.find("Talisma") != std::string::npos || nome == "Convite" || nome == "Dispositivo") {
                            categorias["Missoes"].push_back(id);
                        } else if (nome.find("Pocao") != std::string::npos || nome.find("Frasco") != std::string::npos || nome.find("Elixir") != std::string::npos || nome.find("Regenerador") != std::string::npos) {
                            categorias["Consumiveis"].push_back(id);
                        } else {
                            categorias["Materiais"].push_back(id);
                        }
                    }
                }
            }
        }
        return categorias;
    }

    void menuObterAlma(Personagem* jogador) {
        struct AlmaOpcao {
            std::string nome;
            std::vector<std::unique_ptr<Personagem>> (*criador)(int);
        };

        std::vector<AlmaOpcao> opcoesAlmas = {
            {"Goblin", CriadorInimigos::criarInimigoGoblin},
            {"Slime", CriadorInimigos::criarInimigoSlime},
            {"Fada", CriadorInimigos::criarInimigoFada},
            {"Orc Exilado", CriadorInimigos::criarInimigoOrkExilado},
            {"Abominacao da Floresta", CriadorInimigos::criarInimigoAbominacaoFloresta},
            {"Troll", CriadorInimigos::criarInimigoTroll},
            {"Mimico", CriadorInimigos::criarInimigoMimico},
            {"Mahoraga", CriadorInimigos::criarInimigoMahoraga}
        };

        Aparencia::ordenarAlfabeticamente(opcoesAlmas, [](const AlmaOpcao& op) { return op.nome; });

        TelaBase::executarLoopPadrao(
            "OBTER ALMAS (NECROMANTE)", Cor::AMARELO,
            nullptr,
            [&opcoesAlmas]() {
                std::vector<std::string> nomes;
                for (const auto& op : opcoesAlmas) nomes.push_back(op.nome);
                nomes.push_back("Voltar");
                return nomes;
            },
            [jogador, &opcoesAlmas](int escolhaAlma) {
                if (escolhaAlma == static_cast<int>(opcoesAlmas.size()) || escolhaAlma == -1) return false;
                
                std::string nomeCriado = opcoesAlmas[escolhaAlma].nome;
                
                std::cout << "\n";
                int quantidade = ControleDeInput::lerInteiroComLimites("Quantidade (1 a 99): ", 1, 99);
                auto criados = opcoesAlmas[escolhaAlma].criador(quantidade);
                
                int adicionados = 0;
                for (auto& alma : criados) {
                    if (alma) { jogador->adicionarAlma(std::move(alma)); adicionados++; }
                }
                
                if (adicionados > 0) {
                    std::cout << "\n";
                    std::string plural = adicionados > 1 ? "s" : "";
                    Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgInteracao(std::to_string(adicionados) + "x Alma" + plural + " de " + nomeCriado + " adicionada" + plural + " com sucesso!"));
                    ControleDeInput::aguardarEnter();
                }
                return true;
            }
        );
    }

    void menuObterItem(Personagem* jogador) {
        TelaBase::executarLoopPadrao(
            "OBTER ITEM (CATEGORIAS)", Cor::AMARELO,
            nullptr,
            []() {
                return std::vector<std::string>{"Armas", "Armaduras", "Escudos", "Materiais", "Consumiveis", "Missoes", "Almas de Classes/Inimigos (Necromante)", "Voltar"};
            },
            [jogador](int escolhaCat) {
                if (escolhaCat == 7 || escolhaCat == -1) return false;
                if (escolhaCat == 6) { menuObterAlma(jogador); return true; }
                
                std::string categoriasNomes[] = { "Armas", "Armaduras", "Escudos", "Materiais", "Consumiveis", "Missoes" };
                std::string catSelecionada = categoriasNomes[escolhaCat];
                auto todasCat = obterTodosItensCategorizados();
                auto itensDaCat = todasCat[catSelecionada];
                
                if (itensDaCat.empty()) {
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Nenhum item encontrado nesta categoria.", Cor::VERMELHO));
                    ControleDeInput::aguardarEnter();
                    return true;
                }

                Aparencia::ordenarAlfabeticamente(itensDaCat, [](ItemID id) { return FabricaItens::obterNomeDeID(id); });

                TelaBase::executarLoopPadrao(
                    "OBTER ITEM - " + catSelecionada, Cor::AMARELO,
                    nullptr,
                    [&itensDaCat]() {
                        std::vector<std::string> opcoes;
                        for (auto id : itensDaCat) opcoes.push_back(FabricaItens::obterNomeDeID(id));
                        opcoes.push_back("Voltar");
                        return opcoes;
                    },
                    [jogador, &itensDaCat](int escolhaItem) {
                        if (escolhaItem == static_cast<int>(itensDaCat.size()) || escolhaItem == -1) return false;
                        ItemID idEscolhido = itensDaCat[escolhaItem];
                        std::cout << "\n";
                        int quantidade = ControleDeInput::lerInteiroComLimites("Quantidade (1 a 99): ", 1, 99);
                        for (int q = 0; q < quantidade; ++q) jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idEscolhido));
                        Diario::instancia().registrarItem(Aparencia::removerCoresANSI(FabricaItens::obterNomeDeID(idEscolhido)));
                        std::cout << "\n";
                        std::string plural = quantidade > 1 ? "s" : "";
                        Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgInteracao(std::to_string(quantidade) + "x '" + FabricaItens::obterNomeDeID(idEscolhido) + "' adicionado" + plural + " ao inventario!"));
                        ControleDeInput::aguardarEnter();
                        return true;
                    }
                );
                return true;
            }
        );
    }
}

void Debug::exibirMenuDebug(Personagem* jogador) {
    TelaBase::executarLoopPadrao(
        "MENU DE DEBUG (CHEAT)", Cor::AMARELO,
        nullptr,
        [jogador]() {
            return std::vector<std::string>{
                "God Mode (Max Atributos - Instakill/Imortal)",
                "Definir Atributos Livres",
                "Obter Qualquer Item",
                "Adicionar Ouro e XP (+10000)",
                "Liberar Todos os Mapas (Fast Travel)",
                std::string("Noclip (Atravessar paredes): ") + (jogador->isNoclip() ? Aparencia::cor(Cor::VERDE) + "LIGADO" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::VERMELHO) + "DESLIGADO" + Aparencia::cor(Cor::RESET)),
                "Fechar Debug Menu"
            };
        },
        [jogador](int escolhaDebug) {
            if (escolhaDebug == 0) {
                ativarGodMode(jogador);
            } else if (escolhaDebug == 1) {
                menuDefinirAtributos(jogador);
            } else if (escolhaDebug == 2) {
                menuObterItem(jogador);
            } else if (escolhaDebug == 3) {
                jogador->ganharOuro(10000);
                jogador->ganharXp(10000);
                std::cout << "\n";
                Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("+10000 Ouro e +10000 XP adicionados!", Cor::AMARELO));
                std::cout << "\n";
                ControleDeInput::aguardarEnter();
            } else if (escolhaDebug == 4) {
                Progressao::instancia().definirFlag(Flags::Vila_BjornResgatado, true);
                Progressao::instancia().definirFlag(Flags::Floresta_MissaoMorgana, true);
                Progressao::instancia().definirFlag(Flags::Floresta_MahoragaDerrotado, true);
                Progressao::instancia().definirFlag(Flags::Visitou_Floresta, true);
                Progressao::instancia().definirFlag(Flags::Visitou_Reino, true);
                std::cout << "\n";
                Aparencia::imprimirCentralizado(FuncoesDialogo::formatarMsgSistema("Todos os mapas e submapas liberados para Viagem Rapida (Tecla M)!", Cor::AMARELO));
                std::cout << "\n";
                ControleDeInput::aguardarEnter();
            } else if (escolhaDebug == 5) {
                jogador->alternarNoclip();
            } else if (escolhaDebug == 6 || escolhaDebug == -1) { 
                return false; 
            }
            return true;
        }
    );
}
