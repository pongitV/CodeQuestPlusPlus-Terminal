#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <algorithm>

enum class Cor : uint32_t {
    RESET = 0xFFFFFFFF,
    NEGRITO = 0xFFFFFFFE,
    VERMELHO = 0xFF0000,
    VERDE = 0x00FF00,
    AMARELO = 0xFFFF00,
    AZUL = 0x0000FF,
    MAGENTA = 0xFF00FF,
    CIANO = 0x00FFFF,
    BRANCO = 0xFFFFFF,
    CINZA = 0x808080,
    VERMELHO_CLARO = 0xFF5555,
    VERDE_CLARO = 0x55FF55,
    LARANJA = 0xFFA500,
    ROXO = 0x800080,
    LILAS = 0xC8A2C8,
    FUNDO_VERMELHO = 0x80FF0000,
    FUNDO_VERDE = 0x8000FF00,
    FUNDO_AMARELO = 0x80FFFF00,
    FUNDO_AZUL = 0x800000FF,
    FUNDO_MAGENTA = 0x80FF00FF,
    FUNDO_CIANO = 0x8000FFFF,
    FUNDO_BRANCO = 0x80FFFFFF
};

class Aparencia {
public:
    // --- CONFIGURACOES E CORES ---
    static void inicializarConsole();
    static std::string cor(Cor codigo);
    static std::string cor(Cor estilo, Cor codigo);

    // --- CONTROLE DO TERMINAL ---
    static void maximizarJanelaTerminal();
    static void ocultarCursor();
    static void limparTela();
    static int obterLarguraTerminal();
    static int obterAlturaTerminal();
    static void moverCursor(int x, int y);
    static int obterPosicaoCursorY();
    
    // --- UTILIDADES DE TEXTO E STRING ---
    static std::string removerCoresANSI(const std::string& texto);
    static int obterComprimentoVisual(const std::string& texto);
    static std::string espacosParaCentralizar(int comprimentoTexto);
    static std::string centralizarTexto(const std::string& texto);
    
    // --- DESENHO E FORMATACAO ---
    static std::string gerarBarraSuave(double porcentagem, int tamanhoVisual, const std::string& corCheia, const std::string& corVazia);
    static std::vector<std::string> criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo = "", int larguraMinima = 0, Cor corCaixa = Cor::BRANCO);
    static void imprimirLinhaDivisoria(char caractere = '=');
    static void imprimirCentralizado(const std::string& texto, const std::string& corAnsi = "");
    static void imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual = 0, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static void imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static int imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda = 0, int espacamento = 0, Cor corEsquerda = Cor::RESET, Cor corDireita = Cor::RESET, int atrasoLinhaMs = 0);
    
    // --- ANIMACOES E EFEITOS ---
    static std::string obterCorRGBFade(Cor corTema, int intensidade);
    static void animarFadeIn(int framesTotais, int tempoPorFrameMs, const std::function<void(int frame, int intensidade)>& renderFrame);

    // --- EXIBICAO DE TEXTO (DIGITACAO E DELAY) ---
    static int atrasoDigitacaoMS; // Variável estática para a velocidade padrão
    static void imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs = atrasoDigitacaoMS);
    static void imprimirCentralizadoDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS);
    static void imprimirDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS, bool addNewline = true);
    static void imprimirVetorAnimado(const std::vector<std::string>& linhas, int atrasoMs = 30);
    
    // --- PAINEIS E INTERFACES COMPLETAS ---
    static void exibirPainel(const std::string& titulo, Cor corPrincipal = Cor::BRANCO, const std::vector<std::string>& artePrincipal = {}, int larguraArte = 0, const std::vector<std::string>& arteSecundaria = {}, Cor corSecundaria = Cor::RESET, bool animarFadeIn = false);
    static void exibirPainelTexto(const std::string& titulo, Cor corDoCabecalho = Cor::BRANCO, bool animarFadeIn = false);
    static void exibirPainelArte(const std::vector<std::string>& arteAscii, int larguraVisual, Cor corDaArte, const std::string& tituloSecundario = "", bool animarFadeIn = false);
    static void exibirPrompt(const std::string& mensagem);
    static std::string margemCombate();
    static std::vector<std::string> reduzirEscalaAscii(const std::vector<std::string>& arteOriginal, int fatorX = 2, int fatorY = 2);

    // --- SISTEMA DE LOGS ---
    static void registrarLogBatalha(const std::string& texto);
    static void limparLogBatalha();
    static void exibirUltimosLogs(int quantidade);
    static void exibirHistoricoCompleto();

    // --- UTILIDADES DE ORDENACAO ---
    static void ordenarAlfabeticamente(std::vector<std::string>& lista);

    template <typename T, typename Extrator>
    static void ordenarAlfabeticamente(std::vector<T>& lista, Extrator extratorDeTexto) {
        std::sort(lista.begin(), lista.end(), [&extratorDeTexto](const T& a, const T& b) {
            return extratorDeTexto(a) < extratorDeTexto(b);
        });
    }
};
