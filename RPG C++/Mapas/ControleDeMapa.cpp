#include "ControleDeMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Gerenciadores/GerenciadorCombate.h"
#include "../Utilidades/ControleDeInput.h"
#include <iostream>
#include "../Inventario/FabricaItens.h"

bool ControleDeMapa::processarInputEComandos(char tecla, SistemaPersonagem* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    // --- MENU DE DEBUG (ISOLADO PARA FACIL REMOCAO FUTURA) ---
    if (tecla == '\\' || tecla == '`' || tecla == '=')
    {
        while(true) {
            SimplificacoesAparencia::limparTela();
            TelaMenu::exibirLogoDoJogo("MENU DE DEBUG (CHEAT)");
            std::cout << "\n  [1] God Mode (Max Atributos - Instakill/Imortal)\n";
            std::cout << "  [2] Obter Qualquer Item\n";
            std::cout << "  [3] Adicionar Ouro e XP (+10000)\n";
            std::cout << "  [4] Noclip (Atravessar paredes): " << (jogador->isNoclip() ? SimplificacoesAparencia::cor(Cor::VERDE) + "LIGADO" : SimplificacoesAparencia::cor(Cor::VERMELHO) + "DESLIGADO") << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            std::cout << "  [0] Fechar Debug Menu\n\n  Escolha: ";
            
            std::string escolhaDebug;
            std::cin >> escolhaDebug;
            
            if (escolhaDebug == "1") {
                jogador->obterAtributosFinais().vida += 999999;
                jogador->obterAtributosFinais().forca += 99999;
                jogador->obterAtributosFinais().destreza += 99999;
                jogador->obterAtributosFinais().resistencia += 99999;
                jogador->obterAtributosFinais().constituicao += 99999;
                jogador->obterAtributosFinais().inteligencia += 99999;
                jogador->obterAtributosFinais().sabedoria += 99999;
                jogador->forcarRecalculoCache();
                jogador->definirVida(jogador->obterVidaMaxima());
                std::cout << "\n[SISTEMA] God Mode ativado! Voce agora e um deus intocavel.\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (escolhaDebug == "2") {
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
                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("OBTER ITEM");
                for(size_t i=0; i<todosItens.size(); ++i) {
                    std::cout << "[" << i+1 << "] " << FabricaItens::obterNomeDeID(todosItens[i]) << "\n";
                }
                std::cout << "[0] Voltar\n\nEscolha o ID do item: ";
                int escolhaID;
                if(std::cin >> escolhaID && escolhaID > 0 && escolhaID <= (int)todosItens.size()) {
                    ItemID idEscolhido = todosItens[escolhaID-1];
                    jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idEscolhido));
                    std::cout << "\n[SISTEMA] Item '" << FabricaItens::obterNomeDeID(idEscolhido) << "' adicionado ao inventario!\n";
                    SimplificacoesAparencia::aguardarEnter();
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }
            } else if (escolhaDebug == "3") {
                jogador->ganharOuro(10000);
                jogador->ganharXp(10000);
                std::cout << "\n[SISTEMA] +10000 Ouro e +10000 XP adicionados!\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (escolhaDebug == "4") {
                jogador->alternarNoclip();
                std::cout << "\n[SISTEMA] Noclip " << (jogador->isNoclip() ? "ativado" : "desativado") << "!\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (escolhaDebug == "0") { break; }
        }
        restaurarTela();
        return true;
    }
    // --- FIM DO MENU DE DEBUG ---

    ComandoMapa comando = ControleDeInput::traduzirTeclaParaComando(tecla);

    // Movimentação (Não abre menus, portanto retorna falso)
    if (comando == ComandoMapa::Cima) { proximaPosicaoY--; return false; }
    if (comando == ComandoMapa::Baixo) { proximaPosicaoY++; return false; }
    if (comando == ComandoMapa::Esquerda) { proximaPosicaoX--; return false; }
    if (comando == ComandoMapa::Direita) { proximaPosicaoX++; return false; }

    // Menus (Retornam true informando o mapa que a tela precisa pular a interação de movimento)
    if (comando == ComandoMapa::Inventario)
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Ficha)
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Bestiario)
    {
        TelaBestiario::exibirLista(jogador);
        restaurarTela();
        return true;
    }
    return false;
}

void ControleDeMapa::aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa) {
    if (posicaoY < 0) posicaoY = 0; else if (posicaoY >= static_cast<int>(matrizDoMapa.size())) posicaoY = static_cast<int>(matrizDoMapa.size()) - 1;
    if (posicaoX < 0) posicaoX = 0; else if (posicaoX >= static_cast<int>(matrizDoMapa[0].size())) posicaoX = static_cast<int>(matrizDoMapa[0].size()) - 1;
}

void ControleDeMapa::processarCombate(
    SistemaPersonagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& tituloDoCombate, const std::string& mensagemDeAviso, std::vector<std::unique_ptr<SistemaPersonagem>> inimigosParaBatalha, 
    int posicaoXAposCombate, int posicaoYAposCombate, int posicaoXInicialDoInimigo, int quantidadeDeCelulasOcupadas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo(tituloDoCombate);
    int espacosParaCentralizarMensagem = std::max(0, (larguraDoTerminal - static_cast<int>(mensagemDeAviso.length())) / 2);
    std::string margemEsquerdaMensagem(espacosParaCentralizarMensagem, ' ');
    std::cout << "\n" << margemEsquerdaMensagem << SimplificacoesAparencia::cor(Cor::AMARELO) << "[!] " << mensagemDeAviso << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << margemEsquerdaMensagem << "[0] Nao, recuar | [1] Sim, batalha!\n" << margemEsquerdaMensagem << "Escolha: ";

    int opcaoEscolhidaPeloJogador;
    while (!(std::cin >> opcaoEscolhidaPeloJogador) || (opcaoEscolhidaPeloJogador != 0 && opcaoEscolhidaPeloJogador != 1)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << margemEsquerdaMensagem << "Entrada invalida. Escolha (0 ou 1): ";
    }

    if (opcaoEscolhidaPeloJogador == 1) {
        GerenciadorCombate combate(jogadorAtual, std::move(inimigosParaBatalha));
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < quantidadeDeCelulasOcupadas; ++i) matrizDoMapaAtual[posicaoYAposCombate][posicaoXInicialDoInimigo + i] = '.';
            posicaoXDoJogador = posicaoXAposCombate;
            posicaoYDoJogador = posicaoYAposCombate;
        }
    }

    if (exploracaoEstaAtiva) restaurarTela();
}

void ControleDeMapa::entrarSubMapa(
    std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
    int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
    std::string& tituloDoMapaAtual, std::vector<std::string>& matrizDoSubMapaSalva, bool& subMapaJaFoiVisitado,
    const std::vector<std::string>& matrizDoSubMapaGerada, int posicaoXInicialNoSubMapa, int posicaoYInicialNoSubMapa, const std::string& tituloDoSubMapa, const std::function<void()>& restaurarTela)
{
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
    posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
    posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

    if (!subMapaJaFoiVisitado) { matrizDoMapaAtual = matrizDoSubMapaGerada; subMapaJaFoiVisitado = true; } 
    else { matrizDoMapaAtual = matrizDoSubMapaSalva; }

    posicaoXDoJogador = posicaoXInicialNoSubMapa;
    posicaoYDoJogador = posicaoYInicialNoSubMapa;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = tituloDoSubMapa;
    restaurarTela();
}
