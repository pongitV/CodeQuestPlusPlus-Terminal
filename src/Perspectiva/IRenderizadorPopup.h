#pragma once

#include <vector>
#include <string>
#include "../Core/Utilidades/Aparencia.h"

class IRenderizadorPopup {
public:
    virtual ~IRenderizadorPopup() = default;
    virtual void exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteAscii = {}) = 0;
    virtual void iniciarInteracaoPopup() = 0;
    virtual int lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteLogo = {}, bool voltarHabilitado = true) = 0;
};
