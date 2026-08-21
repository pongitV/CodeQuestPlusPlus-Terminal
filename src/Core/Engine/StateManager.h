// [PT-BR] Arquivo: StateManager.h
// [PT-BR] Proposito: Gerenciamento do ciclo de vida dos estados do jogo (Padrao State Pattern).
// [EN-US] File: StateManager.h
// [EN-US] Purpose: Life cycle management of game states (State Pattern).

#pragma once

#include <memory>
#include "Domain/Characters/Character.h"

class Game;

// [PT-BR] Estrutura de contexto global compartilhada entre os estados do jogo.
// [EN-US] Global context structure shared between game states.
struct GameContext {
    std::unique_ptr<Character> player;
};

// [PT-BR] Interface base para os estados do jogo (Menu, Exploracao, Combate).
// [EN-US] Base interface for game states (Menu, Exploration, Combat).
class GameState {
public:
    virtual ~GameState() = default;
    virtual void onEnter([[maybe_unused]] Game& game, [[maybe_unused]] GameContext& ctx) {}
    virtual void execute(Game& game, GameContext& ctx) = 0;
    virtual void onExit([[maybe_unused]] Game& game, [[maybe_unused]] GameContext& ctx) {}
};

// [PT-BR] Maquina de estados principal do jogo.
// [EN-US] Main game state machine.
class Game {
private:
    std::unique_ptr<GameState> currentState;
    std::unique_ptr<GameState> nextState;
    bool pendingChange = false;
    GameContext context;

public:
    explicit Game(std::unique_ptr<GameState> initialState) noexcept 
        : currentState(std::move(initialState)) {}
    
    // [PT-BR] Solicita a transicao para um novo estado no proximo ciclo
    // [EN-US] Requests transition to a new state on the next cycle
    void changeState(std::unique_ptr<GameState> newState) noexcept { 
        nextState = std::move(newState);
        pendingChange = true;
    }

    GameContext& getContext() noexcept { return context; }
    const GameContext& getContext() const noexcept { return context; }
    
    // [PT-BR] Loop principal de execucao da maquina de estados
    // [EN-US] Main execution loop of the state machine
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

// [PT-BR] Estado ativo durante a exploracao do mapa.
// [EN-US] Active state during map exploration.
class ExplorationState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
    void onExit(Game& game, GameContext& ctx) override;
};

// [PT-BR] Estado ativo durante a exibicao de menus.
// [EN-US] Active state during menu display.
class MenuState final : public GameState {
public:
    void execute(Game& game, GameContext& ctx) override;
};
