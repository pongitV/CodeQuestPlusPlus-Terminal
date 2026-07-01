#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "InventarioCombate.h"
#include "../../Entidades/Personagem.h"
#include "Item.h"
#include "Equipamentos/EquipamentoArma.h"
#include "Equipamentos/EquipamentoEscudo.h"
#include "Equipamentos/EquipamentoArmadura.h"
#include "../../Visoes/TelasBase/Inventario/TelaInventario.h"
#include "../../Visoes/TelasBase/Menu/TelaMenu.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Visoes/TelasBase/TelaBase.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"

namespace {
}

void InventarioCombate::gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    TelaBase::executarLoop(
        [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
        [jogadorAtual]() {
            TelaInventario::exibirCaixaEquipados(jogadorAtual);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("BOLSO: " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + " Moedas de Ouro [$$]", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
        },
        [jogadorAtual]() {
            std::vector<std::string> opcoes;
            if (jogadorAtual->obterConsumivelRapido()) {
                int qtd = jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem());
                opcoes.push_back(Aparencia::cor(Cor::VERDE) + "[+] Acesso Rapido: " + jogadorAtual->obterConsumivelRapido()->obterNomeItem() + " (" + std::to_string(qtd) + "x)" + Aparencia::cor(Cor::RESET));
            }
            opcoes.push_back("[X] Arsenal de Equipamentos");
            opcoes.push_back("[o] Itens Consumiveis");
            opcoes.push_back("[#] Estoque e Materiais");
            opcoes.push_back("[!] Itens de Missao");
            opcoes.push_back("[<] VOLTAR");
            return opcoes;
        },
        [&](int escolha) {
            int offset = jogadorAtual->obterConsumivelRapido() ? 1 : 0;
            if (escolha < 0 || escolha == 4 + offset) return false; // VOLTAR
            
            if (offset == 1 && escolha == 0) {
                Item* rapido = jogadorAtual->obterConsumivelRapido();
                std::string nomeRapido = rapido->obterNomeItem(); // Salva o nome antes do item ser deletado
                int countAntes = jogadorAtual->obterInventario()->contarItem(nomeRapido);
                if (countAntes > 0) {
                    bool consumiu = false;
                    processarUsoDeItem(jogadorAtual, rapido, turnoFoiConsumido ? &consumiu : nullptr);
                    if (consumiu && turnoFoiConsumido) *turnoFoiConsumido = true;
                    
                    if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                        return false; 
                    }
                    if (jogadorAtual->obterInventario()->contarItem(nomeRapido) == 0) {
                        jogadorAtual->desequiparConsumivel();
                    }
                } else {
                    jogadorAtual->desequiparConsumivel();
                }
                if (turnoFoiConsumido && *turnoFoiConsumido) return false;
                return true;
            }

            int categoria = escolha - offset;
            
            std::vector<Item*> mapIndexParaItem;
            
            TelaBase::executarLoop(
                [](bool animar) { TelaInventario::exibirCabecalhoInventario(animar); },
                [categoria]() {
                    std::string titulo = "";
                    if (categoria == 0) titulo = "=== [X] ARSENAL DE EQUIPAMENTOS [X] ===";
                    else if (categoria == 1) titulo = "=== [o] ITENS CONSUMIVEIS [o] ===";
                    else if (categoria == 2) titulo = "=== [#] ESTOQUE E MATERIAIS [#] ===";
                    else if (categoria == 3) titulo = "=== [!] ITENS DE MISSAO [!] ===";
                    std::cout << "\n";
                    Aparencia::imprimirCentralizado(titulo, Aparencia::cor(Cor::AMARELO));
                    std::cout << "\n";
                },
                [jogadorAtual, categoria, &mapIndexParaItem]() {
                    auto itens = TelaInventario::obterListaCategoria(jogadorAtual, categoria, false);
                    Aparencia::ordenarAlfabeticamente(itens, [](const auto& par) { return par.first; });
                    
                    std::vector<std::string> ops;
                    mapIndexParaItem.clear();

                    auto adicionarCategoria = [&](const std::string& titulo, const std::vector<std::pair<std::string, Item*>>& lista) {
                        if (!lista.empty()) {
                            std::vector<std::string> linhasItem;
                            for (const auto& p : lista) {
                                linhasItem.push_back(" " + p.first); // Adiciona um pequeno recuo interno
                            }
                            
                            std::vector<std::string> caixa = Aparencia::criarCaixa(linhasItem, titulo, 60, Cor::AMARELO);
                            
                            ops.push_back("#HEADER#" + caixa[0]); // Topo da caixa
                            mapIndexParaItem.push_back(nullptr);
                            for (size_t i = 0; i < lista.size(); ++i) {
                                ops.push_back(caixa[i + 1]); // Linha com o item interativo
                                mapIndexParaItem.push_back(lista[i].second);
                            }
                            ops.push_back("#HEADER#" + caixa.back()); // Fundo da caixa
                            mapIndexParaItem.push_back(nullptr);
                            ops.push_back("#HEADER#"); // Separacao visual entre caixas
                            mapIndexParaItem.push_back(nullptr);
                        }
                    };

                    if (categoria == 0) {
                        std::vector<std::pair<std::string, Item*>> equipados, danoFisico, danoMagico, armaduras, escudos, outros;
                        std::vector<std::string> nomesProcessados;
                        for (auto& p : itens) {
                            Item* it = p.second;
                            std::string nomeItem = it->obterNomeItem();
                            
                            bool jaProcessado = false;
                            for (const auto& n : nomesProcessados) {
                                if (n == nomeItem) {
                                    jaProcessado = true;
                                    break;
                                }
                            }
                            if (jaProcessado) continue;
                            nomesProcessados.push_back(nomeItem);
                            
                            Item* equipado = nullptr;
                            for (auto* invIt : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                if (invIt->obterNomeItem() == nomeItem && jogadorAtual->isItemEquipado(invIt)) {
                                    equipado = invIt;
                                    break;
                                }
                            }
                            
                            if (equipado) {
                                equipados.push_back({nomeItem, equipado});
                                
                                int total = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                int sobra = total - 1;
                                
                                if (sobra > 0) {
                                    std::string nomeSobra = nomeItem + (sobra > 1 ? " (" + std::to_string(sobra) + "x)" : "");
                                    Item* desequipado = nullptr;
                                    for (auto* invIt : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                        if (invIt->obterNomeItem() == nomeItem && !jogadorAtual->isItemEquipado(invIt)) {
                                            desequipado = invIt;
                                            break;
                                        }
                                    }
                                    
                                    if (desequipado) {
                                        if (desequipado->obterTipo() == TipoEquipamento::ARMA) {
                                            if (auto* arma = dynamic_cast<EquipamentoArma*>(desequipado)) {
                                                if (arma->obterDanoMagico() > arma->obterDanoFisico()) danoMagico.push_back({nomeSobra, desequipado});
                                                else danoFisico.push_back({nomeSobra, desequipado});
                                            } else {
                                                outros.push_back({nomeSobra, desequipado});
                                            }
                                        } else if (desequipado->obterTipo() == TipoEquipamento::ARMADURA) {
                                            armaduras.push_back({nomeSobra, desequipado});
                                        } else if (desequipado->obterTipo() == TipoEquipamento::ESCUDO) {
                                            escudos.push_back({nomeSobra, desequipado});
                                        } else {
                                            outros.push_back({nomeSobra, desequipado});
                                        }
                                    }
                                }
                            } else {
                                if (it->obterTipo() == TipoEquipamento::ARMA) {
                                    if (auto* arma = dynamic_cast<EquipamentoArma*>(it)) {
                                        if (arma->obterDanoMagico() > arma->obterDanoFisico()) danoMagico.push_back(p);
                                        else danoFisico.push_back(p);
                                    } else {
                                        outros.push_back(p);
                                    }
                                } else if (it->obterTipo() == TipoEquipamento::ARMADURA) {
                                    armaduras.push_back(p);
                                } else if (it->obterTipo() == TipoEquipamento::ESCUDO) {
                                    escudos.push_back(p);
                                } else {
                                    outros.push_back(p);
                                }
                            }
                        }
                        adicionarCategoria("EQUIPAMENTOS EM USO", equipados);
                        adicionarCategoria("ARMAS - DANO FISICO", danoFisico);
                        adicionarCategoria("ARMAS - DANO MAGICO", danoMagico);
                        adicionarCategoria("ARMADURAS", armaduras);
                        adicionarCategoria("ESCUDOS", escudos);
                        adicionarCategoria("OUTROS EQUIPAMENTOS", outros);
                    } else if (categoria == 1) {
                        std::vector<std::pair<std::string, Item*>> equipados, cura, buff, outros;
                        for (auto& p : itens) {
                            Item* it = p.second;
                            if (jogadorAtual->isItemEquipado(it)) {
                                equipados.push_back(p);
                            } else if (it->temPropriedade(Propriedade::ConsumivelCura)) {
                                cura.push_back(p);
                            } else if (it->temPropriedade(Propriedade::ConsumivelBuff)) {
                                buff.push_back(p);
                            } else {
                                outros.push_back(p);
                            }
                        }
                        adicionarCategoria("EQUIPADO: ACESSO RAPIDO", equipados);
                        adicionarCategoria("POCOES DE CURA E RESTAURACAO", cura);
                        adicionarCategoria("ELIXIRES E BUFFS", buff);
                        adicionarCategoria("OUTROS CONSUMIVEIS", outros);
                    } else {
                        // Missões e Materiais, apenas listar
                        std::vector<std::string> linhas;
                        std::vector<Item*> itensMapeados;
                        for (auto& p : itens) {
                            linhas.push_back(" " + p.first);
                            itensMapeados.push_back(p.second);
                        }
                        if (!linhas.empty()) {
                            std::vector<std::string> caixa = Aparencia::criarCaixa(linhas, "ITENS", 60, Cor::AMARELO);
                            ops.push_back("#HEADER#" + caixa[0]);
                            mapIndexParaItem.push_back(nullptr);
                            for (size_t i = 0; i < itensMapeados.size(); ++i) {
                                ops.push_back(caixa[i + 1]);
                                mapIndexParaItem.push_back(itensMapeados[i]);
                            }
                            ops.push_back("#HEADER#" + caixa.back());
                            mapIndexParaItem.push_back(nullptr);
                            ops.push_back("#HEADER#");
                            mapIndexParaItem.push_back(nullptr);
                        }
                    }

                    ops.push_back("[<] VOLTAR");
                    mapIndexParaItem.push_back(nullptr);
                    return ops;
                },
                [&](int escolhaItem) {
                    if (escolhaItem < 0 || escolhaItem >= static_cast<int>(mapIndexParaItem.size())) return false; // Clicou pra voltar via teclado
                    
                    Item* itemEncontrado = mapIndexParaItem[escolhaItem];
                    if (itemEncontrado == nullptr) {
                        if (escolhaItem == static_cast<int>(mapIndexParaItem.size()) - 1) return false; // Clicou em VOLTAR no fim da lista
                        return true; // Clicou em cima do titulo da categoria, entao apenas ignora e mantem a tela aberta
                    }
                    
                    bool submenuAberto = true;
                    while (submenuAberto) {
                        TipoEquipamento tipo = itemEncontrado->obterTipo();
                        std::vector<std::string> ops;
                        if (tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) {
                            ops = {"[+] Equipar / Desequipar", "[?] Inspecionar (Atributos, Requisitos, Lore)"};
                        } else if (tipo == TipoEquipamento::MISSAO) {
                            ops = {Aparencia::cor(Cor::CINZA) + "[!] Uso em missoes (Automatico)" + Aparencia::cor(Cor::RESET), "[?] Inspecionar (Lore)"};
                        } else if (tipo == TipoEquipamento::CONSUMIVEL) {
                            ops = {"[*] Usar item", "[+] Equipar no Acesso Rapido", "[?] Inspecionar Efeitos"};
                        } else {
                            ops = {Aparencia::cor(Cor::CINZA) + "[#] Levar para NPC (Forja/Mago)" + Aparencia::cor(Cor::RESET), "[?] Inspecionar Detalhes"};
                        }
                        ops.push_back("[<] Cancelar");
                        
                        Aparencia::iniciarInteracaoPopup();
                        int subOpcao = ControleDeInput::lerSelecaoMenuEmPopup(
                            "OPCOES DE ITEM", 
                            {"O que deseja fazer com:", Aparencia::cor(Cor::AMARELO) + ">> " + itemEncontrado->obterNomeItem() + " <<" + Aparencia::cor(Cor::RESET)}, 
                            ops, 
                            Cor::AMARELO
                        );

                        if (subOpcao == static_cast<int>(ops.size()) - 1 || subOpcao == -1) {
                            submenuAberto = false;
                            break;
                        }
                        
                        if (subOpcao == 0) {
                                int quantidadeParaUsar = 1;
                                if (tipo == TipoEquipamento::CONSUMIVEL) {
                                    int qtdDisponivel = jogadorAtual->obterInventario()->contarItem(itemEncontrado->obterNomeItem());
                                    if (qtdDisponivel > 1 && turnoFoiConsumido == nullptr) {
                                        std::vector<std::string> opcoesQtd = {
                                            "Usar 1 unidade",
                                            "Usar Todos (" + std::to_string(qtdDisponivel) + " unidades)",
                                            "Digitar quantidade...",
                                            "Cancelar"
                                        };
                                        
                                        int escolhaQtd = ControleDeInput::lerSelecaoMenuEmPopup(
                                            "QUANTIDADE: " + itemEncontrado->obterNomeItem(),
                                            {"Voce possui " + std::to_string(qtdDisponivel) + " unidades deste item."},
                                            opcoesQtd, 
                                            Cor::AMARELO
                                        );
                                        
                                        if (escolhaQtd == 0) {
                                            quantidadeParaUsar = 1;
                                        } else if (escolhaQtd == 1) {
                                            quantidadeParaUsar = qtdDisponivel;
                                        } else if (escolhaQtd == 2) {
                                            std::string msgQtd = "Quantidade (1 a " + std::to_string(qtdDisponivel) + ", 0 cancelar): ";
                                            quantidadeParaUsar = Aparencia::lerInteiroEmPopupFlutuante(msgQtd, 0, qtdDisponivel, Cor::AMARELO);
                                        } else {
                                            submenuAberto = false;
                                            break; // Cancelar
                                        }
                                    }
                                }
                                
                                if (quantidadeParaUsar == 0) {
                                    submenuAberto = false;
                                    break; // Cancelado
                                }

                                std::string nomeItem = itemEncontrado->obterNomeItem();
                                bool consumiuAlgumTurno = false;
                                
                                for (int i = 0; i < quantidadeParaUsar; ++i) {
                                    Item* itemAtual = nullptr;
                                    if (itemEncontrado->isEquipavel()) {
                                        if (jogadorAtual->isItemEquipado(itemEncontrado)) {
                                            itemAtual = itemEncontrado;
                                        } else {
                                            for (auto* it : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                                if (it->obterNomeItem() == nomeItem && !jogadorAtual->isItemEquipado(it)) {
                                                    itemAtual = it;
                                                    break;
                                                }
                                            }
                                        }
                                    } else {
                                        for (auto* it : jogadorAtual->obterInventario()->obterTodosOsItens()) {
                                            if (it->obterNomeItem() == nomeItem && !jogadorAtual->isItemEquipado(it)) {
                                                itemAtual = it;
                                                break;
                                            }
                                        }
                                        if (!itemAtual && jogadorAtual->isItemEquipado(itemEncontrado)) {
                                            itemAtual = itemEncontrado;
                                        }
                                    }
                                    
                                    if (itemAtual) {
                                        bool ehEquipavel = itemAtual->isEquipavel(); // Salva a informacao antes de consumir
                                        int countAntes = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                        bool turnoLocal = false;
                                        processarUsoDeItem(jogadorAtual, itemAtual, turnoFoiConsumido ? &turnoLocal : nullptr);
                                        if (turnoLocal) consumiuAlgumTurno = true;
                                        
                                        if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) {
                                            if (quantidadeParaUsar > 1) {
                                                std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema("Este item requer selecao de alvo e sera usado apenas uma vez.", Cor::AMARELO) << "\n";
                                                ControleDeInput::aguardarEnter();
                                            }
                                            break;
                                        }
                                        
                                        int countDepois = jogadorAtual->obterInventario()->contarItem(nomeItem);
                                        if (countDepois == countAntes && !ehEquipavel) {
                                            break;
                                        }
                                    }
                                }
                                
                                if (jogadorAtual->obterConsumivelRapido() && jogadorAtual->obterInventario()->contarItem(jogadorAtual->obterConsumivelRapido()->obterNomeItem()) == 0) {
                                    jogadorAtual->desequiparConsumivel();
                                }

                                if (turnoFoiConsumido && consumiuAlgumTurno) {
                                    *turnoFoiConsumido = true;
                                }
                                submenuAberto = false; // Sai do Submenu apos o uso
                                break;
                        } else if (subOpcao == 1 && tipo == TipoEquipamento::CONSUMIVEL) {
                                if (jogadorAtual->obterConsumivelRapido() == itemEncontrado) {
                                    jogadorAtual->desequiparConsumivel();
                                    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema(itemEncontrado->obterNomeItem() + " desequipado do acesso rapido!", Cor::AMARELO) << "\n";
                                } else {
                                    jogadorAtual->equiparItem(itemEncontrado);
                                    std::cout << "\n" << Aparencia::margemCombate() << FuncoesDialogo::formatarMsgSistema(itemEncontrado->obterNomeItem() + " equipado no acesso rapido!", Cor::VERDE) << "\n";
                                }
                                ControleDeInput::aguardarEnter();
                                submenuAberto = false;
                                break;
                        } else if ((subOpcao == 1 && tipo != TipoEquipamento::CONSUMIVEL) || (subOpcao == 2 && tipo == TipoEquipamento::CONSUMIVEL)) {
                                Aparencia::limparTela();
                                Aparencia::exibirPainelTexto("INSPECAO DE ITEM", Cor::AMARELO);
                                TelaInventario::exibirInspecaoItem(itemEncontrado, jogadorAtual);
                                std::cout << "\n";
                                ControleDeInput::aguardarEnter();
                                submenuAberto = false;
                                break;
                        }
                        
                        submenuAberto = false;
                        break;
                    } // Fim do while (submenuAberto)

                    if (turnoFoiConsumido && *turnoFoiConsumido) return false;
                    return true;
                }
            );
            if (turnoFoiConsumido && *turnoFoiConsumido) {
                return false;
            }
            return true;
        }
    );
}

void InventarioCombate::processarUsoDeItem(Personagem* jogadorAtual, Item* itemEncontrado, bool* turnoFoiConsumido)
{
    if (turnoFoiConsumido && *turnoFoiConsumido) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Voce ja usou um item neste turno!\n";
        ControleDeInput::aguardarEnter();
        return;
    }

    if (itemEncontrado->isEquipavel())
    {
        if (itemEncontrado->obterTipo() == TipoEquipamento::ESCUDO && itemEncontrado->obterDurabilidadeAtualEscudo() <= 0) {
            std::string msgQuebrado = FuncoesDialogo::formatarMsgSistema("O escudo [" + itemEncontrado->obterNomeItem() + "] esta quebrado e nao pode ser equipado!", Cor::VERMELHO);
            std::cout << "\n" << Aparencia::margemCombate() << msgQuebrado << "\n";
            if (turnoFoiConsumido && !jogadorAtual->obterItemSelecionadoParaUso()) ControleDeInput::aguardarEnter();
            return; // Retorna cedo, nao alterando 'turnoFoiConsumido' para true
        }

        bool desequipou = false;
        if (itemEncontrado == jogadorAtual->obterArma()) {
            jogadorAtual->desequiparArma();
            desequipou = true;
        } else if (itemEncontrado == jogadorAtual->obterEscudo()) {
            jogadorAtual->desequiparEscudo();
            desequipou = true;
        } else if (itemEncontrado == jogadorAtual->obterArmadura()) {
            jogadorAtual->desequiparArmadura();
            desequipou = true;
        }
        
        if (desequipou) {
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
        } else {
            if (!itemEncontrado->podeSerEquipadoPor(jogadorAtual)) {
                std::string msg = itemEncontrado->obterMensagemRequisito();
                if (msg.substr(0, 1) == "\n") std::cout << "\n" << Aparencia::margemCombate() << msg.substr(1);
                else std::cout << Aparencia::margemCombate() << msg;
                if (turnoFoiConsumido && !jogadorAtual->obterItemSelecionadoParaUso()) ControleDeInput::aguardarEnter();
                return;
            }
            jogadorAtual->equiparItem(itemEncontrado);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << itemEncontrado->obterNomeItem() << " equipado(a)!\n";
        }

        if (turnoFoiConsumido) {
            *turnoFoiConsumido = true;
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Turno gasto alterando um equipamento...\n";
        }
        ControleDeInput::aguardarEnter();
        return;
    }
    
    if (itemEncontrado->usarDoInventario(jogadorAtual, turnoFoiConsumido))
    {
        // Se o item for Debuff (que seta o ponteiro para pedir alvo), ignorar enter
        if (!jogadorAtual->obterItemSelecionadoParaUso()) {
            ControleDeInput::aguardarEnter();
        }
        return;
    }

    std::string msgErro;
    switch (itemEncontrado->obterTipo()) {
        case TipoEquipamento::MATERIAL:
            msgErro = "Materiais são utilizados para NPCs especializados.";
            break;
        case TipoEquipamento::MISSAO:
            msgErro = "Itens de missao sao ativados automaticamente no momento ou local certo ou na historia.";
            break;
        case TipoEquipamento::CONSUMIVEL:
            msgErro = "Este consumivel nao pode ser usado " + std::string(turnoFoiConsumido ? "no combate!" : "fora de combate!");
            break;
        default:
            msgErro = "Este item nao possui uso direto no inventario.";
            break;
    }

    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << msgErro << "\n";
    ControleDeInput::aguardarEnter();
}
