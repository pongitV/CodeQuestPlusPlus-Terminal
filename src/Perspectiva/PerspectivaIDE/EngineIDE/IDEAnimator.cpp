#include "IDEAnimator.h"
#include <iostream>
#include <thread>
#include <chrono>

void IDEAnimator::efeitoDigitar(const std::vector<std::string>& linhas, int velocidadeMs) {
    for (const auto& linha : linhas) {
        for (char c : linha) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(velocidadeMs));
        }
        std::cout << "\n";
    }
}

void IDEAnimator::scrollAnimado(const std::vector<std::string>& conteudo, int linhasPorFrame, int intervaloMs) {
    for (size_t i = 0; i < conteudo.size(); i += linhasPorFrame) {
        std::cout << "\033[H\033[J";
        for (size_t j = i; j < i + 20 && j < conteudo.size(); ++j) {
            std::cout << conteudo[j] << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(intervaloMs));
    }
}

void IDEAnimator::transicaoTelas(const std::function<void()>& renderNovaTela, int duracaoMs) {
    int passos = 10;
    for (int i = 0; i < passos; ++i) {
        std::cout << "\033[H\033[J";
        renderNovaTela();
        std::this_thread::sleep_for(std::chrono::milliseconds(duracaoMs / passos));
    }
}

void IDEAnimator::piscarCursor(bool ativo) {
    if (ativo) {
        std::cout << "\033[?25h";
    } else {
        std::cout << "\033[?25l";
    }
}
