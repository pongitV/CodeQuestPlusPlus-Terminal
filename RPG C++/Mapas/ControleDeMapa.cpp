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
            
            std::string esc;
            std::cin >> esc;
            
            if (esc == "1") {
                jogador->obterAtributosFinais().vida += 999999;
                jogador->obterAtributosFinais().forca += 99999;
                jogador->obterAtributosFinais().destreza += 99999;
                jogador->obterAtributosFinais().resistencia += 99999;
                jogador->obterAtributosFinais().constituicao += 99999;
                jogador->obterAtributosFinais().inteligencia += 99999;
                jogador->obterAtributosFinais().sabedoria += 99999;
                jogador->definirVida(jogador->obterVidaMaxima());
                std::cout << "\n[SISTEMA] God Mode ativado! Voce agora e um deus intocavel.\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (esc == "2") {
                std::vector<std::string> todosItens = {
                    "Adaga artesanal de pedra", "Arco recurvo de madeira", "Cajado de cristal magico", "Varinha corroida", "Violao encantado",
                    "Espada longa de ferro", "Machado de guerra danificado", "Gosma acida (Arma)", "Tronco de arvore amarrotado",
                    "Escudo medio de metal", "Barreira magica", "Capa magica", "Bracedeiras de prata",
                    "Armadura de malha e metal", "Armadura leve de couro com malha", "Tunica", "Traje de Couro e tecido nobre", "Armadura de trapos e sucata",
                    "Pocao de Cura (30%VM)", "Pocao de Furia (Buff)", "Elixir Arcano (Buff)", "Frasco de Gosma (Debuff)", "Frasco de Fraqueza (Debuff)", "Orgao regenerador",
                    "Talisma do Urso", "Talisma do Corvo", "Talisma do Leopardo", "Talisma da Coruja",
                    "Gosma acida", "Dente de goblin", "Nucleo pegajoso", "Po magico", "Madeira enfeiticada", "Coracao da floresta", "Pedra magica de upgrade", "Gema de amolar", "Convite Real",
                    "Dispositivo de teclas de linguagem desconhecida"
                };
                SimplificacoesAparencia::limparTela();
                TelaMenu::exibirLogoDoJogo("OBTER ITEM");
                for(size_t i=0; i<todosItens.size(); ++i) {
                    std::cout << "[" << i+1 << "] " << todosItens[i] << "\n";
                }
                std::cout << "[0] Voltar\n\nEscolha o ID do item: ";
                int itemID;
                if(std::cin >> itemID && itemID > 0 && itemID <= (int)todosItens.size()) {
                    jogador->obterInventario()->adicionarItem(FabricaItens::criarItem(todosItens[itemID-1]));
                    std::cout << "\n[SISTEMA] Item '" << todosItens[itemID-1] << "' adicionado ao inventario!\n";
                    SimplificacoesAparencia::aguardarEnter();
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); }
            } else if (esc == "3") {
                jogador->ganharOuro(10000);
                jogador->ganharXp(10000);
                std::cout << "\n[SISTEMA] +10000 Ouro e +10000 XP adicionados!\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (esc == "4") {
                jogador->alternarNoclip();
                std::cout << "\n[SISTEMA] Noclip " << (jogador->isNoclip() ? "ativado" : "desativado") << "!\n";
                SimplificacoesAparencia::aguardarEnter();
            } else if (esc == "0") { break; }
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

void ControleDeMapa::processarCombate(
    SistemaPersonagem* jogadorAtual, std::vector<std::string>& matrizDoMapaAtual, 
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& exploracaoEstaAtiva,
    const std::string& titulo, const std::string& msg, std::vector<std::unique_ptr<SistemaPersonagem>> inimigos, 
    int px, int py, int rootX, int celulas, int larguraDoTerminal, const std::function<void()>& restaurarTela)
{
    SimplificacoesAparencia::limparTela();
    TelaMenu::exibirLogoDoJogo(titulo);
    int espacosM = std::max(0, (larguraDoTerminal - static_cast<int>(msg.length())) / 2);
    std::string mE(espacosM, ' ');
    std::cout << "\n" << mE << SimplificacoesAparencia::cor(Cor::AMARELO) << "[!] " << msg << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    std::cout << mE << "[0] Nao, recuar | [1] Sim, batalha!\n" << mE << "Escolha: ";

    int opcao;
    while (!(std::cin >> opcao) || (opcao != 0 && opcao != 1)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << mE << "Entrada invalida. Escolha (0 ou 1): ";
    }

    if (opcao == 1) {
        GerenciadorCombate combate(jogadorAtual, std::move(inimigos));
        combate.iniciarCombate();

        if (jogadorAtual->obterVida() > 0) {
            for (int i = 0; i < celulas; ++i) matrizDoMapaAtual[py][rootX + i] = '.';
            posicaoXDoJogador = px;
            posicaoYDoJogador = py;
        }
    }

    if (exploracaoEstaAtiva) restaurarTela();
}

void ControleDeMapa::entrarSubMapa(
    std::vector<std::string>& matrizDoMapaAtual, std::vector<std::string>& matrizDoMapaPrincipalSalva,
    int& posicaoXSalvaAntesDeEntrarNoSubMapa, int& posicaoYSalvaAntesDeEntrarNoSubMapa,
    int& posicaoXDoJogador, int& posicaoYDoJogador, bool& jogadorEstaDentroDeUmSubMapa,
    std::string& tituloDoMapaAtual, std::vector<std::string>& mapaSalvo, bool& jaVisitado,
    const std::vector<std::string>& mapaGerado, int spawnX, int spawnY, const std::string& titulo, const std::function<void()>& restaurarTela)
{
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual;
    posicaoXSalvaAntesDeEntrarNoSubMapa = posicaoXDoJogador;
    posicaoYSalvaAntesDeEntrarNoSubMapa = posicaoYDoJogador;

    if (!jaVisitado) { matrizDoMapaAtual = mapaGerado; jaVisitado = true; } 
    else { matrizDoMapaAtual = mapaSalvo; }

    posicaoXDoJogador = spawnX;
    posicaoYDoJogador = spawnY;
    jogadorEstaDentroDeUmSubMapa = true;
    tituloDoMapaAtual = titulo;
    restaurarTela();
}
