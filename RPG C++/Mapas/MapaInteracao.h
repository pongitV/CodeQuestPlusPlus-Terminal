#pragma once

#include <functional>

class Mapa2Floresta; // Forward declaration

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


class Mapa1Vila; // Forward declaration

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