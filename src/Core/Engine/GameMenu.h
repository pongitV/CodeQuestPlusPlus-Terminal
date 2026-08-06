/*
 * Arquivo: GameMenu.h
 * Proposito: Gerencia a navegacao do menu inicial e o fluxo de criacao do personagem principal.
 */

#pragma once

#include <memory>

#include "Domain/Characters/Character.h"

/*
 * Controla os menus iniciais do jogo (Novo Jogo, Carregar Jogo, Criacao de Personagem).
 */
class GameMenu
{
public:
    // Exibe o menu principal e retorna o personagem carregado ou criado
    static std::unique_ptr<Character> mainMenu();

    // Executa o assistente interativo de criacao de personagem (Raca, Classe, Nome)
    static std::unique_ptr<Character> startCharacterSystemCreation();
};

