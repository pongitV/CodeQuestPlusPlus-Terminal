/*
 * Arquivo: RandomGenerator.h
 * Propósito: Gerador estático de números aleatórios e probabilidades.
 */

#pragma once

/*
 * Fornece métodos utilitários para cálculo de aleatoriedade no jogo.
 */
class RandomGenerator {
public:
    // Retorna um número inteiro entre min e max (inclusivo)
    static int getInteger(int min, int max);
    
    // Retorna verdadeiro se um teste de probabilidade (0 a 100%) for bem-sucedido
    static bool rollChance(int successPercentage);
};

