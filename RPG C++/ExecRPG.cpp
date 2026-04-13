#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h> // Necessario para ShellExecuteEx
    #include <shlobj.h>   // Necessario para IsUserAnAdmin
#endif

#include "Sistema/Menu.h"
#include "Sistema/Personagem.h"
#include "Mapas/Mapa1Vila.h"
#include "Mapas/Mapa2Floresta.h"
#include "Sistema/SistemaRPG.h"
#include "Sistema/GeradorInimigos.h"

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
    Menu::maximizarJanelaDoTerminal(); 
    Menu::limparTelaDoTerminal();

    // 3. Inicia o fluxo do jogo
    Personagem* jogador = Menu::iniciarCriacaoDePersonagem();
    Mapa mapaDoJogo(jogador);
    mapaDoJogo.iniciarLoopDeExploracaoDoMapa();

    delete jogador;
    return 0;
}