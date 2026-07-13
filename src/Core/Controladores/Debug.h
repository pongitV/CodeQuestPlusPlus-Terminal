#pragma once

class Personagem;

class Debug {
public:
    static bool isGodModeAtivo;
    static bool isNoclipAtivo;

    static void exibirMenuDebug(Personagem* jogador);
};
