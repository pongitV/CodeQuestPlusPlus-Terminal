#pragma once

#include <string>

enum class ComandoMapa {
    Cima,
    Baixo,
    Esquerda,
    Direita,
    Inventario,
    Ficha,
    Bestiario,
    Nenhum
};

class ControleDeInput 
{
public:
    // Abstrai as restricoes do SO ao interagir diretamente com o input do usuario
    static bool teclaPressionada();
    static char lerTecla();
    static ComandoMapa traduzirTeclaParaComando(char tecla);
    static void limparBuffer();
    static std::string lerEntradaProtegida();
};
