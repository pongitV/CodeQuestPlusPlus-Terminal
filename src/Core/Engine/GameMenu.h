// [PT-BR] Arquivo: GameMenu.h
// [PT-BR] Proposito: Gerencia a navegacao do menu inicial e o fluxo de criacao do personagem principal.
// [EN-US] File: GameMenu.h
// [EN-US] Purpose: Manages initial menu navigation and the main character creation flow.

#pragma once

#include <memory>

#include "Domain/Characters/Character.h"

// [PT-BR] Controla os menus iniciais do jogo (Novo Jogo, Carregar Jogo, Criacao de Personagem).
// [EN-US] Controls initial game menus (New Game, Load Game, Character Creation).
class GameMenu
{
public:
    // [PT-BR] Exibe o menu principal e retorna o personagem carregado ou criado
    // [EN-US] Displays the main menu and returns the loaded or created character
    static std::unique_ptr<Character> mainMenu();

    // [PT-BR] Executa o assistente interativo de criacao de personagem (Raca, Classe, Nome)
    // [EN-US] Executes the interactive character creation wizard (Race, Class, Name)
    static std::unique_ptr<Character> startCharacterSystemCreation();
};
