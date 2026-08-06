/*
 * Arquivo: StateManager.h
 * Proposito: Gerenciamento do ciclo de vida dos estados do jogo (Padrao State Pattern).
 */

#pragma once

#include <memory>
#include "Domain/Characters/Character.h"

class Game;

/*
 * Estrutura de contexto global compartilhada entre os estados do jogo.
 */
struct GameContext {
    std::unique_ptr<Character> player;
};

/*
 * Interface base para os estados do jogo (Menu, Exploracao, Combate).
 */
class GameState {
public:
    virtual ~GameState() = default;
    virtual void onEnter(Game& game, GameContext& ctx) {}
    virtual void execute(Game& game, GameContext& ctx) = 0;
    virtual void onExit(Game& game, GameContext& ctx) {}
};

/*
 * Maquina de estados principal do jogo.
 */
class Game {
private:
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;
    bool pendingChange = false;
    GameContext context;

public:
    explicit Game(std::unique_ptr<GameState> initialState) noexcept 
        : currentState(std::move(initialState)) {}
    
    // Solicita a transicao para um novo estado no proximo ciclo
    void changeState(std::unique_ptr<GameState> newState) noexcept { 
        nextState = std::move(newState);
        pendingChange = true;
    }

    GameContext& getContext() noexcept { return context; }
    const GameContext& getContext() const noexcept { return context; }
    
    // Loop principal de execucao da maquina de estados
    void run() {
        if (currentState) currentState->onEnter(*this, context);
        while (currentState) {
            currentState->execute(*this, context);
            
            if (pendingChange) {
                if (currentState) currentState->onExit(*this, context);
                currentState = std::move(nextState);
                if (currentState) currentState->onEnter(*this, context);
                pendingChange = false;
            }
        }
    }
};

/*
 * Estado ativo durante a exploracao do mapa.
 */
class ExplorationState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
    void onExit(Game& game, GameContext& ctx) override;
};

/*
 * Estado ativo durante a exibicao de menus.
 */
class MenuState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
};

