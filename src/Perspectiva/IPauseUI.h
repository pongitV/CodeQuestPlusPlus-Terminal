#pragma once
#include "../Entidades/Personagem.h"

class IPauseUI {
public:
    virtual ~IPauseUI() = default;
    virtual int renderizarMenuPause() = 0;
    virtual int renderizarMenuConfiguracoes(Personagem* jogador) = 0;
    virtual int renderizarMenuAparencia(Personagem* jogador) = 0;
    virtual int renderizarMenuFundo(int corFundoAtualIndex) = 0;
    virtual int renderizarMenuSensibilidade(int percX, int percY) = 0;
};
