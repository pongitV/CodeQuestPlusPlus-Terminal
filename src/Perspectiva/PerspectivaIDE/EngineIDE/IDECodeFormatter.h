#pragma once

#include <string>
#include <vector>

class IDECodeFormatter {
public:
    static std::string formatarPalavraChave(const std::string& texto);
    static std::string formatarTipo(const std::string& texto);
    static std::string formatarString(const std::string& texto);
    static std::string formatarNumero(const std::string& texto);
    static std::string formatarComentario(const std::string& texto);
    static std::string formatarFuncao(const std::string& texto);
    static std::string formatarVariavel(const std::string& texto);

    static std::string formatarLinhaCodigo(const std::vector<std::pair<std::string, std::string>>& tokens);
    static std::vector<std::string> formatarBloco(const std::vector<std::string>& linhas);
};
