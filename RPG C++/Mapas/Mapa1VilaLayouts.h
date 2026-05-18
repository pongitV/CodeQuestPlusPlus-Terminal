#pragma once

#include <vector>
#include <string>

class Mapa1VilaLayouts {
public:
    static std::vector<std::string> obterLayoutVilaInicial();
    static std::vector<std::string> obterLayoutForja();
    static std::vector<std::string> obterLayoutLoja();
    static std::vector<std::string> obterLayoutCaverna(bool bjornResgatado);
};
