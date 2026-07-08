#pragma once

#include <string>
#include <vector>
#include <functional>
#include "../../Entidades/Personagem.h"
#include "Menu/TelaNome.h"
#include "Menu/TelaRaca.h"
#include "Menu/TelaClasse.h"
#include "Menu/TelaDificuldade.h"
#include "Menu/TelaParry.h"

class RegistroTelas {
public:
    static bool modoRaycasterAtivo();

    // --- TelaAbertura ---
    static void abertura();

    // --- TelaMenu ---
    static void painelLogo(const std::string& tituloDaTela = "", bool animarFadeIn = false);
    static bool confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
        const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
    static std::vector<std::string> quadroAtributos(const Atributos& stats,
        const std::string& tituloSecao, const std::string& tituloHabilidade,
        const std::string& nomeHab, const std::string& descHab,
        const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2);
    static int menuPrincipal();
    static void tutorialParry(const std::string& infoBox = "");

    // --- TelaNome ---
    static TelaNome::Resultado telaNome();

    // --- TelaRaca ---
    static TelaRaca::Resultado telaRaca(const std::string& nomePersonagem);

    // --- TelaClasse ---
    static TelaClasse::Resultado telaClasse(const std::string& nomePersonagem, const std::string& raca);

    // --- TelaDificuldade ---
    static TelaDificuldade::Resultado telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe);

    // --- TelaParry ---
    static TelaParry::Resultado telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe);

    // --- TelaIntroducao ---
    static void telaIntroducao();
    
    // --- Sair do Jogo ---
    static bool confirmarSaida();
};
