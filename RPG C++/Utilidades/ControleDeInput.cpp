#include "ControleDeInput.h"
#include <iostream>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

bool ControleDeInput::teclaPressionada() 
{
#ifdef _WIN32
    return _kbhit() != 0;
#else
    return false; // Ambiente POSIX requer implementacao detalhada non-blocking
#endif
}

char ControleDeInput::lerTecla() 
{
#ifdef _WIN32
    return static_cast<char>(_getch());
#else
    return static_cast<char>(std::cin.get());
#endif
}

void ControleDeInput::limparBuffer() 
{
#ifdef _WIN32
    while (_kbhit()) _getch();
#endif
}

ComandoMapa ControleDeInput::traduzirTeclaParaComando(char tecla)
{
    if (tecla == 'w' || tecla == 'W' || tecla == 72) return ComandoMapa::Cima;
    if (tecla == 's' || tecla == 'S' || tecla == 80) return ComandoMapa::Baixo;
    if (tecla == 'a' || tecla == 'A' || tecla == 75) return ComandoMapa::Esquerda;
    if (tecla == 'd' || tecla == 'D' || tecla == 77) return ComandoMapa::Direita;
    if (tecla == 'i' || tecla == 'I') return ComandoMapa::Inventario;
    if (tecla == 'c' || tecla == 'C') return ComandoMapa::Ficha;
    if (tecla == 'b' || tecla == 'B') return ComandoMapa::Bestiario;
    return ComandoMapa::Nenhum;
}
