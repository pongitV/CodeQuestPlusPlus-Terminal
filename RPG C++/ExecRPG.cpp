#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
#endif

#include "Sistema/FuncionalidadeMenu.h"
#include "Sistema/Personagem.h"
#include "Sistema/SimplificacoesAparencia.h"
#include "Mapas/Mapa1Vila.h"
#include "Mapas/Mapa2Floresta.h"
#include "Sistema/GerenciadorCombate.h"
#include "Sistema/GeradorInimigos.h"
#include "Sistema/GerenciadorDeSave.h"

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

int main() 
{
    // 1. Tenta elevar para Administrador antes de tudo
    garantirAdmin();

    // 2. Configura a tela (agora com permissao total)
    SimplificacoesAparencia::maximizarJanelaTerminal(); 
    SimplificacoesAparencia::limparTela();

    // 3. Inicia o fluxo do jogo
    while (true) {
        auto jogador{Menu::menuPrincipal()};
        if (!jogador) break;

        Mapa mapaDoJogo{jogador.get()};
        mapaDoJogo.iniciarLoopDeExploracaoDoMapa();
        
        if (jogador->obterVida() > 0) {
            GerenciadorDeSave::salvarJogo(jogador.get());
            if (!jogador->obterVoltarProMenu()) break;
        } else {
            break; // Sai do jogo caso tenha morrido em combate e nao tenha pedido para voltar ao menu
        }
    }

    return 0;
}