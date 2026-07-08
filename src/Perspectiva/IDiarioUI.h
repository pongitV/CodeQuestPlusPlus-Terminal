#pragma once
#include <string>
#include <vector>
#include "../Core/Utilidades/Aparencia.h"

class IDiarioUI {
public:
    virtual ~IDiarioUI() = default;
    virtual void renderizarFundo() = 0;
    virtual void exibirCabecalho(int startY) = 0;
    virtual void renderizarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, Cor corCaixa, int minY, int startYOverride) = 0;
    virtual void renderizarPopupMensagem(const std::string& titulo, const std::vector<std::string>& texto) = 0;
    virtual void renderizarPopupInspecaoComArte(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) = 0;
};
