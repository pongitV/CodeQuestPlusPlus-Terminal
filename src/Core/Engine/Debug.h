/*
 * Arquivo: Debug.h
 * Proposito: Fornece utilitarios e menu de trapacas (cheats/debug) para testes de desenvolvimento.
 */

#pragma once

class Character;

/*
 * Classe estatica de depuracao e trapacas durante o desenvolvimento.
 */
class Debug {
public:
    // Indica se o modo imortal/deus esta ativo
    static bool isGodModeActive;

    // Indica se a colisao de mapa (noclip) esta desativada
    static bool isNoclipActive;

    // Exibe o menu principal de depuracao e permite ativar cheats no personagem
    static void displayDebugMenu(Character* player);
};

