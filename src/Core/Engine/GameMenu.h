/*
 * Arquivo: GameMenu.h
 * Propósito: Gerencia a navegação do menu inicial e o fluxo de criação do personagem principal.
 */

#pragma once

#include <memory>

#include "Domain/Characters/Character.h"

/*
 * Controla os menus iniciais do jogo (Novo Jogo, Carregar Jogo, Criação de Personagem).
 */
class GameMenu
{
public:
    /*
     * Exibe o menu principal e retorna o personagem carregado ou criado.
     */
    static std::unique_ptr<Character> mainMenu();

    /*
     * Executa o assistente interativo de criação de personagem (Raça, Classe, Nome).
     */
    static std::unique_ptr<Character> startCharacterSystemCreation();
};

