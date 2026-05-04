#pragma once

#include <functional>

class Mapa2Floresta;
struct ContextoInteracaoFloresta {
    Mapa2Floresta* self;
    int proximaPosicaoX;
    int proximaPosicaoY;
    int larguraDoTerminal;
    const std::function<void()>& restaurarTela;
    char celula;
};
class InteracaoFloresta {
public:
    virtual ~InteracaoFloresta() = default;
    virtual void processar(ContextoInteracaoFloresta& ctx) = 0;
};


class Mapa1Vila;
struct ContextoInteracaoVila {
    Mapa1Vila* self;
    int proximaPosicaoX;
    int proximaPosicaoY;
    int larguraDoTerminal;
    const std::function<void()>& restaurarTela;
    char celula;
};
class InteracaoVila {
public:
    virtual ~InteracaoVila() = default;
    virtual void processar(ContextoInteracaoVila& ctx) = 0;
};