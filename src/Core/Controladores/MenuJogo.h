#pragma once

#include <memory>

#include "../../Entidades/Personagem.h"

class MenuJogo
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

    static std::unique_ptr<Personagem> menuPrincipal();
    static std::unique_ptr<Personagem> iniciarCriacaoDeSistemaPersonagem();

private:

    static std::vector<std::string> lerInformacoesDosSaves(const std::vector<std::string>& saves);
    static void etapaEscolherNome(std::string& nomeDoPersonagem, EtapaCriacao& etapaAtual);
    static void etapaEscolherRaca(const std::string& nome, std::unique_ptr<RacaBase>& racaEscolhida, EtapaCriacao& etapaAtual);
    static void etapaEscolherClasse(const std::string& nome, RacaBase* raca, std::unique_ptr<ClasseBase>& classeEscolhida, EtapaCriacao& etapaAtual);
    static void etapaConfigurarParry(const std::string& nome, RacaBase* raca, ClasseBase* classe, int dificuldade, bool& parry, EtapaCriacao& etapaAtual);
    static void etapaEscolherDificuldade(const std::string& nome, RacaBase* raca, ClasseBase* classe, int& dificuldade, EtapaCriacao& etapaAtual);
};
