#include <iostream>
#include <memory>
#include <array>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
#endif

#include "Entidades/Classes/Arqueiro.h"
#include "Entidades/Classes/Bardo.h"
#include "Entidades/Classes/ClasseBase.h"
#include "Entidades/Classes/Guerreiro.h"
#include "Entidades/Classes/Mago.h"
#include "Core/Controladores/MenuJogo.h"
#include "Mapas/Vila/Mapa1Vila.h"
#include "Mapas/Floresta/Mapa2Floresta.h"
#include "Mapas/Reino/Mapa3PonteReino.h"
#include "Mapas/Reino/Mapa4Reino.h"
#include "Entidades/Racas/Anao.h"
#include "Entidades/Racas/Elfo.h"
#include "Entidades/Racas/Humano.h"
#include "Entidades/Racas/Orc.h"
#include "Sistemas/Progresso/Salvamento.h"
#include "Sistemas/Progresso/Progressao.h"
#include "Sistemas/Progresso/ProgressaoFlags.h"
#include "Core/Utilidades/Aparencia.h"
#include "Perspectiva/GerenciadorPerspectiva.h"

// Funcao para garantir que o jogo rode como Administrador
bool garantirAdmin() noexcept 
{
#ifdef _WIN32
    if (!IsUserAnAdmin()) 
    {
        std::array<char, MAX_PATH> caminho{};
        if (GetModuleFileNameA(nullptr, caminho.data(), MAX_PATH) != 0) 
        {
            SHELLEXECUTEINFOA sei = { sizeof(sei) };
            sei.lpVerb = "runas"; // Comando para elevar privilegios
            sei.lpFile = caminho.data();
            sei.hwnd = nullptr;
            sei.nShow = SW_NORMAL;

            if (ShellExecuteExA(&sei)) 
            {
                return true; // Sucesso ao abrir nova instancia, fechar a atual
            }
        }
    }
#endif
    return false; // Continua execucao normal (ja e admin ou falhou)
}

// --- PADRAO STATE PARA O FLUXO DO JOGO ---
class Jogo;

struct ContextoJogo {
    std::unique_ptr<Personagem> jogador;
};

class EstadoJogo {
public:
    virtual ~EstadoJogo() = default;
    virtual void onEnter(Jogo& jogo, ContextoJogo& ctx) {}
    virtual void executar(Jogo& jogo, ContextoJogo& ctx) = 0;
    virtual void onExit(Jogo& jogo, ContextoJogo& ctx) {}
};

class Jogo {
private:
    std::unique_ptr<EstadoJogo> estadoAtual;
    std::unique_ptr<EstadoJogo> proximoEstado;
    bool mudancaPendente = false;
    ContextoJogo contexto;

public:
    explicit Jogo(std::unique_ptr<EstadoJogo> estadoInicial) noexcept 
        : estadoAtual(std::move(estadoInicial)) {}
    
    void mudarEstado(std::unique_ptr<EstadoJogo> novoEstado) noexcept { 
        proximoEstado = std::move(novoEstado);
        mudancaPendente = true;
    }

    ContextoJogo& obterContexto() noexcept { return contexto; }
    const ContextoJogo& obterContexto() const noexcept { return contexto; }
    
    void rodar() {
        if (estadoAtual) estadoAtual->onEnter(*this, contexto);
        while (estadoAtual) {
            estadoAtual->executar(*this, contexto);
            
            if (mudancaPendente) {
                if (estadoAtual) estadoAtual->onExit(*this, contexto);
                estadoAtual = std::move(proximoEstado);
                if (estadoAtual) estadoAtual->onEnter(*this, contexto);
                mudancaPendente = false;
            }
        }
    }
};

class EstadoMenu; // Forward declaration

class EstadoExploracao final : public EstadoJogo {
public:
    void executar(Jogo& jogo, ContextoJogo& ctx) override;
    void onExit(Jogo& jogo, ContextoJogo& ctx) override;
};

class EstadoMenu final : public EstadoJogo {
public:
    void executar(Jogo& jogo, ContextoJogo& ctx) override {
        auto jogador = MenuJogo::menuPrincipal();
        if (!jogador) { 
            jogo.mudarEstado(nullptr); 
            return; 
        }
        ctx.jogador = std::move(jogador);
        jogo.mudarEstado(std::make_unique<EstadoExploracao>());
    }
};

void EstadoExploracao::onExit(Jogo& jogo, ContextoJogo& ctx) {
    Personagem* jogador = ctx.jogador.get();
    // Salva o jogo APENAS se o jogador estiver voltando para o menu principal por escolha propria.
    if (jogador && jogador->obterVida() > 0 && jogador->obterVoltarProMenu()) {
        Salvamento::salvarJogo(jogador);
    }
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
// -----------------------------------------

int main() 
{
    // 1. Tenta elevar para Administrador antes de tudo
    if (garantirAdmin()) return 0;

    // 2. Configura a tela (agora com permissao total)
    Aparencia::inicializarConsole();
    Aparencia::maximizarJanelaTerminal(); 
    Aparencia::limparTela();
    
    // 3. Inicializa os renders do Gerenciador de Visoes (Seta o RendererProvider)
    GerenciadorPerspectiva::obterInstancia().inicializar();

    // 3. Inicia o fluxo do jogo usando o State Pattern
    Jogo rpg(std::make_unique<EstadoMenu>());
    rpg.rodar();

    return 0;
}
