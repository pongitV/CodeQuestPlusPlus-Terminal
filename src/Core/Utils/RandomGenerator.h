// [PT-BR] Arquivo: RandomGenerator.h
// [PT-BR] Proposito: Gerador estatico de numeros aleatorios e probabilidades.
// [EN-US] File: RandomGenerator.h
// [EN-US] Purpose: Static generator of random numbers and probabilities.

#pragma once

// [PT-BR] Fornece metodos utilitarios para calculo de aleatoriedade no jogo.
// [EN-US] Provides utility methods for randomness calculation in the game.
class RandomGenerator {
public:
    // [PT-BR] Retorna um numero inteiro entre min e max (inclusivo)
    // [EN-US] Returns an integer between min and max (inclusive)
    static int getInteger(int min, int max);
    
    // [PT-BR] Retorna verdadeiro se um teste de probabilidade (0 a 100%) for bem-sucedido
    // [EN-US] Returns true if a probability roll (0 to 100%) succeeds
    static bool rollChance(int successPercentage);
};
