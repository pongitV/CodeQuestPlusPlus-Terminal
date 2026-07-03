#pragma once

#include <string>
#include <vector>

class Personagem;

class TelaPauseRaycaster {
public:
    static int renderizarMenuPause();
    static int renderizarMenuConfiguracoes(Personagem* jogador);
    static int renderizarMenuAparencia(Personagem* jogador);
    static int renderizarMenuFundo(int corFundoAtualIndex);
    static int renderizarMenuSensibilidade(int percX, int percY);
    static bool renderizarConfirmacaoSalvar(Personagem* jogador);
};
