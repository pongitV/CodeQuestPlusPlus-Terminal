#pragma once

#include <string>
#include <vector>

#include "../../../Entidades/Personagem.h"

class TelaMenuIDE {
public:
    static void exibirPainelLogoJogo(const std::string& tituloDaTela = "", bool animarFadeIn = false);
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
    static std::vector<std::string> comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2 = "", const std::string& nomeHab2 = "", const std::string& descHab2 = "");
    static int exibirOpcoesMenuPrincipal(bool temSave, const std::vector<std::vector<std::string>>& artesClasses, const std::vector<std::vector<std::string>>& artesRacas, const std::vector<std::vector<std::string>>& artesInimigos);
    static int exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves);
    static void exibirPromptNome();
    static int exibirPromptRaca(const std::string& nome);
    static int exibirPromptClasse(const std::string& nome, const std::string& nomeRaca);
    static int exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse);
    static int exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse);
    static void exibirIntroducaoJornada(const std::string& infoBox = "");
};
