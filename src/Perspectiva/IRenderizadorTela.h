#pragma once

#include <string>
#include "../Core/Utilidades/Aparencia.h"

class IRenderizadorTela {
public:
    virtual ~IRenderizadorTela() = default;
    virtual void limparTela() = 0;
    virtual void exibirPainelTexto(const std::string& texto, Cor cor) = 0;
};
