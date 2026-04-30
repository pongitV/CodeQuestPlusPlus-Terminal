#pragma once

class ControleDeInput 
{
public:
    // Abstrai as restricoes do SO ao interagir diretamente com o input do usuario
    static bool teclaPressionada();
    static char lerTecla();
    static void limparBuffer();
};
