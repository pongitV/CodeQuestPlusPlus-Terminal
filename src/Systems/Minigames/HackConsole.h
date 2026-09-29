#pragma once
#include <string>

class Character;

// Executa o minigame de hacking via terminal.
class HackConsole {
public:
    // Inicia o minigame de hack. Retorna verdadeiro se o jogador obtiver sucesso.
    static bool startHack(Character* player);
};

