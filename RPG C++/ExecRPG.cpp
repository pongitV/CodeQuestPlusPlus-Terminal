#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
#endif

#include "Classes/Arqueiro.cpp"
#include "Classes/Bardo.cpp"
#include "Classes/ClasseBase.h"
#include "Classes/Guerreiro.cpp"
#include "Classes/Mago.cpp"
#include "Gerenciadores/GerenciadorMenu.h"
#include "Mapas/Mapa1Vila.h"
#include "Racas/Dwarf.cpp"
#include "Racas/Elfo.cpp"
#include "Racas/Humano.cpp"
#include "Racas/Ork.cpp"
#include "Sistemas/SistemaSave.h"
#include "Utilidades/SimplificacoesAparencia.h"

// Funcao para garantir que o jogo rode como Administrador
void garantirAdmin() 
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
            exit(0); // Fecha a instancia sem admin
        }
    }
#endif
}

// --- PADRAO STATE PARA O FLUXO DO JOGO ---
class Jogo;

class EstadoJogo {
public:
    virtual ~EstadoJogo() = default;
    virtual void executar(Jogo& jogo) = 0;
};

class Jogo {
private:
    std::unique_ptr<EstadoJogo> estadoAtual;
    std::unique_ptr<SistemaPersonagem> jogadorAtual;
public:
    Jogo(std::unique_ptr<EstadoJogo> estadoInicial) : estadoAtual(std::move(estadoInicial)) {}
    
    void mudarEstado(std::unique_ptr<EstadoJogo> novoEstado) { estadoAtual = std::move(novoEstado); }
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

void EstadoExploracao::executar(Jogo& jogo) {
    SistemaPersonagem* jogador = jogo.obterJogador();
    if (!jogador) { jogo.mudarEstado(nullptr); return; }

    Mapa1Vila mapaDoJogo{jogador};
    mapaDoJogo.iniciarLoopDeExploracaoDoMapa1Vila();
    
    if (jogador->obterVida() > 0) {
        SistemaSave::salvarJogo(jogador);
        if (!jogador->obterVoltarProMenu()) { jogo.mudarEstado(nullptr); return; }
    }
    // Volta para o menu principal
    jogo.definirJogador(nullptr);
    jogo.mudarEstado(std::make_unique<EstadoMenu>());
}
// -----------------------------------------

int main() 
{
    // 1. Tenta elevar para Administrador antes de tudo
    garantirAdmin();

    // 2. Configura a tela (agora com permissao total)
    SimplificacoesAparencia::inicializarConsole();
    SimplificacoesAparencia::maximizarJanelaTerminal(); 
    SimplificacoesAparencia::limparTela();

    // 3. Inicia o fluxo do jogo usando o State Pattern
    Jogo rpg(std::make_unique<EstadoMenu>());
    rpg.rodar();

    return 0;
}