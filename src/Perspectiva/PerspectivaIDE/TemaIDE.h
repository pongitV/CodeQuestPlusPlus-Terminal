#pragma once

#include <string>
#include <vector>

namespace TemaIDE {

    // Paleta de cores C++ para o Terminal
    const std::string COR_PALAVRA_CHAVE = "\033[38;2;86;156;214m"; // Azul
    const std::string COR_TIPO = "\033[38;2;78;201;176m";          // Azul/Verde (Turquesa)
    const std::string COR_STRING = "\033[38;2;214;157;133m";        // Laranja/Verde
    const std::string COR_NUMERO = "\033[38;2;181;206;168m";        // Verde claro
    const std::string COR_COMENTARIO = "\033[38;2;96;139;78m";      // Verde escuro
    const std::string COR_FUNCAO = "\033[38;2;220;220;170m";        // Amarelo claro
    const std::string COR_VARIAVEL = "\033[38;2;156;220;254m";      // Azul claro
    const std::string COR_OPERADOR = "\033[38;2;180;180;180m";      // Cinza
    const std::string COR_RESET = "\033[0m";

    inline std::string palavraChave(const std::string& texto) {
        return COR_PALAVRA_CHAVE + texto + COR_RESET;
    }

    inline std::string tipo(const std::string& texto) {
        return COR_TIPO + texto + COR_RESET;
    }

    inline std::string stringLiteral(const std::string& texto) {
        return COR_STRING + "\"" + texto + "\"" + COR_RESET;
    }

    inline std::string numero(const std::string& texto) {
        return COR_NUMERO + texto + COR_RESET;
    }

    inline std::string comentario(const std::string& texto) {
        return COR_COMENTARIO + "// " + texto + COR_RESET;
    }

    inline std::string comentarioBloco(const std::vector<std::string>& texto) {
        std::string resultado = COR_COMENTARIO + "/*\n";
        for (const auto& linha : texto) {
            resultado += " * " + linha + "\n";
        }
        resultado += " */" + COR_RESET;
        return resultado;
    }

    inline std::string funcao(const std::string& texto) {
        return COR_FUNCAO + texto + COR_RESET;
    }

    inline std::string variavel(const std::string& texto) {
        return COR_VARIAVEL + texto + COR_RESET;
    }

    inline std::vector<std::string> formatarCaixaComoClasse(const std::string& nomeClasse, const std::vector<std::string>& conteudo) {
        std::vector<std::string> formatado;
        formatado.push_back(palavraChave("class ") + tipo(nomeClasse) + " {");
        formatado.push_back(palavraChave("public:"));
        for (const auto& linha : conteudo) {
            formatado.push_back("    " + linha);
        }
        formatado.push_back("};");
        return formatado;
    }

} // namespace TemaIDE
