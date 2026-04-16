#include "Personagem.h"
#include <memory>

#pragma once

class Menu
{
public:
    // ==========================================
    // CONTROLE DE JANELA E TERMINAL
    // ==========================================
    static void maximizarJanelaDoTerminal();
    static int obterLarguraDoTerminalEmColunas();
    static void limparTelaDoTerminal();

    // ==========================================
    // CONTROLE DE INPUT
    // ==========================================
    static void aguardarPressionamentoDeEnter();

    // ==========================================
    // RENDERIZACAO DE TEXTO E UI
    // ==========================================
    static void imprimirTextoComEfeitoDeDigitacao(const std::string& textoParaImprimir, int tempoDeEsperaEmMilissegundos);
    static void imprimirLinhasCentralizadasNaTela(const std::vector<std::string>& linhasParaImprimir, int larguraVisualDaArte = 0, const std::string& codigoDeCorAnsi = "");
    static void exibirLogoDoJogo(const std::string& tituloDaTela = "");
    
    // ==========================================
    // FLUXO PRINCIPAL
    // ==========================================
    static std::unique_ptr<Personagem> iniciarCriacaoDePersonagem();

private:
    static bool exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir);
};