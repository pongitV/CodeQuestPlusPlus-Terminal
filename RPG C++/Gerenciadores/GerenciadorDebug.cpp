#include "GerenciadorDebug.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/ControleDeInput.h"

namespace {
    void ativarGodMode(SistemaPersonagem* jogador) {
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
        ControleDeInput::aguardarEnter();
    }

    void menuDefinirAtributos(SistemaPersonagem* jogador) {
        while (true) {
            Aparencia::limparTela();
            Aparencia::exibirPainelTexto("DEFINIR ATRIBUTOS (CHEAT)", Cor::AMARELO);
            
            auto& attrs = jogador->obterAtributosFinais();
            std::vector<std::string> opcoesAtr = {
                "Vida Maxima  : " + std::to_string(attrs.vida), "Forca        : " + std::to_string(attrs.forca),
                "Destreza     : " + std::to_string(attrs.destreza), "Resistencia  : " + std::to_string(attrs.resistencia),
                "Constituicao : " + std::to_string(attrs.constituicao), "Inteligencia : " + std::to_string(attrs.inteligencia),
                "Sabedoria    : " + std::to_string(attrs.sabedoria), "Voltar"
            };
            
            int escolhaAtr = ControleDeInput::lerSelecaoMenuComSetas(opcoesAtr, true);
            if (escolhaAtr == 7 || escolhaAtr == -1) break;
            
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
        }
    }

    void menuObterItem(SistemaPersonagem* jogador) {
        static const std::vector<ItemID> todosItens = {
            ItemID::AdagaPedra, ItemID::ArcoMadeira, ItemID::CajadoCristal, ItemID::VarinhaCorroida, ItemID::ViolaoEncantado,
            ItemID::EspadaFerro, ItemID::MachadoGuerra, ItemID::GosmaAcidaArma, ItemID::TroncoAmarrotado, ItemID::EspadaExterminio,
            ItemID::EscudoMetal, ItemID::BarreiraMagica, ItemID::CapaMagica, ItemID::BracedeirasPrata,
            ItemID::ArmaduraMalha, ItemID::ArmaduraCouro, ItemID::Tunica, ItemID::TrajeNobre, ItemID::ArmaduraTrapos, ItemID::ArmaduraCavaleiro, ItemID::ArmaduraBau, ItemID::RodaAdaptacao,
            ItemID::PocaoCura30, ItemID::PocaoFuria, ItemID::ElixirArcano, ItemID::FrascoGosma, ItemID::FrascoFraqueza, ItemID::OrgaoRegenerador,
            ItemID::TalismaUrso, ItemID::TalismaCorvo, ItemID::TalismaLeopardo, ItemID::TalismaCoruja,
            ItemID::GosmaAcida, ItemID::DenteGoblin, ItemID::NucleoPegajoso, ItemID::PoMagico, ItemID::MadeiraEnfeiticada, ItemID::CoracaoFloresta, ItemID::PedraUpgrade, ItemID::ConviteReal,
            ItemID::DispositivoLinguagem
        };
        
        while (true) {
            Aparencia::limparTela();
            Aparencia::exibirPainelTexto("OBTER ITEM", Cor::AMARELO);
            
            int limit = (todosItens.size() + 1) / 2;
            std::cout << "\n";
            for (int i = 0; i < limit; ++i) {
                std::string col1 = "[" + std::to_string(i + 1) + "] " + FabricaItens::obterNomeDeID(todosItens[i]);
                std::string col2 = "";
                if (i + limit < (int)todosItens.size()) { col2 = "[" + std::to_string(i + limit + 1) + "] " + FabricaItens::obterNomeDeID(todosItens[i + limit]); }
                std::cout << "      " << std::left << std::setw(45) << col1 << col2 << "\n";
            }
            std::cout << "\n  [0] Voltar\n\n";
            
            int escolhaID = ControleDeInput::lerInteiroComLimites("  Escolha o ID do item: ", 0, todosItens.size());
            if (escolhaID == 0) break;
            
            int quantidade = ControleDeInput::lerInteiroComLimites("  Quantidade: ", 1, 999);
            ItemID idEscolhido = todosItens[escolhaID - 1];
            
            for (int q = 0; q < quantidade; ++q) jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idEscolhido));
            
            std::cout << "\n  " << Aparencia::cor(Cor::AMARELO) << "[SISTEMA] " << quantidade << "x '" << FabricaItens::obterNomeDeID(idEscolhido) << "' adicionado(s) ao inventario!" << Aparencia::cor(Cor::RESET) << "\n";
            ControleDeInput::aguardarEnter();
        }
    }
}

void GerenciadorDebug::exibirMenuDebug(SistemaPersonagem* jogador) {
    bool primeiraVez = true;
    while (true) {
        Aparencia::limparTela();
        Aparencia::exibirPainelTexto("MENU DE DEBUG (CHEAT)", Cor::AMARELO, primeiraVez);
        primeiraVez = false;
        
        std::vector<std::string> opcoesDebug = {
            "God Mode (Max Atributos - Instakill/Imortal)",
            "Definir Atributos Livres",
            "Obter Qualquer Item",
            "Adicionar Ouro e XP (+10000)",
            std::string("Noclip (Atravessar paredes): ") + (jogador->isNoclip() ? Aparencia::cor(Cor::VERDE) + "LIGADO" + Aparencia::cor(Cor::RESET) : Aparencia::cor(Cor::VERMELHO) + "DESLIGADO" + Aparencia::cor(Cor::RESET)),
            "Fechar Debug Menu"
        };

        std::cout << "\n";
        int escolhaDebug = ControleDeInput::lerSelecaoMenuComSetas(opcoesDebug, true);
        
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
            Aparencia::imprimirCentralizado("[SISTEMA] +10000 Ouro e +10000 XP adicionados!", Aparencia::cor(Cor::AMARELO));
            std::cout << "\n";
            ControleDeInput::aguardarEnter();
            
        } else if (escolhaDebug == 4) {
            jogador->alternarNoclip();
        } else if (escolhaDebug == 5 || escolhaDebug == -1) { 
            break; 
        }
    }
}
