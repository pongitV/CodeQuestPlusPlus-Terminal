#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
#endif

#include "Classes/Arqueiro.h"
#include "Classes/Bardo.h"
#include "Classes/ClasseBase.h"
#include "Classes/Guerreiro.h"
#include "Classes/Mago.h"
#include "Gerenciadores/GerenciadorMenu.h"
#include "Mapas/Mapa1Vila.h"
#include "Racas/Dwarf.h"
#include "Racas/Elfo.h"
#include "Racas/Humano.h"
#include "Racas/Ork.h"
#include "Sistemas/SistemaSave.h"
#include "Utilidades/SimplificacoesAparencia.h"

// Funcao para garantir que o jogo rode como Administrador
bool garantirAdmin() 
{
#ifdef _WIN32
    if (!IsUserAnAdmin()) 
    {
        char caminho[MAX_PATH];
        GetModuleFileNameA(NULL, caminho, MAX_PATH);

        SHELLEXECUTEINFOA sei = { sizeof(sei) };
        sei.lpVerb = "runas"; // Comando para elevar privilegios
        sei.lpFile = caminho;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;

        if (ShellExecuteExA(&sei)) 
        {
            return true; // Sucesso ao abrir nova instancia, fechar a atual
        }
    }
#endif
    return false; // Continua execucao normal (ja e admin ou falhou)
}

// --- PADRAO STATE PARA O FLUXO DO JOGO ---
class Jogo;

class EstadoJogo {
public:
    virtual ~EstadoJogo() = default;
    virtual void onEnter(Jogo& jogo) {}
    virtual void executar(Jogo& jogo) = 0;
    virtual void onExit(Jogo& jogo) {}
};

class Jogo {
private:
    std::unique_ptr<EstadoJogo> estadoAtual;
    std::unique_ptr<SistemaPersonagem> jogadorAtual;
public:
    Jogo(std::unique_ptr<EstadoJogo> estadoInicial) : estadoAtual(std::move(estadoInicial)) {
        if (estadoAtual) estadoAtual->onEnter(*this);
    }
    
    void mudarEstado(std::unique_ptr<EstadoJogo> novoEstado) { 
        if (estadoAtual) estadoAtual->onExit(*this);
        estadoAtual = std::move(novoEstado); 
        if (estadoAtual) estadoAtual->onEnter(*this);
    }
    void definirJogador(std::unique_ptr<SistemaPersonagem> jogador) { jogadorAtual = std::move(jogador); }
    SistemaPersonagem* obterJogador() const { return jogadorAtual.get(); }
    
    void rodar() {
        while (estadoAtual) {
            estadoAtual->executar(*this);
        }
    }
};

class EstadoMenu; // Forward declaration

class EstadoExploracao : public EstadoJogo {
public:
    void executar(Jogo& jogo) override;
    void onExit(Jogo& jogo) override;
};

class EstadoMenu : public EstadoJogo {
public:
    void executar(Jogo& jogo) override {
        auto jogador = GerenciadorMenu::menuPrincipal();
        if (!jogador) { jogo.mudarEstado(nullptr); return; }
        jogo.definirJogador(std::move(jogador));
        jogo.mudarEstado(std::make_unique<EstadoExploracao>());
    }
};

void EstadoExploracao::onExit(Jogo& jogo) {
    SistemaPersonagem* jogador = jogo.obterJogador();
    // Salva o jogo caso a transicao de mapa aconteca enquanto o jogador ainda esta vivo
    if (jogador && jogador->obterVida() > 0) {
        SistemaSave::salvarJogo(jogador);
    }
    // Desvincula e limpa a memoria do jogador para a proxima iteracao
    jogo.definirJogador(nullptr);
}

void EstadoExploracao::executar(Jogo& jogo) {
    SistemaPersonagem* jogador = jogo.obterJogador();
    if (!jogador) { jogo.mudarEstado(nullptr); return; }

    Mapa1Vila mapaDoJogo{jogador};
    mapaDoJogo.iniciarLoopDeExploracaoDoMapa1Vila();
    
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
    SimplificacoesAparencia::inicializarConsole();
    SimplificacoesAparencia::maximizarJanelaTerminal(); 
    SimplificacoesAparencia::limparTela();

    // 3. Inicia o fluxo do jogo usando o State Pattern
    Jogo rpg(std::make_unique<EstadoMenu>());
    rpg.rodar();

    return 0;
}