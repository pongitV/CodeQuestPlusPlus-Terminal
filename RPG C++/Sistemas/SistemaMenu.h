#pragma once

#include <memory>

#include "SistemaPersonagem.h"

class SistemaMenu
{
public:
    enum class EtapaCriacao 
    {
        Nome = 1,
        Raca,
        Classe,
        Dificuldade,
        Parry,
        Concluido
    };

    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    
    static std::unique_ptr<SistemaPersonagem> iniciarCriacaoDeSistemaPersonagem();

    static void exibirHordaDeInimigosLadoALado(const std::vector<SistemaPersonagem*>& listaDeInimigos);

private:
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);

    static void etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual);
    static void etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual);
    static void etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, EtapaCriacao& etapaAtual);
    static void etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, int dificuldade, bool& parry, EtapaCriacao& etapaAtual);
    static void etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual);
};