/*
 * Arquivo: Debug.h
 * Propósito: Fornece utilitários e menu de trapaças (cheats/debug) para testes de desenvolvimento.
 */

#pragma once

class Character;

/*
 * Classe estática de depuração e trapaças durante o desenvolvimento.
 */
class Debug {
public:
    // Indica se o modo imortal/deus está ativo
    static bool isGodModeActive;

    // Indica se a colisão de mapa (noclip) está desativada
    static bool isNoclipActive;

    /*
     * Exibe o menu principal de depuração e permite ativar cheats no personagem.
     * 
     * player: Ponteiro para a instância do personagem do jogador.
     */
    static void displayDebugMenu(Character* player);
};

