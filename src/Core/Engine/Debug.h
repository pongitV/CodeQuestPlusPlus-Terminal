// [PT-BR] Arquivo: Debug.h
// [PT-BR] Proposito: Fornece utilitarios e menu de trapacas (cheats/debug) para testes de desenvolvimento.
// [EN-US] File: Debug.h
// [EN-US] Purpose: Provides utilities and cheat/debug menu for development testing.

#pragma once

class Character;

// [PT-BR] Classe estatica de depuracao e trapacas durante o desenvolvimento.
// [EN-US] Static class for debugging and cheating during development.
class Debug {
public:
    // [PT-BR] Indica se o modo imortal/deus esta ativo
    // [EN-US] Indicates whether God mode (invincibility/max stats) is active
    static bool isGodModeActive;

    // [PT-BR] Indica se a colisao de mapa (noclip) esta desativada
    // [EN-US] Indicates whether map collision (noclip) is disabled
    static bool isNoclipActive;

    // [PT-BR] Exibe o menu principal de depuracao e permite ativar cheats no personagem
    // [EN-US] Displays the main debug menu and allows activating cheats on the character
    static void displayDebugMenu(Character* player);
};
