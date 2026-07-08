#include "GerenciadorDeEstados.h"
#include "../../Entidades/Classes/Arqueiro.h"
#include "../../Entidades/Classes/Bardo.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Entidades/Classes/Guerreiro.h"
#include "../../Entidades/Classes/Mago.h"
#include "MenuJogo.h"
#include "../../Mapas/Vila/Mapa1Vila.h"
#include "../../Mapas/Floresta/Mapa2Floresta.h"
#include "../../Mapas/Reino/Mapa3PonteReino.h"
#include "../../Mapas/Reino/Mapa4Reino.h"
#include "../../Entidades/Racas/Anao.h"
#include "../../Entidades/Racas/Elfo.h"
#include "../../Entidades/Racas/Humano.h"
#include "../../Entidades/Racas/Orc.h"
#include "../../Sistemas/Progresso/Progressao.h"
#include "../../Sistemas/Progresso/ProgressaoFlags.h"

void EstadoMenu::executar(Jogo& jogo, ContextoJogo& ctx) {
    auto jogador = MenuJogo::menuPrincipal();
    if (!jogador) { 
        jogo.mudarEstado(nullptr); 
        return; 
    }
    ctx.jogador = std::move(jogador);
    jogo.mudarEstado(std::make_unique<EstadoExploracao>());
}

void EstadoExploracao::onExit(Jogo& jogo, ContextoJogo& ctx) {
    // Desvincula e limpa a memoria do jogador para a proxima iteracao
    ctx.jogador.reset();
}

void EstadoExploracao::executar(Jogo& jogo, ContextoJogo& ctx) {
    Personagem* jogador = ctx.jogador.get();
    if (!jogador) { 
        jogo.mudarEstado(nullptr); 
        return; 
    }

    auto mapaVila = std::make_unique<Mapa1Vila>(jogador);
    auto mapaFloresta = std::make_unique<Mapa2Floresta>(jogador);
    auto mapaPonteReino = std::make_unique<Mapa3PonteReino>(jogador);
    auto mapaReino = std::make_unique<Mapa4Reino>(jogador);

    IMapa* mapaAtual = mapaVila.get();
    while (mapaAtual) {
        ProximaTransicaoMapa transicao = mapaAtual->iniciarLoopDeExploracao();
        
        if (transicao == ProximaTransicaoMapa::VoltarMenu || jogador->obterVida() <= 0 || jogador->obterVoltarProMenu()) {
            break;
        }
        else if (transicao == ProximaTransicaoMapa::Vila) {
            mapaAtual = mapaVila.get();
            mapaVila->exploracaoEstaAtiva = true;
        }
        else if (transicao == ProximaTransicaoMapa::Floresta) {
            mapaAtual = mapaFloresta.get();
            mapaFloresta->exploracaoEstaAtiva = true;
            if (!Progressao::instancia().obterFlag(Flags::Visitou_Floresta)) Progressao::instancia().definirFlag(Flags::Visitou_Floresta, true);
        }
        else if (transicao == ProximaTransicaoMapa::PonteReino) {
            mapaAtual = mapaPonteReino.get();
            mapaPonteReino->exploracaoEstaAtiva = true;
            if (!Progressao::instancia().obterFlag(Flags::Visitou_PonteReino)) Progressao::instancia().definirFlag(Flags::Visitou_PonteReino, true);
        }
        else if (transicao == ProximaTransicaoMapa::Reino) {
            mapaAtual = mapaReino.get();
            mapaReino->exploracaoEstaAtiva = true;
            if (!Progressao::instancia().obterFlag(Flags::Visitou_Reino)) Progressao::instancia().definirFlag(Flags::Visitou_Reino, true);
        }
        else { // Inclui ProximaTransicaoMapa::Nenhuma
            // Se nenhuma transicao foi solicitada, significa que o loop de exploracao do mapa
            // terminou por uma razao interna (ex: o mapa de destino nao foi atualizado para o novo sistema).
            // Sair do loop de exploracao e voltar ao menu e o comportamento mais seguro.
            jogador->definirVoltarProMenu(true);
            break;
        }
    }
    
    if (jogador->obterVida() > 0 && !jogador->obterVoltarProMenu()) { 
        jogo.mudarEstado(nullptr); 
        return; 
    }

    jogo.mudarEstado(std::make_unique<EstadoMenu>());
}
