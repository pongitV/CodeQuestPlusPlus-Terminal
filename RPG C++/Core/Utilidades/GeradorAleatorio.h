#pragma once

class GeradorAleatorio {
public:
    // Retorna um numero entre min e max (inclusivo)
    static int obterInteiro(int min, int max);
    
    // Retorna true se um evento com probabilidade % acontecer (0 a 100)
    static bool rolarChance(int porcentagemSucesso);
};
