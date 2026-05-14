#include "GerenciadorDebug.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"

void GerenciadorDebug::exibirMenuDebug(SistemaPersonagem* jogador) {
    while (true) {
        Aparencia::limparTela();
        Aparencia::exibirCabecalho("MENU DE DEBUG (CHEAT)", Cor::AMARELO);
        
        std::vector<std::string> opcoesDebug = {
            "God Mode (Max Atributos - Instakill/Imortal)",
            "Obter Qualquer Item",
            "Adicionar Ouro e XP (+10000)",
            std::string("Noclip (Atravessar paredes): ") + (jogador->isNoclip() ? Aparencia::cor(Cor::VERDE) + "LIGADO" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::VERMELHO) + "DESLIGADO" + Aparencia::cor(Cor::RESET)),
            "Fechar Debug Menu"
        };

        std::cout << "\n";
        int escolhaDebug = ControleDeInput::lerSelecaoMenuComSetas(opcoesDebug, true);
        
        if (escolhaDebug == 0) {
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
            Aparencia::imprimirCentralizado("[SISTEMA] God Mode ativado! Voce agora e um deus intocavel.", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
            Aparencia::aguardarEnter();
            
        } else if (escolhaDebug == 1) {
            static const std::vector<ItemID> todosItens = {
                ItemID::AdagaPedra, ItemID::ArcoMadeira, ItemID::CajadoCristal, ItemID::VarinhaCorroida, ItemID::ViolaoEncantado,
                ItemID::EspadaFerro, ItemID::MachadoGuerra, ItemID::GosmaAcidaArma, ItemID::TroncoAmarrotado,
                ItemID::EscudoMetal, ItemID::BarreiraMagica, ItemID::CapaMagica, ItemID::BracedeirasPrata,
                ItemID::ArmaduraMalha, ItemID::ArmaduraCouro, ItemID::Tunica, ItemID::TrajeNobre, ItemID::ArmaduraTrapos,
                ItemID::PocaoCura30, ItemID::PocaoFuria, ItemID::ElixirArcano, ItemID::FrascoGosma, ItemID::FrascoFraqueza, ItemID::OrgaoRegenerador,
                ItemID::TalismaUrso, ItemID::TalismaCorvo, ItemID::TalismaLeopardo, ItemID::TalismaCoruja,
                ItemID::GosmaAcida, ItemID::DenteGoblin, ItemID::NucleoPegajoso, ItemID::PoMagico, ItemID::MadeiraEnfeiticada, ItemID::CoracaoFloresta, ItemID::PedraUpgrade, ItemID::ConviteReal,
                ItemID::DispositivoLinguagem
            };
            
            while (true) {
                Aparencia::limparTela();
                Aparencia::exibirCabecalho("OBTER ITEM", Cor::AMARELO);
                
                int limit = (todosItens.size() + 1) / 2;
                std::cout << "\n";
                for (int i = 0; i < limit; ++i) {
                    std::string col1 = "[" + std::to_string(i + 1) + "] " + FabricaItens::obterNomeDeID(todosItens[i]);
                    std::string col2 = "";
                    if (i + limit < (int)todosItens.size()) {
                        col2 = "[" + std::to_string(i + limit + 1) + "] " + FabricaItens::obterNomeDeID(todosItens[i + limit]);
                    }
                    
                    std::cout << "      " << std::left << std::setw(45) << col1 << col2 << "\n";
                }
                std::cout << "\n  [0] Voltar\n\n";
                
                int escolhaID = ControleDeInput::lerInteiroComLimites("  Escolha o ID do item: ", 0, todosItens.size());
                if (escolhaID == 0) break;
                
                int quantidade = ControleDeInput::lerInteiroComLimites("  Quantidade: ", 1, 999);
                ItemID idEscolhido = todosItens[escolhaID - 1];
                
                for (int q = 0; q < quantidade; ++q) {
                    jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idEscolhido));
                }
                
                std::cout << "\n  " << Aparencia::cor(Cor::AMARELO) << "[SISTEMA] " << quantidade << "x '" << FabricaItens::obterNomeDeID(idEscolhido) << "' adicionado(s) ao inventario!" << Aparencia::cor(Cor::RESET) << "\n";
                Aparencia::aguardarEnter();
            }
            
        } else if (escolhaDebug == 2) {
            jogador->ganharOuro(10000);
            jogador->ganharXp(10000);
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[SISTEMA] +10000 Ouro e +10000 XP adicionados!", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
            Aparencia::aguardarEnter();
            
        } else if (escolhaDebug == 3) {
            jogador->alternarNoclip();
        } else if (escolhaDebug == 4 || escolhaDebug == -1) { 
            break; 
        }
    }
}
