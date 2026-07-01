#include "HackConsole.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Entidades/Personagem.h"
#include <iostream>
#include <vector>

using namespace std;

bool HackConsole::iniciarHack(Personagem* jogador) {
    Aparencia::limparTela();
    
    vector<string> linhasCodigo = {
        "// Sistema de Segurança v1.2",
        "bool unlockDoor() {",
        "    int accessLevel = getPlayerAccess();",
        "    if (accessLevel < 5) {",
        "        ______ false; // <- ERRO AQUI: Faltando palavra-chave de retorno",
        "    }",
        "    return true;",
        "}"
    };
    
    string resposta = "";
    bool rodando = true;
    bool hackSucesso = false;
    
    while (rodando) {
        Aparencia::limparTela();
        cout << Aparencia::cor(Cor::CIANO) << "=== CONSOLE DE DESENVOLVEDOR ===" << Aparencia::cor(Cor::RESET) << "\n\n";
        
        for (const auto& linha : linhasCodigo) {
            cout << linha << "\n";
        }
        
        cout << "\n> Insira a palavra-chave que falta: " << resposta;
        
        char tecla = ControleDeInput::lerTecla();
        if (tecla == 13) { // ENTER
            if (resposta == "return") {
                hackSucesso = true;
                rodando = false;
            } else {
                cout << "\n\n" << Aparencia::cor(Cor::VERMELHO) << "[ERRO DE SINTAXE] Acesso negado." << Aparencia::cor(Cor::RESET);
                ControleDeInput::aguardarEnter();
                rodando = false;
            }
        } else if (tecla == 8) { // BACKSPACE
            if (!resposta.empty()) resposta.pop_back();
        } else if (tecla == 27) { // ESC
            rodando = false;
        } else if (tecla >= 32 && tecla <= 126) {
            resposta += tecla;
        }
    }
    
    if (hackSucesso) {
        Aparencia::limparTela();
        cout << Aparencia::cor(Cor::VERDE) << "\n[COMPILACAO BEM-SUCEDIDA] Acesso de dev concedido." << Aparencia::cor(Cor::RESET) << "\n";
        
        cout << "\nVoce encontrou um Log corrompido! Ganhou +200 XP e curou vida.\n";
        jogador->ganharXp(200);
        jogador->modificarVida(50);
        ControleDeInput::aguardarEnter();
    }
    
    return hackSucesso;
}
