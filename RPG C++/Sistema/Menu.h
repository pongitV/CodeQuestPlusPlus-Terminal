#include "Personagem.h"

#pragma once

class Menu 
{
public:
    static void configurarTelaCheia();
    static void exibirLogo(const std::string& titulo = "");
    static void limparTela();
    static void esperar();
    static void digitar(const std::string& texto, int velocidade);
    
    static Personagem* criarPersonagem();
    static void exibirStatusJogador(Personagem* p);
    static void exibirInventario(Personagem* p); 
    static void exibirFichaJogador(Personagem* p);
    static void exibirTelaVitoria(Personagem* p, int ouro, int danoCausado, int danoRecebido);
    static void exibirTelaDerrota(Personagem* p, int ouro, int danoCausado, int danoRecebido);

    static void exibirHorda(const std::vector<Personagem*>& inimigos);

private:
    static int obterLarguraTerminal(); 
    static bool exibirPreviaLadoALado(const std::string& tipo, const std::string& nome, const std::vector<std::string>& info, const std::vector<std::string>& arte);
};