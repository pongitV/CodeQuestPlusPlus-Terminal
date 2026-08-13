/*
 * Arquivo: HackConsole.h
 * Proposito: Minigame de terminal para hackear dispositivos/consoles interativos no mapa.
 */

#pragma once
#include <string>

class Character;

/*
 * Executa o minigame de hacking via terminal.
 */
class HackConsole {
public:
    /*
     * Inicia o minigame de hack. Retorna verdadeiro se o player obtiver sucesso.
     */
    static bool startHack(Character* player);
};

