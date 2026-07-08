#pragma once
#include <string>
#include <vector>

class TelaMapaMundoRaycaster {
public:
    static void renderizarPopup(const std::vector<std::string>& arte, const std::vector<std::string>& lugares, int selecao, bool redesenhoCompleto = true);
};
