#pragma once

#include <vector>
#include <string>

class Mapa2FlorestaLayouts {
public:
    static std::vector<std::string> obterLayoutFloresta();
    static std::vector<std::string> obterLayoutCabana();
    static std::vector<std::string> obterLayoutCoracaoDaArvore();
    static std::vector<std::string> obterLayoutLabirinto();
    static std::vector<std::string> obterLayoutSalaDoChefe();
};
