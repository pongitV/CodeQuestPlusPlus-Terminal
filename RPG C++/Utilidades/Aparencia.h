#pragma once

#include <string>
#include <vector>
#include <cstdint>

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
    static void inicializarConsole();
    static std::string cor(Cor codigo);
    static std::string cor(Cor estilo, Cor codigo);

    static void maximizarJanelaTerminal();
    static void ocultarCursor();
    static void limparTela();
    static int obterLarguraTerminal();
    static int obterAlturaTerminal();
    static void moverCursor(int x, int y);
    static int obterPosicaoCursorY();
    
    static std::string removerCoresANSI(const std::string& texto);
    static int obterComprimentoVisual(const std::string& texto);
    static std::string gerarBarraSuave(double porcentagem, int tamanhoVisual, const std::string& corCheia, const std::string& corVazia);
    static std::string espacosParaCentralizar(int comprimentoTexto);
    static std::string centralizarTexto(const std::string& texto);
    static std::vector<std::string> criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo = "", int larguraMinima = 0, Cor corCaixa = Cor::BRANCO);
    static void imprimirLinhaDivisoria(char caractere = '=');
    static void imprimirCentralizado(const std::string& texto, const std::string& corAnsi = "");
    static void imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual = 0, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static void imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi = "", int atrasoLinhaMs = 0);
    static void imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs = atrasoDigitacaoMS);
    static void imprimirCentralizadoDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS);
    static int atrasoDigitacaoMS; // Variável estática para a velocidade padrão
    static void imprimirDigitando(const std::string& texto, int atrasoMs = atrasoDigitacaoMS, bool addNewline = true);
    static void imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true);
    static void imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::vector<std::string>& linhas);
    static void imprimirVetorAnimado(const std::vector<std::string>& linhas, int atrasoMs = 30);
    static void exibirCabecalho(const std::string& titulo, Cor corDoCabecalho = Cor::BRANCO);
    static int imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda = 0, int espacamento = 0, Cor corEsquerda = Cor::RESET, Cor corDireita = Cor::RESET, int atrasoLinhaMs = 0);
    static void exibirPrompt(const std::string& mensagem);
    static std::string margemCombate();
    static void exibirLogoAscii(const std::vector<std::string>& arteAscii, int larguraVisual, Cor corDaArte, const std::string& tituloSecundario = "", int atrasoLinhaMs = 0);
    static std::vector<std::string> reduzirEscalaAscii(const std::vector<std::string>& arteOriginal);

    static void registrarLogBatalha(const std::string& texto);
    static void limparLogBatalha();
    static void exibirUltimosLogs(int quantidade);
    static void exibirHistoricoCompleto();
};
