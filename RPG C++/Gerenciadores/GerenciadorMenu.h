#pragma once

#include <memory>

#include "../Sistemas/SistemaPersonagem.h"

class GerenciadorMenu
{
public:
    enum class EtapaCriacao 
    {
        Nome = 1,
        Raca,
        Classe,
        Parry,
        Dificuldade,
        Concluido
    };

    static std::unique_ptr<SistemaPersonagem> menuPrincipal();
    static std::unique_ptr<SistemaPersonagem> iniciarCriacaoDeSistemaPersonagem();

private:

    static void etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual);
    static void etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual);
    static void etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, EtapaCriacao& etapaAtual);
    static void etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, bool& parry, EtapaCriacao& etapaAtual);
    static void etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual);
};