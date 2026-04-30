#include "SistemaPersonagem.h"
#include <memory>

#pragma once

class SistemaMenu
{
public:
    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    
    static std::unique_ptr<SistemaPersonagem> iniciarCriacaoDeSistemaPersonagem();

    static void exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos);

private:
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);

    static void etapaEscolherNome(std::string& nomeDoPersonagem, int& etapaAtual);
    static void etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, int& etapaAtual);
    static void etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, int& etapaAtual);
    static void etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, bool& parry, int& etapaAtual);
    static void etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, int& etapaAtual);
};