#pragma once

#include <vector>
#include <string>

class TelaCena3D {
public:
    virtual ~TelaCena3D() = default;

    void executar();

protected:
    virtual std::vector<std::string> obterLogo() const = 0;
    virtual std::string obterCorLogo() const = 0;

    virtual std::vector<std::string> obterFundoInicial() const = 0;

    virtual std::vector<std::string> comporFundoComElementos(float opacity) const = 0;

    virtual int obterTargetY() const { return 2; }

    void sobreporPainel(std::vector<std::string>& fundo, const std::vector<std::string>& arte, int startY, int startX) const;
};
