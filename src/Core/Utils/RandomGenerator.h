/*
 * Arquivo: RandomGenerator.h
 * Proposito: Gerador estatico de numeros aleatorios e probabilidades.
 */

#pragma once

/*
 * Fornece metodos utilitarios para calculo de aleatoriedade no jogo.
 */
class RandomGenerator {
public:
    // Retorna um numero inteiro entre min e max (inclusivo)
    static int getInteger(int min, int max);
    
    // Retorna verdadeiro se um teste de probabilidade (0 a 100%) for bem-sucedido
    static bool rollChance(int successPercentage);
};

