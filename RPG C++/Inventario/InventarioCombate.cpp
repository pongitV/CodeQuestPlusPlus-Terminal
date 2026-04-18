#include <iostream>
#include <string>
#include <vector>

#include "InventarioCombate.h"
#include "../Sistema/Personagem.h"
#include "Item.h"
#include "../Interfaces/TelaInventario.h"
#include "../Sistema/Menu.h"

void InventarioCombate::gerenciarInventario(Personagem* jogadorAtual, bool* turnoFoiConsumido)
{
    if (jogadorAtual == nullptr) return;
    int larguraDoTerminal = Menu::obterLarguraDoTerminalEmColunas();
    std::string codigoDoItemDigitado;
    do 
    {
        TelaInventario::exibir(jogadorAtual);
        std::string mensagemDoPrompt = "Digite o codigo do item ou [0] VOLTAR: ";
        int espacosParaCentralizarMensagem = (larguraDoTerminal - (int)mensagemDoPrompt.length()) / 2;
        std::cout << "\n" << std::string(espacosParaCentralizarMensagem > 0 ? espacosParaCentralizarMensagem : 0, ' ') << mensagemDoPrompt;
        std::cin >> codigoDoItemDigitado;

        if (codigoDoItemDigitado != "0")
        {
            Item* itemEncontrado = jogadorAtual->obterInventario()->buscarItemPorCodigo(
                codigoDoItemDigitado, jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura()
            );

            if (itemEncontrado)
            {
                bool ehEquipamento = (itemEncontrado->obterTipo() == TipoEquipamento::ARMA || 
                                      itemEncontrado->obterTipo() == TipoEquipamento::ESCUDO || 
                                      itemEncontrado->obterTipo() == TipoEquipamento::ARMADURA);
                bool isPocao = itemEncontrado->temPropriedade(Propriedade::ConsumivelCura);
                bool isBuff = itemEncontrado->temPropriedade(Propriedade::ConsumivelBuff);
                bool isDebuff = itemEncontrado->temPropriedade(Propriedade::ConsumivelDebuffLentidao) || itemEncontrado->temPropriedade(Propriedade::ConsumivelDebuffFraqueza);
                bool isTalisma = itemEncontrado->temPropriedade(Propriedade::TalismaForca) || itemEncontrado->temPropriedade(Propriedade::TalismaInteligencia) || 
                                 itemEncontrado->temPropriedade(Propriedade::TalismaDestreza) || itemEncontrado->temPropriedade(Propriedade::TalismaSabedoria);

                if (isPocao || isBuff || isDebuff || isTalisma || itemEncontrado == jogadorAtual->obterArma() || itemEncontrado == jogadorAtual->obterEscudo() || itemEncontrado == jogadorAtual->obterArmadura() || ehEquipamento)
                {
                    if (turnoFoiConsumido && *turnoFoiConsumido) 
                    {
                        std::cout << "\n[SISTEMA]: Voce ja usou um item neste turno!\n";
                        Menu::aguardarPressionamentoDeEnter();
                        continue;
                    }

                    if (isDebuff) 
                    {
                        if (turnoFoiConsumido) 
                        {
                            jogadorAtual->definirItemSelecionadoParaUso(itemEncontrado);
                            break; // Sai do inventario para abrir o alvo
                        } 
                        else 
                        {
                            std::cout << "\n[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n";
                            Menu::aguardarPressionamentoDeEnter();
                            continue;
                        }
                    }
                    else if (isBuff || isTalisma || isPocao) 
                    {
                        if (isBuff && !turnoFoiConsumido) {
                            std::cout << "\n[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n";
                            Menu::aguardarPressionamentoDeEnter();
                            continue;
                        }
                        
                        bool sucesso = itemEncontrado->aoUsar(*jogadorAtual);
                        if (sucesso) {
                            jogadorAtual->obterInventario()->removerItem(itemEncontrado->obterNomeItem());
                            if (turnoFoiConsumido) *turnoFoiConsumido = true;
                        }
                    } 
                    else if (itemEncontrado == jogadorAtual->obterArma()) 
                    {
                        jogadorAtual->desequiparArma();
                        std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
                    } 
                    else if (itemEncontrado == jogadorAtual->obterEscudo()) 
                    {
                        jogadorAtual->desequiparEscudo();
                        std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
                    } 
                    else if (itemEncontrado == jogadorAtual->obterArmadura()) 
                    {
                        jogadorAtual->desequiparArmadura();
                        std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " desequipado(a)!\n";
                    } 
                    else 
                    {
                        jogadorAtual->equiparItem(itemEncontrado);
                        std::cout << "\n[SISTEMA]: " << itemEncontrado->obterNomeItem() << " equipado(a)!\n";
                    }
                    
                    if (turnoFoiConsumido) 
                    {
                        *turnoFoiConsumido = true;
                        std::cout << "[SISTEMA]: Turno gasto usando um consumivel...\n";
                    }
                    Menu::aguardarPressionamentoDeEnter();
                }
                else 
                {
                    std::cout << "\n[SISTEMA]: Este item nao pode ser usado " << (turnoFoiConsumido ? "em combate!" : "fora de combate!") << "\n";
                    Menu::aguardarPressionamentoDeEnter();
                }
            }
        }
    } while (codigoDoItemDigitado != "0");
}
