#pragma once

#include <memory>
#include "../../Entidades/Personagem.h"

class Jogo;

struct ContextoJogo {
    std::unique_ptr<Personagem> jogador;
};

class EstadoJogo {
public:
    virtual ~EstadoJogo() = default;
    virtual void onEnter(Jogo& jogo, ContextoJogo& ctx) {}
    virtual void executar(Jogo& jogo, ContextoJogo& ctx) = 0;
    virtual void onExit(Jogo& jogo, ContextoJogo& ctx) {}
};

class Jogo {
private:
    std::unique_ptr<EstadoJogo> estadoAtual;
    std::unique_ptr<EstadoJogo> proximoEstado;
    bool mudancaPendente = false;
    ContextoJogo contexto;

public:
    explicit Jogo(std::unique_ptr<EstadoJogo> estadoInicial) noexcept 
        : estadoAtual(std::move(estadoInicial)) {}
    
    void mudarEstado(std::unique_ptr<EstadoJogo> novoEstado) noexcept { 
        proximoEstado = std::move(novoEstado);
        mudancaPendente = true;
    }

    ContextoJogo& obterContexto() noexcept { return contexto; }
    const ContextoJogo& obterContexto() const noexcept { return contexto; }
    
    void rodar() {
        if (estadoAtual) estadoAtual->onEnter(*this, contexto);
        while (estadoAtual) {
            estadoAtual->executar(*this, contexto);
            
            if (mudancaPendente) {
                if (estadoAtual) estadoAtual->onExit(*this, contexto);
                estadoAtual = std::move(proximoEstado);
                if (estadoAtual) estadoAtual->onEnter(*this, contexto);
                mudancaPendente = false;
            }
        }
    }
};

class EstadoExploracao final : public EstadoJogo {
public:
    void executar(Jogo& jogo, ContextoJogo& ctx) override;
    void onExit(Jogo& jogo, ContextoJogo& ctx) override;
};

class EstadoMenu final : public EstadoJogo {
public:
    void executar(Jogo& jogo, ContextoJogo& ctx) override;
};
