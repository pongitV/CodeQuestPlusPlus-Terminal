// [PT-BR] Arquivo: TerminalSessionGuard.h
// [PT-BR] Proposito: Guard RAII para restaurar configuracoes de cursor e cores do terminal ao sair do jogo.
// [EN-US] File: TerminalSessionGuard.h
// [EN-US] Purpose: RAII guard to restore terminal cursor and colors upon game exit.

#pragma once

#include <iostream>

// [PT-BR] Classe guard RAII que garante o reset do console no encerramento da sessao
// [EN-US] RAII guard class guaranteeing console reset on session termination
class TerminalSessionGuard {
public:
    TerminalSessionGuard() = default;
    
    ~TerminalSessionGuard() {
        // [PT-BR] Restaura o cursor visivel e reseta atributos ANSI
        // [EN-US] Restores visible cursor and resets ANSI attributes
        std::cout << "\033[?25h\033[0m" << std::flush;
    }

    TerminalSessionGuard(const TerminalSessionGuard&) = delete;
    TerminalSessionGuard& operator=(const TerminalSessionGuard&) = delete;
    TerminalSessionGuard(TerminalSessionGuard&&) = default;
    TerminalSessionGuard& operator=(TerminalSessionGuard&&) = default;
};
