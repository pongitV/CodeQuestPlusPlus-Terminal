#pragma once

#include <iostream>

// Classe guard RAII que garante o reset do console no encerramento da sessao
class TerminalSessionGuard {
public:
    TerminalSessionGuard() = default;
    
    ~TerminalSessionGuard() {
        // Restaura o cursor visivel e reseta atributos ANSI
        std::cout << "\033[?25h\033[0m" << std::flush;
    }

    TerminalSessionGuard(const TerminalSessionGuard&) = delete;
    TerminalSessionGuard& operator=(const TerminalSessionGuard&) = delete;
    TerminalSessionGuard(TerminalSessionGuard&&) = default;
    TerminalSessionGuard& operator=(TerminalSessionGuard&&) = default;
};
