#include "ControleDeMapa.h"
#include "../Inventario/InventarioCombate.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaBestiario.h"
#include "../Telas/TelaMenu.h"
#include "../Utilidades/Aparencia.h"
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
            Aparencia::limparTela();
            Aparencia::exibirCabecalho("MENU DE DEBUG (CHEAT)", Cor::AMARELO);
            std::cout << "\n  [1] God Mode (Max Atributos - Instakill/Imortal)\n";
            std::cout << "  [2] Obter Qualquer Item\n";
            std::cout << "  [3] Adicionar Ouro e XP (+10000)\n";
            std::cout << "  [4] Noclip (Atravessar paredes): " << (jogador->isNoclip() ? Aparencia::cor(Cor::VERDE) + "LIGADO" : Aparencia::cor(Cor::VERMELHO) + "DESLIGADO") << Aparencia::cor(Cor::RESET) << "\n";
            std::cout << "  [0] Fechar Debug Menu\n\n  Escolha: ";
            
            std::string escolhaDebug = ControleDeInput::lerEntradaProtegida();
            
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
                Aparencia::aguardarEnter();
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
                while (true) {
                    Aparencia::limparTela();
                    Aparencia::exibirCabecalho("OBTER ITEM", Cor::AMARELO);
                    for(size_t i=0; i<todosItens.size(); ++i) {
                        std::cout << "[" << i+1 << "] " << FabricaItens::obterNomeDeID(todosItens[i]) << "\n";
                    }
                    std::cout << "[0] Voltar\n\nEscolha o ID do item: ";
                
                std::string entradaID = ControleDeInput::lerEntradaProtegida();
                int escolhaID = -1;
                try { escolhaID = std::stoi(entradaID); } catch (...) {}
                
                if(escolhaID != -1) {
                        if (escolhaID == 0) break;
                        if (escolhaID > 0 && escolhaID <= (int)todosItens.size()) {
                            std::cout << "Quantidade: ";
                        std::string entradaQtd = ControleDeInput::lerEntradaProtegida();
                        int quantidade = -1;
                        try { quantidade = std::stoi(entradaQtd); } catch (...) {}
                        
                        if (quantidade > 0) {
                                ItemID idEscolhido = todosItens[escolhaID-1];
                                for (int q = 0; q < quantidade; ++q) {
                                    jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(idEscolhido));
                                }
                                std::cout << "\n[SISTEMA] " << quantidade << "x '" << FabricaItens::obterNomeDeID(idEscolhido) << "' adicionado(s) ao inventario!\n";
                                Aparencia::aguardarEnter();
                            }
                        }
                }
                }
            } else if (escolhaDebug == "3") {
                jogador->ganharOuro(10000);
                jogador->ganharXp(10000);
                std::cout << "\n[SISTEMA] +10000 Ouro e +10000 XP adicionados!\n";
                Aparencia::aguardarEnter();
            } else if (escolhaDebug == "4") {
                jogador->alternarNoclip();
                std::cout << "\n[SISTEMA] Noclip " << (jogador->isNoclip() ? "ativado" : "desativado") << "!\n";
                Aparencia::aguardarEnter();
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
    Aparencia::limparTela();
    Aparencia::exibirCabecalho(tituloDoCombate, Cor::VERMELHO);
    int espacosParaCentralizarMensagem = std::max(0, (larguraDoTerminal - static_cast<int>(mensagemDeAviso.length())) / 2);
    std::string margemEsquerdaMensagem(espacosParaCentralizarMensagem, ' ');
    std::cout << "\n" << margemEsquerdaMensagem << Aparencia::cor(Cor::AMARELO) << "[!] " << mensagemDeAviso << Aparencia::cor(Cor::RESET) << "\n";
    int opcaoEscolhidaPeloJogador = ControleDeInput::lerInteiroComLimites("[0] Nao, recuar | [1] Sim, batalha!\n" + margemEsquerdaMensagem + "Escolha: ", 0, 1, false, margemEsquerdaMensagem);

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
    padronizarTamanhoDoMapa(matrizDoMapaAtual);

    posicaoXDoJogador = posicaoXInicialNoSubMapa;
    posicaoYDoJogador = posicaoYInicialNoSubMapa;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = tituloDoSubMapa;
    restaurarTela();
}

void ControleDeMapa::calcularCameraVertical(int alturaDoTerminal, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = alturaDoTerminal - 7;
    if (maxLinhasVisiveis < 5) maxLinhasVisiveis = 5;
    startY = 0;
    endY = tamanhoDoMapa;

    if (endY > maxLinhasVisiveis) {
        startY = posicaoYDoJogador - (maxLinhasVisiveis / 2);
        if (startY < 0) startY = 0;
        endY = startY + maxLinhasVisiveis;
        if (endY > tamanhoDoMapa) {
            endY = tamanhoDoMapa;
            startY = endY - maxLinhasVisiveis;
            if (startY < 0) startY = 0;
        }
    }
}

void ControleDeMapa::calcularCameraHorizontal(int larguraDoTerminal, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX) {
    int maxColunasVisiveis = larguraDoTerminal - 1; // -1 para evitar quebras de linha acidentais
    if (maxColunasVisiveis < 10) maxColunasVisiveis = 10;
    startX = 0;
    endX = larguraDoMapa;

    if (endX > maxColunasVisiveis) {
        startX = posicaoXDoJogador - (maxColunasVisiveis / 2);
        if (startX < 0) startX = 0;
        endX = startX + maxColunasVisiveis;
        if (endX > larguraDoMapa) {
            endX = larguraDoMapa;
            startX = endX - maxColunasVisiveis;
            if (startX < 0) startX = 0;
        }
    }
}

std::string ControleDeMapa::calcularMargemCentralizada(int larguraDoTerminal, int larguraDoTexto) {
    int espacos = (larguraDoTerminal - larguraDoTexto) / 2;
    return std::string(espacos > 0 ? espacos : 0, ' ');
}

void ControleDeMapa::padronizarTamanhoDoMapa(std::vector<std::string>& matrizDoMapa) {
    size_t maxLength = 0;
    for (const auto& linha : matrizDoMapa) {
        if (linha.length() > maxLength) maxLength = linha.length();
    }
    for (auto& linha : matrizDoMapa) {
        if (linha.length() < maxLength) linha.append(maxLength - linha.length(), ' ');
    }
}
