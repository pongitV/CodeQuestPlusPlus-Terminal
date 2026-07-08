#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace FormatadorAnsi {

    inline std::string palavraChave(const std::string& texto) {
        return "\033[38;2;86;156;214m" + texto + "\033[0m";
    }

    inline std::string tipo(const std::string& texto) {
        return "\033[38;2;78;201;176m" + texto + "\033[0m";
    }

    inline std::string stringLit(const std::string& texto) {
        return "\033[38;2;214;157;133m\"" + texto + "\"\033[0m";
    }

    inline std::string comentario(const std::string& texto) {
        return "\033[38;2;96;139;78m" + texto + "\033[0m";
    }

    inline std::string funcao(const std::string& texto) {
        return "\033[38;2;220;220;170m" + texto + "\033[0m()";
    }

    inline std::string corRGB(const std::string& texto, int r, int g, int b) {
        return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + texto + "\033[0m";
    }

    inline std::string fundoRGB(const std::string& texto, int r, int g, int b) {
        return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m" + texto + "\033[0m";
    }

    inline std::vector<std::string> formatarTextoIDE(const std::vector<std::string>& texto) {
        std::vector<std::string> formatado;
        formatado.push_back(palavraChave("public:"));
        for (const auto& linha : texto) {
            if (linha.empty() || linha == " ") {
                formatado.push_back("");
            } else {
                formatado.push_back("    " + palavraChave("string") + " msg = " + stringLit(linha) + ";");
            }
        }
        formatado.push_back("};");
        return formatado;
    }

    inline std::vector<std::string> formatarArteIDE(const std::vector<std::string>& arte) {
        if (arte.empty()) return arte;
        std::vector<std::string> formatado;
        formatado.push_back(comentario("/* =================================="));
        for (const auto& l : arte) {
            formatado.push_back(l);
        }
        formatado.push_back(comentario("================================== */"));
        return formatado;
    }

    inline std::string formatarTituloIDE(const std::string& titulo) {
        return palavraChave("class") + " " + tipo(titulo) + " {";
    }

    inline std::vector<std::string> formatarOpcoesIDE(const std::vector<std::string>& opcoes) {
        std::vector<std::string> opcoesIDE;
        for (const auto& op : opcoes) {
            opcoesIDE.push_back(funcao(op));
        }
        return opcoesIDE;
    }

    inline std::string formatarTextoSIMD(const std::string& texto) {
        return comentario("// ") + texto;
    }

}
