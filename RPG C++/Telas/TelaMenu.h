#pragma once

#include <string>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"

class TelaMenu {
public:
    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
    static std::vector<std::string> comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab);
    static void exibirOpcoesMenuPrincipal(bool temSave);
    static void exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves);
    static void exibirPromptNome();
    static void exibirPromptRaca(const std::string& nome);
    static void exibirPromptClasse(const std::string& nome, const std::string& nomeRaca);
    static void exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse);
    static void exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse);
};
