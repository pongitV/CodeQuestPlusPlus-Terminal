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
    
    // O fundo livre de qualquer painel, inimigo ou UI adicional
    virtual std::vector<std::string> obterFundoInicial() const = 0;
    
    // Retorna o fundo ja com os elementos (inimigos, lapide, caixas de loot) aplicados usando a opacidade dada
    virtual std::vector<std::string> comporFundoComElementos(float opacity) const = 0;

    virtual int obterTargetY() const { return 2; } // Padrao de altura que o usuario pediu para igualar

    // Metodo utilitario para as classes filhas desenharem paineis na tela 3D
    void sobreporPainel(std::vector<std::string>& fundo, const std::vector<std::string>& arte, int startY, int startX) const;
};
