#pragma once

class SistemaPersonagem;

class TelaPause {
public:
    static void exibirMenuDePausa(SistemaPersonagem* jogador);

private:
    static void exibirMenuConfiguracoes(SistemaPersonagem* jogador);
};
