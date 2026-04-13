#include "Personagem.h"

#pragma once

class Menu
{
public:
    static void maximizarJanelaDoTerminal();
    static void limparTelaDoTerminal();
    static void aguardarPressionamentoDeEnter();
    static void imprimirTextoComEfeitoDeDigitacao(const std::string& textoParaImprimir, int tempoDeEsperaEmMilissegundos);
    static int obterLarguraDoTerminalEmColunas();

    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    static void exibirLogoParaTelaDeCombate(const std::string& tituloDaTela = "");
    
    static Personagem* iniciarCriacaoDePersonagem();
    static void exibirBarraDeStatusDoJogador(Personagem* jogadorAtual);
    static void exibirTelaDeInventario(Personagem* jogadorAtual); 
    static void exibirTelaDeAtributosDoJogador(Personagem* jogadorAtual);
    static void exibirTelaDeVitoria(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, const std::vector<std::string>& itensObtidos);
    static void exibirTelaDeDerrota(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido);

    static void exibirHordaDeInimigosLadoALado(const std::vector<Personagem*>& listaDeInimigos);
    static void imprimirLinhasCentralizadasNaTela(const std::vector<std::string>& linhasParaImprimir, int larguraVisualDaArte = 0, const std::string& codigoDeCorAnsi = "");

private:
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
};