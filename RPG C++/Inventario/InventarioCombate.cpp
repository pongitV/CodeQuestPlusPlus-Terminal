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
                    else if (isBuff) 
                    {
                        if (!turnoFoiConsumido) 
                        {
                            std::cout << "\n[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n";
                            Menu::aguardarPressionamentoDeEnter();
                            continue;
                        }

                        std::string nomeDoItemEncontrado = itemEncontrado->obterNomeItem();
                        jogadorAtual->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
                        jogadorAtual->definirMultiplicador(1.5);
                        jogadorAtual->obterInventario()->removerItem(nomeDoItemEncontrado);
                        std::cout << "\n[SISTEMA]: " << nomeDoItemEncontrado << " consumida! Atributos ampliados em 1.5x por 2 turnos!\n";
                    }
                    else if (isTalisma)
                    {
                        std::string nomeTalisma = itemEncontrado->obterNomeItem();
                        if (itemEncontrado->temPropriedade(Propriedade::TalismaForca)) 
                        {
                            jogadorAtual->alterarAtributoEstatico("forca", 5);
                            jogadorAtual->alterarAtributoEstatico("inteligencia", -5);
                        } 
                        else if (itemEncontrado->temPropriedade(Propriedade::TalismaInteligencia)) 
                        {
                            jogadorAtual->alterarAtributoEstatico("inteligencia", 5);
                            jogadorAtual->alterarAtributoEstatico("forca", -5);
                        } 
                        else if (itemEncontrado->temPropriedade(Propriedade::TalismaDestreza)) 
                        {
                            jogadorAtual->alterarAtributoEstatico("destreza", 5);
                            jogadorAtual->alterarAtributoEstatico("sabedoria", -5);
                        } 
                        else if (itemEncontrado->temPropriedade(Propriedade::TalismaSabedoria)) 
                        {
                            jogadorAtual->alterarAtributoEstatico("sabedoria", 5);
                            jogadorAtual->alterarAtributoEstatico("destreza", -5);
                        }
                        std::cout << "\n[SISTEMA]: " << nomeTalisma << " consumido!\n";
                        jogadorAtual->obterInventario()->removerItem(nomeTalisma);
                    }
                    else if (isPocao) 
                    {
                        if (jogadorAtual->obterVida() >= jogadorAtual->obterVidaMaxima()) 
                        {
                            std::cout << "\n[SISTEMA]: Sua vida ja esta cheia!\n";
                            Menu::aguardarPressionamentoDeEnter();
                            continue;
                        }

                        std::string nomeDoItemEncontrado = itemEncontrado->obterNomeItem(); 
                        int quantidadeDeCura = static_cast<int>(jogadorAtual->obterVidaMaxima() * 0.30);
                        
                        jogadorAtual->modificarVida(quantidadeDeCura); 
                        jogadorAtual->obterInventario()->removerItem(nomeDoItemEncontrado);
                        
                        std::cout << "\n[SISTEMA]: " << nomeDoItemEncontrado << " usada! +" << quantidadeDeCura << " HP.\n";
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
