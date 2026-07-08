#pragma once

#include <string>
#include <vector>
#include "../../../TelasBase/TelaBase.h"
#include "../../../IDiarioUI.h"

class Personagem;

class TelaDiarioRaycaster : public IDiarioUI {
public:
    void renderizarFundo() override;
    void exibirCabecalho(int startY) override;
    void renderizarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, Cor corCaixa, int minY, int startYOverride) override;
    void renderizarPopupMensagem(const std::string& titulo, const std::vector<std::string>& texto) override;
    void renderizarPopupInspecaoComArte(const std::string& titulo, const std::vector<std::string>& arte, const std::vector<std::string>& info, const std::string& subtitulo) override;
};
