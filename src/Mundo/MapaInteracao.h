#pragma once

#include <functional>
#include <string>

enum class ProximaTransicaoMapa {
    Nenhuma,
    VoltarMenu,
    Vila,
    Floresta,
    PonteReino,
    Reino
};

class IMapa {
public:
    virtual ~IMapa() = default;
    virtual std::string obterTitulo() const = 0;
    virtual ProximaTransicaoMapa iniciarLoopDeExploracao() = 0;
};


class Mapa2Floresta;
struct ContextoInteracaoFloresta {
    Mapa2Floresta* self;
    int proximaPosicaoX;
    int proximaPosicaoY;
    int larguraDoTerminal;
    const std::function<void()>& restaurarTela;
    char celula;
    const std::function<void()>& animarTela;
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
    const std::function<void()>& animarTela;
};
class InteracaoVila {
public:
    virtual ~InteracaoVila() = default;
    virtual void processar(ContextoInteracaoVila& ctx) = 0;
};
