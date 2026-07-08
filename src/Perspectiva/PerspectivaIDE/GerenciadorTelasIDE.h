#pragma once

#include "../TelasBase/IGerenciadorTelas.h"

class GerenciadorTelasIDE : public IGerenciadorTelas {
public:
    void abertura() override;
    void painelLogo(const std::string& tituloDaTela, bool animarFadeIn) override;
    bool confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
        const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) override;
    std::vector<std::string> quadroAtributos(const Atributos& stats,
        const std::string& tituloSecao, const std::string& tituloHabilidade,
        const std::string& nomeHab, const std::string& descHab,
        const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) override;
    int menuPrincipal() override;
    void tutorialParry(const std::string& infoBox) override;
    TelaNome::Resultado telaNome() override;
    TelaRaca::Resultado telaRaca(const std::string& nomePersonagem) override;
    TelaClasse::Resultado telaClasse(const std::string& nomePersonagem, const std::string& raca) override;
    TelaDificuldade::Resultado telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) override;
    TelaParry::Resultado telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) override;
    void telaIntroducao() override;
    bool confirmarSaida() override;
};
