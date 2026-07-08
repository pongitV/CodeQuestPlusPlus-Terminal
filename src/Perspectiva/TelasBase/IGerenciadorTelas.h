#pragma once

#include <string>
#include <vector>
#include "../../Entidades/Personagem.h"
#include "Menu/TelaNome.h"
#include "Menu/TelaRaca.h"
#include "Menu/TelaClasse.h"
#include "Menu/TelaDificuldade.h"
#include "Menu/TelaParry.h"

// Interface abstrata (DIP) para que RegistroTelas não dependa de implementações concretas (Raycaster ou IDE)
class IGerenciadorTelas {
public:
    virtual ~IGerenciadorTelas() = default;

    virtual void abertura() = 0;
    virtual void painelLogo(const std::string& tituloDaTela, bool animarFadeIn) = 0;
    virtual bool confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
        const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) = 0;
    virtual std::vector<std::string> quadroAtributos(const Atributos& stats,
        const std::string& tituloSecao, const std::string& tituloHabilidade,
        const std::string& nomeHab, const std::string& descHab,
        const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) = 0;
    virtual int menuPrincipal() = 0;
    virtual void tutorialParry(const std::string& infoBox) = 0;
    virtual TelaNome::Resultado telaNome() = 0;
    virtual TelaRaca::Resultado telaRaca(const std::string& nomePersonagem) = 0;
    virtual TelaClasse::Resultado telaClasse(const std::string& nomePersonagem, const std::string& raca) = 0;
    virtual TelaDificuldade::Resultado telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) = 0;
    virtual TelaParry::Resultado telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) = 0;
    virtual void telaIntroducao() = 0;
    virtual bool confirmarSaida() = 0;
};
