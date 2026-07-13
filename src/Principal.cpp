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
#include "Sistemas/Progresso/Progressao.h"
#include "Sistemas/Progresso/ProgressaoFlags.h"
#include "Core/Utilidades/Aparencia.h"
#include "Sistemas/ConfiguracaoTerminal/ControleInputOutput/ControleDeInput.h"
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

#include "Core/Controladores/GerenciadorDeEstados.h"

int main() 
{
    // 1. Tenta elevar para Administrador antes de tudo
    if (garantirAdmin()) return 0;

    // 2. Configura a tela (agora com permissao total)
    Aparencia::inicializarConsole();
    Aparencia::maximizarJanelaTerminal(); 
    Aparencia::limparTela();
    
    // Configura Mouse Events do Console
    ControleDeInput::habilitarMouseInput();
    
    // 3. Inicializa os renders do Gerenciador de Visoes (Seta o RendererProvider)
    GerenciadorPerspectiva::obterInstancia().inicializar();

    // 3. Inicia o fluxo do jogo usando o State Pattern
    Jogo rpg(std::make_unique<EstadoMenu>());
    rpg.rodar();

    return 0;
}
