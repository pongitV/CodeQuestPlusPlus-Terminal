#include "IDECodeFormatter.h"

std::string IDECodeFormatter::formatarPalavraChave(const std::string& texto) {
    return "\033[38;2;86;156;214m" + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarTipo(const std::string& texto) {
    return "\033[38;2;78;201;176m" + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarString(const std::string& texto) {
    return "\033[38;2;214;157;133m\"" + texto + "\"\033[0m";
}

std::string IDECodeFormatter::formatarNumero(const std::string& texto) {
    return "\033[38;2;181;206;168m" + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarComentario(const std::string& texto) {
    return "\033[38;2;96;139;78m// " + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarFuncao(const std::string& texto) {
    return "\033[38;2;220;220;170m" + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarVariavel(const std::string& texto) {
    return "\033[38;2;156;220;254m" + texto + "\033[0m";
}

std::string IDECodeFormatter::formatarLinhaCodigo(const std::vector<std::pair<std::string, std::string>>& tokens) {
    std::string resultado;
    for (const auto& [tipo, texto] : tokens) {
        if (tipo == "keyword") resultado += formatarPalavraChave(texto);
        else if (tipo == "type") resultado += formatarTipo(texto);
        else if (tipo == "string") resultado += formatarString(texto);
        else if (tipo == "number") resultado += formatarNumero(texto);
        else if (tipo == "comment") resultado += formatarComentario(texto);
        else if (tipo == "function") resultado += formatarFuncao(texto);
        else if (tipo == "variable") resultado += formatarVariavel(texto);
        else resultado += texto;
    }
    return resultado;
}

std::vector<std::string> IDECodeFormatter::formatarBloco(const std::vector<std::string>& linhas) {
    std::vector<std::string> resultado;
    for (const auto& linha : linhas) {
        resultado.push_back("    " + linha);
    }
    return resultado;
}
