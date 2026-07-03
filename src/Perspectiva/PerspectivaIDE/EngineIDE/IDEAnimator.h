#pragma once

#include <string>
#include <vector>
#include <functional>

class IDEAnimator {
public:
    static void efeitoDigitar(const std::vector<std::string>& linhas, int velocidadeMs = 30);
    static void scrollAnimado(const std::vector<std::string>& conteudo, int linhasPorFrame = 1, int intervaloMs = 50);
    static void transicaoTelas(const std::function<void()>& renderNovaTela, int duracaoMs = 300);
    static void piscarCursor(bool ativo);
};
