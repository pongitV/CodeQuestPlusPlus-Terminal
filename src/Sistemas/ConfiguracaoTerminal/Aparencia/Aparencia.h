#pragma once

#include "Cor.h"
#include "../../../Core/Utilidades/AnsiColor.h"
#include "../../../Core/Utilidades/TerminalUtils.h"
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

class Aparencia {
public:
    static Cor corFundoAtiva;

    static void inicializarConsole();
    static std::string converterVetorParaStringBruta(const std::vector<std::string>& vetor);
    static std::string sobreporPainelNaLinhaAnsi(const std::string& backgroundLine, const std::string& panelLine, int startX);
    static std::string cor(Cor codigo);
    static std::vector<std::string> converterStringBrutaParaVetor(const std::string& textoBruto);
    static void padronizarTamanhoVetor(std::vector<std::string>& linhas);
    static std::vector<std::string> carregarArte(const std::string& caminhoArquivo);
    static std::string cor(Cor estilo, Cor codigo);
    static std::string corRGB(uint8_t r, uint8_t g, uint8_t b, bool negrito = false);
    static std::string bgRGB(uint8_t r, uint8_t g, uint8_t b);

    static void maximizarJanelaTerminal();
    static void ajustarFonteParaResolucao();
    static void ocultarCursor();
    static void mostrarCursor();
    static void definirCorFundoTerminal(const std::string& hexColor);
    static void limparTela();
    static int obterLarguraTerminal();
    static int obterAlturaTerminal();
    static void moverCursor(int x, int y);
    static int obterPosicaoCursorY();
    
    static std::string removerCoresANSI(const std::string& texto);
    static int obterComprimentoVisual(const std::string& texto);
    static std::string espacosParaCentralizar(int comprimentoTexto);
    static std::string centralizarTexto(const std::string& texto);
    
    static void imprimirCentralizado(const std::string& texto, const std::string& corAnsi = "");
    static void imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual = 0, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static void imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static int imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda = 0, int espacamento = 0, Cor corEsquerda = Cor::RESET, Cor corDireita = Cor::RESET, int atrasoLinhaMs = 0);
    
    static std::string obterCorRGBFade(Cor corTema, int intensidade);
    static void animarFadeIn(int framesTotais, int tempoPorFrameMs, const std::function<void(int frame, int intensidade)>& renderFrame);
    static void exibirTelaIntro(const std::vector<std::string>& arteLogo, const std::vector<std::string>& textoNarracao, Cor corTema);

    static int atrasoDigitacaoMS;
    static void imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs = atrasoDigitacaoMS);
    static void imprimirCentralizadoDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS);
    static void imprimirDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS, bool addNewline = true);
    static void imprimirVetorAnimado(const std::vector<std::string>& linhas, int atrasoMs = 30);
    
    static void exibirPainel(const std::string& titulo, Cor corPrincipal = Cor::BRANCO, const std::vector<std::string>& artePrincipal = {}, int larguraArte = 0, const std::vector<std::string>& arteSecundaria = {}, Cor corSecundaria = Cor::RESET, bool animarFadeIn = false);
    static void exibirPainelTexto(const std::string& titulo, Cor corDoCabecalho = Cor::BRANCO, bool animarFadeIn = false);
    static void exibirPainelArte(const std::vector<std::string>& arteAscii, int larguraVisual, Cor corDaArte, const std::string& tituloSecundario = "", bool animarFadeIn = false);
    static void exibirTituloPadrao(const std::string& titulo, Cor corTema = Cor::BRANCO);
    static void exibirPrompt(const std::string& mensagem);
    static void exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema = Cor::BRANCO, const std::vector<std::string>& arteAscii = {}, bool mostrarMensagemEnter = true);
    static void renderizarCaixaPopupAnimada(const std::vector<std::string>& caixa, int startX, int startY, bool animar);
    static void removerCaixaPopupAnimada(int finalBoxWidth, int finalBoxHeight, int startX, int startY, bool animar = true);
    static void iniciarInteracaoPopup();
    static void atualizarMinTamanhoPopup(int largura, int altura);
    static int obterMinLarguraPopup();
    static int obterMinAlturaPopup();
    static int lerInteiroEmPopupFlutuante(const std::string& mensagem, int limiteMin, int limiteMax, Cor corTema = Cor::BRANCO);
    static void limparPopupAnterior();
    static void definirUltimoPopup(int x, int y, int w, int h, const std::string& bgAnsi = "\033[48;2;25;25;25m");
    static std::vector<std::string> reduzirEscalaAscii(const std::vector<std::string>& arteOriginal, float fatorX = 2.0f, float fatorY = 2.0f);
    static std::string sobreporLogoAnsi(const std::string& backgroundLine, const std::vector<std::string>& logoChars, int startX, const std::string& fgColor, int larguraTerminal);
    static std::string fadarLinhaAnsi(const std::string& linha, float ratio);
    
    static void animarTransicaoCena3D(
        const std::vector<std::string>& logoBase,
        const std::string& corFinalLogo,
        const std::vector<std::string>& fundoLivre,
        std::function<std::vector<std::string>(float)> getFundoFinal,
        int targetY = 2
    );

    static int FATOR_COMPRESSAO_GLOBAL;

    static void registrarLogBatalha(const std::string& texto);
    static void limparLogBatalha();
    static void exibirUltimosLogs(int quantidade);
    static void exibirHistoricoCompleto();

    static void ordenarAlfabeticamente(std::vector<std::string>& lista);

    template <typename T, typename Extrator>
    static void ordenarAlfabeticamente(std::vector<T>& lista, Extrator extratorDeTexto) {
        std::sort(lista.begin(), lista.end(), [&extratorDeTexto](const T& a, const T& b) {
            return extratorDeTexto(a) < extratorDeTexto(b);
        });
    }
};
