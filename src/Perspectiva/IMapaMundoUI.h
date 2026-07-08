#pragma once
#include <string>
#include <vector>

class IMapaMundoUI {
public:
    virtual ~IMapaMundoUI() = default;
    virtual void renderizarPopup(const std::vector<std::string>& arte, const std::vector<std::string>& lugares, int selecao, bool redesenhoCompleto = true) = 0;
};
