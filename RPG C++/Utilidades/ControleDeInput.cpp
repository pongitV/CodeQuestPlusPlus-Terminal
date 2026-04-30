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
