#include "Aparencia.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>
#include <sstream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
#include "ControleDeInput.h"

namespace {
    std::vector<std::string> historicoBatalha;
}

int Aparencia::atrasoDigitacaoMS = 25; // Inicialização da velocidade padrão (50ms)

void Aparencia::inicializarConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Configura UTF-8 globalmente apenas uma vez
    // Habilita interpretacao de codigos ANSI nativamente no console do Windows
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
    }
#endif
}

std::string Aparencia::cor(Cor codigo) {
    if (codigo == Cor::RESET) return "\033[0m";
    if (codigo == Cor::NEGRITO) return "\033[1m";

    uint32_t val = static_cast<uint32_t>(codigo);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string prefix = isBg ? "\033[48;2;" : "\033[38;2;";
    return prefix + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::cor(Cor estilo, Cor codigo) {
    std::string estiloStr = "";
    if (estilo == Cor::NEGRITO) estiloStr = "1;";
    else if (estilo == Cor::RESET) estiloStr = "0;";

    if (codigo == Cor::RESET) return "\033[" + estiloStr + "0m";
    if (codigo == Cor::NEGRITO) return "\033[" + estiloStr + "1m";

    uint32_t val = static_cast<uint32_t>(codigo);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string type = isBg ? "48;2;" : "38;2;";
    return "\033[" + estiloStr + type + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

void Aparencia::maximizarJanelaTerminal() {
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();

    // Define o estilo da janela para "popup" (sem bordas, sem barra de titulo)
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

    // Maximiza a janela popup, que por padrao ocupa a tela inteira
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void Aparencia::ocultarCursor() {
#ifdef _WIN32
    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);
#else
    std::cout << "\033[?25l";
#endif
}

void Aparencia::limparTela() {
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
}

int Aparencia::obterLarguraTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left; // Removido o +1 para evitar line wrap duplo invisivel
    }
#endif
    return 119;
}

int Aparencia::obterAlturaTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif
    return 30;
}

void Aparencia::moverCursor(int x, int y) {
#ifdef _WIN32
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

int Aparencia::obterPosicaoCursorY() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.dwCursorPosition.Y;
    }
#endif
    return 8;
}

std::string Aparencia::removerCoresANSI(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.length());
    bool in_sequence = false;

    for (size_t i = 0; i < texto.length(); ++i) {
        if (in_sequence) {
            // As sequências de controle CSI terminam com um caractere no intervalo 0x40-0x7E ('@' a '~').
            if (texto[i] >= 0x40 && texto[i] <= 0x7E) {
                in_sequence = false;
            }
        } else {
            // Verifica o início de uma sequência CSI: ESC [
            if (texto[i] == '\x1b' && i + 1 < texto.length() && texto[i+1] == '[') {
                in_sequence = true;
                i++; // Pula também o caractere '['
            } else {
                resultado += texto[i];
            }
        }
    }
    return resultado;
}

int Aparencia::obterComprimentoVisual(const std::string& texto) {
    std::string semCores = removerCoresANSI(texto);
    int comprimento = 0;
    for (size_t i = 0; i < semCores.length(); ) {
        unsigned char c = static_cast<unsigned char>(semCores[i]);
        if ((c & 0x80) == 0) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
        comprimento++;
    }
    return comprimento;
}

std::string Aparencia::gerarBarraSuave(double porcentagem, int tamanhoVisual, const std::string& corCheia, const std::string& corVazia) {
    const std::vector<std::string> fracoes = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
    if (porcentagem < 0.0) porcentagem = 0.0;
    if (porcentagem > 1.0) porcentagem = 1.0;

    double totalBlocos = porcentagem * tamanhoVisual;
    int blocosInteiros = static_cast<int>(totalBlocos);
    int indiceFracao = static_cast<int>((totalBlocos - blocosInteiros) * 8);

    std::string barra = corCheia;
    for (int i = 0; i < blocosInteiros; ++i) barra += "█";
    if (blocosInteiros < tamanhoVisual) barra += fracoes[indiceFracao];
    
    barra += corVazia;
    for (int i = blocosInteiros + 1; i < tamanhoVisual; ++i) barra += " ";
    barra += "\033[0m";
    return barra;
}

std::string Aparencia::espacosParaCentralizar(int comprimentoTexto) {
    int espacos = (obterLarguraTerminal() - comprimentoTexto) / 2;
    if (espacos < 0) espacos = 0;
    return std::string(espacos, ' ');
}

std::string Aparencia::centralizarTexto(const std::string& texto) {
    return espacosParaCentralizar(obterComprimentoVisual(texto)) + texto;
}

std::vector<std::string> Aparencia::criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, int larguraMinima, Cor corCaixa) {
    int maxLargura = larguraMinima;
    for (const auto& linha : linhas) {
        int comp = obterComprimentoVisual(linha);
        if (comp > maxLargura) maxLargura = comp;
    }
    
    std::vector<std::string> caixa;
    std::string corStr = cor(corCaixa);
    std::string resetStr = cor(Cor::RESET);

    std::string top = "╔";
    int tituloLen = obterComprimentoVisual(titulo);
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = maxLargura + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) {
            top += "═";
        }
    } else {
        for (int i = 0; i < maxLargura + 2; ++i) {
            top += "═";
        }
    }
    top += "╗";
    caixa.push_back(corStr + top + resetStr);

    for (const auto& linha : linhas) {
        int comp = obterComprimentoVisual(linha);
        int padding = maxLargura - comp;
        caixa.push_back(corStr + "║ " + resetStr + linha + std::string(padding > 0 ? padding : 0, ' ') + corStr + " ║" + resetStr);
    }

    std::string bottom = "╚";
    for (int i = 0; i < maxLargura + 2; ++i) {
        bottom += "═";
    }
    bottom += "╝";
    caixa.push_back(corStr + bottom + resetStr);

    return caixa;
}

void Aparencia::imprimirLinhaDivisoria(char caractere) {
    std::string linha = "";
    int largura = obterLarguraTerminal();
    if (caractere == '=') {
        for (int i = 0; i < largura; ++i) linha += "═";
    } else if (caractere == '-') {
        for (int i = 0; i < largura; ++i) linha += "─";
    } else {
        linha = std::string(largura, caractere);
    }
    std::cout << linha << "\n";
}

void Aparencia::imprimirCentralizado(const std::string& texto, const std::string& corAnsi) {
    std::cout << espacosParaCentralizar(obterComprimentoVisual(texto)) << corAnsi << texto << (corAnsi.empty() ? "" : cor(Cor::RESET)) << "\n";
}

void Aparencia::imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual, const std::string& corAnsi, int atrasoLinhaMs) {
    for (const std::string& linha : linhas) {
        if (larguraVisual > 0) {
            std::cout << espacosParaCentralizar(larguraVisual) << corAnsi << linha << (corAnsi.empty() ? "" : cor(Cor::RESET)) << "\n";
        } else {
            imprimirCentralizado(linha, corAnsi);
        }
        if (atrasoLinhaMs > 0) {
            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(atrasoLinhaMs));
        }
    }
}

std::string Aparencia::obterCorRGBFade(Cor corTema, int intensidade) {
    int r = intensidade, g = intensidade, b = intensidade;
    if (corTema == Cor::VERDE) { r = 0; g = intensidade; b = 0; }
    else if (corTema == Cor::AMARELO) { r = intensidade; g = intensidade; b = 0; }
    else if (corTema == Cor::CIANO) { r = 0; g = intensidade; b = intensidade; }
    else if (corTema == Cor::MAGENTA) { r = intensidade; g = 0; b = intensidade; }
    else if (corTema == Cor::VERMELHO) { r = intensidade; g = 0; b = 0; }
    else if (corTema == Cor::AZUL) { r = 0; g = 0; b = intensidade; }
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

void Aparencia::animarFadeIn(int framesTotais, int tempoPorFrameMs, const std::function<void(int frame, int intensidade)>& renderFrame) {
    for (int frame = 1; frame <= framesTotais; ++frame) {
        auto inicioFrame = std::chrono::steady_clock::now();

        int intensidade = (255 * frame) / framesTotais;
        renderFrame(frame, intensidade);

        auto fimFrame = std::chrono::steady_clock::now();
        auto duracaoFrame = std::chrono::duration_cast<std::chrono::milliseconds>(fimFrame - inicioFrame).count();
        int tempoEspera = std::max(0, tempoPorFrameMs - static_cast<int>(duracaoFrame));
        std::this_thread::sleep_for(std::chrono::milliseconds(tempoEspera));
    }
}

void Aparencia::imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi, int atrasoLinhaMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(linha));
    }
    imprimirCentralizadoMultilinha(linhas, tamanhoDaLinhaMaisLonga, corAnsi, atrasoLinhaMs);
}

void Aparencia::imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(linha));
    }
    std::string margem = espacosParaCentralizar(tamanhoDaLinhaMaisLonga);
    for (const std::string& linha : linhas) {
        imprimirDigitando(margem + linha, atrasoMs, true);
    }
}

void Aparencia::imprimirCentralizadoDigitando(const std::string& texto, int atrasoMs) {
    std::string margem = espacosParaCentralizar(obterComprimentoVisual(texto));
    imprimirDigitando(margem + texto, atrasoMs, true);
}

void Aparencia::imprimirDigitando(const std::string& texto, int atrasoMs, bool addNewline) {
    int linhaMsg = Aparencia::obterAlturaTerminal() - 1; // Subtrai 1 para evitar scroll na ultima linha
    if (linhaMsg < 1) linhaMsg = 1;
    std::cout << "\033[s\033[" << linhaMsg << ";1H" << cor(Cor::NEGRITO, Cor::CINZA) << "[Pressione 'k' para pular]" << cor(Cor::RESET) << "\033[u";

    size_t i = 0;
    while (i < texto.length() && texto[i] == ' ') {
        std::cout << texto[i];
        i++;
    }
    std::cout << std::flush;

    for (; i < texto.length(); ++i) {
        // Processa codigos ANSI inteiros instantaneamente para nao corromper a string se o usuario pular
        if (texto[i] == '\033') {
            size_t startAnsi = i;
            if (i + 1 < texto.length() && texto[i+1] == '[') {
                i += 2;
                while (i < texto.length() && !(texto[i] >= 0x40 && texto[i] <= 0x7E)) {
                    i++;
                }
                std::cout << texto.substr(startAnsi, i - startAnsi + 1) << std::flush;
                continue;
            }
        }

        if (ControleDeInput::teclaPressionada()) { 
            char tecla = ControleDeInput::lerTecla(); 
            if (tecla == 'k' || tecla == 'K') { 
                std::cout << "\033[s\033[" << linhaMsg << ";1H\033[K\033[u" << texto.substr(i) << std::flush; 
                if (addNewline) {
                    std::cout << std::endl;
                }
                return;
            } 
        }
        std::cout << texto[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(atrasoMs));
    }

    std::cout << "\033[s\033[" << linhaMsg << ";1H\033[K\033[u" << std::flush;

    if (addNewline) {
        std::cout << std::endl;
    }
}

void Aparencia::imprimirVetorAnimado(const std::vector<std::string>& linhas, int atrasoMs) {
    for (const std::string& linha : linhas) {
        std::cout << linha << "\n";
        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(atrasoMs));
    }
}

void Aparencia::exibirPainel(
    const std::string& titulo, 
    Cor corPrincipal, 
    const std::vector<std::string>& artePrincipal, 
    int larguraArte, 
    const std::vector<std::string>& arteSecundaria, 
    Cor corSecundaria, 
    bool animarFadeIn
) {
    std::string tituloUpper = titulo;
    std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int larguraTerminal = obterLarguraTerminal();
    std::string linhaDivisoria = "";
    for(int i = 0; i < larguraTerminal; ++i) linhaDivisoria += "═";

    bool temArte = !artePrincipal.empty();
    bool temArteSecundaria = !arteSecundaria.empty();
    bool temTitulo = !tituloUpper.empty();

    if (animarFadeIn) {
        uint32_t val1 = static_cast<uint32_t>(corPrincipal);
        uint8_t r1 = (val1 >> 16) & 0xFF, g1 = (val1 >> 8) & 0xFF, b1 = val1 & 0xFF;
        
        uint32_t val2 = static_cast<uint32_t>(corSecundaria != Cor::RESET ? corSecundaria : corPrincipal);
        uint8_t r2 = (val2 >> 16) & 0xFF, g2 = (val2 >> 8) & 0xFF, b2 = val2 & 0xFF;

        int passos = 4;
        for (int step = 1; step <= passos; ++step) {
            double pct = static_cast<double>(step) / passos;
            std::string corRGB1 = "\033[38;2;" + std::to_string(static_cast<int>(r1 * pct)) + ";" + std::to_string(static_cast<int>(g1 * pct)) + ";" + std::to_string(static_cast<int>(b1 * pct)) + "m";
            std::string corRGB2 = "\033[38;2;" + std::to_string(static_cast<int>(r2 * pct)) + ";" + std::to_string(static_cast<int>(g2 * pct)) + ";" + std::to_string(static_cast<int>(b2 * pct)) + "m";
            
            std::ostringstream buffer;
            buffer << "\n\n";

            if (temArte) {
                int recuo = std::max(0, (larguraTerminal - larguraArte) / 2);
                std::string margem(recuo, ' ');
                for (size_t i = 0; i < artePrincipal.size(); ++i) {
                    buffer << margem << corRGB1 << artePrincipal[i];
                    if (temArteSecundaria && i < arteSecundaria.size()) {
                        buffer << corRGB2 << arteSecundaria[i];
                    }
                    buffer << "\n";
                }
                buffer << "\n" << corRGB1 << linhaDivisoria << "\n";
            }

            if (temTitulo) {
                buffer << espacosParaCentralizar(obterComprimentoVisual(tituloUpper)) << corRGB1 << tituloUpper << "\n";
                buffer << corRGB1 << linhaDivisoria << "\n";
            }
            buffer << "\033[0m";
            
            std::cout << "\033[?25l\033[H" << buffer.str() << std::flush;
            if (step < passos) std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } else {
        std::cout << "\n\n";
        
        if (temArte) {
            int recuo = std::max(0, (larguraTerminal - larguraArte) / 2);
            std::string margem(recuo, ' ');
            for (size_t i = 0; i < artePrincipal.size(); ++i) {
                std::cout << margem << cor(corPrincipal) << artePrincipal[i];
                if (temArteSecundaria && i < arteSecundaria.size()) {
                    std::cout << cor(corSecundaria) << arteSecundaria[i];
                }
                std::cout << cor(Cor::RESET) << "\n";
            }
            std::cout << "\n" << cor(corPrincipal) << linhaDivisoria << cor(Cor::RESET) << "\n";
        }

        if (temTitulo) {
            imprimirCentralizado(tituloUpper, cor(corPrincipal));
            std::cout << cor(corPrincipal) << linhaDivisoria << cor(Cor::RESET) << "\n";
        }
    }
}

void Aparencia::exibirPainelTexto(const std::string& titulo, Cor corDoCabecalho, bool animarFadeIn) {
    exibirPainel(titulo, corDoCabecalho, {}, 0, {}, Cor::RESET, animarFadeIn);
}

int Aparencia::imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda, int espacamento, Cor corEsquerda, Cor corDireita, int atrasoLinhaMs) {
    int larguraEsq = minLarguraEsquerda;
    for (const auto& s : colunaEsquerda) {
        if (obterComprimentoVisual(s) > larguraEsq) {
            larguraEsq = obterComprimentoVisual(s);
        }
    }
    
    int larguraDir = 0;
    for (const auto& s : colunaDireita) {
        if (obterComprimentoVisual(s) > larguraDir) {
            larguraDir = obterComprimentoVisual(s);
        }
    }

    int recuo = (obterLarguraTerminal() - (larguraEsq + espacamento + larguraDir)) / 2;
    if (recuo < 0) recuo = 0;

    size_t maxLinhas = std::max(colunaEsquerda.size(), colunaDireita.size());
    std::cout << "\n";
    for (size_t i = 0; i < maxLinhas; ++i) {
        std::cout << std::string(recuo, ' ');
        
        if (i < colunaEsquerda.size()) {
            std::string textoEsq = colunaEsquerda[i];
            int padding = larguraEsq - obterComprimentoVisual(textoEsq);
            std::cout << cor(corEsquerda) << textoEsq << std::string(padding > 0 ? padding : 0, ' ') << cor(Cor::RESET);
        } else {
            std::cout << std::string(larguraEsq, ' ');
        }
        
        std::cout << std::string(espacamento, ' ');
        
        if (i < colunaDireita.size()) {
            std::cout << cor(corDireita) << colunaDireita[i] << cor(Cor::RESET);
        }
        std::cout << "\n";
        
        if (atrasoLinhaMs > 0) {
            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(atrasoLinhaMs));
        }
    }
    
    return recuo;
}

void Aparencia::exibirPrompt(const std::string& mensagem) {
    std::cout << "\n" << espacosParaCentralizar(obterComprimentoVisual(mensagem)) << mensagem;
}

void Aparencia::exibirPainelArte(const std::vector<std::string>& arteAscii, int larguraVisual, Cor corDaArte, const std::string& tituloSecundario, bool animarFadeIn) {
    exibirPainel(tituloSecundario, corDaArte, arteAscii, larguraVisual, {}, Cor::RESET, animarFadeIn);
}

std::vector<std::string> Aparencia::reduzirEscalaAscii(const std::vector<std::string>& arteOriginal, int fatorX, int fatorY) {
    std::vector<std::string> arteReduzida;
    if (fatorX <= 0) fatorX = 1;
    if (fatorY <= 0) fatorY = 1;

    for (size_t i = 0; i < arteOriginal.size(); i += fatorY) {
        std::string novaLinha = "";
        for (size_t j = 0; j < arteOriginal[i].length(); j += fatorX) {
            novaLinha += arteOriginal[i][j];
        }
        arteReduzida.push_back(novaLinha);
    }
    return arteReduzida;
}

std::string Aparencia::margemCombate() {
    return espacosParaCentralizar(91); // Centraliza a partir do interior da HUD
}

void Aparencia::registrarLogBatalha(const std::string& texto) {
    historicoBatalha.push_back(texto);
}

void Aparencia::limparLogBatalha() {
    historicoBatalha.clear();
}

void Aparencia::exibirUltimosLogs(int quantidade) {
    if (historicoBatalha.empty()) return;
    int inicio = std::max(0, static_cast<int>(historicoBatalha.size()) - quantidade);
    int total = static_cast<int>(historicoBatalha.size()) - inicio;
    for (int i = 0; i < total; ++i) {
        int index = inicio + i;
        imprimirCentralizado(removerCoresANSI(historicoBatalha[index]), cor(Cor::CINZA));
    }
    std::cout << "\n";
}

void Aparencia::exibirHistoricoCompleto() {
    if (historicoBatalha.empty()) {
        limparTela();
        exibirPainelTexto("HISTORICO DE BATALHA", Cor::CIANO);
        imprimirCentralizado("O historico esta vazio.", cor(Cor::CINZA));
        std::cout << "\n";
        imprimirLinhaDivisoria();
        ControleDeInput::aguardarEnter();
        return;
    }

    int linhasPorPagina = std::max(5, obterAlturaTerminal() - 12);
    int totalPaginas = (static_cast<int>(historicoBatalha.size()) + linhasPorPagina - 1) / linhasPorPagina;
    int paginaAtual = totalPaginas - 1; // Inicia na pagina mais recente

    while (true) {
        limparTela();
        exibirPainelTexto("HISTORICO DE BATALHA", Cor::CIANO);

        int inicio = paginaAtual * linhasPorPagina;
        int fim = std::min(inicio + linhasPorPagina, static_cast<int>(historicoBatalha.size()));
        std::vector<std::string> pagina(historicoBatalha.begin() + inicio, historicoBatalha.begin() + fim);
        
        imprimirBlocoCentralizado(pagina);
        std::cout << "\n";
        imprimirLinhaDivisoria();
        imprimirCentralizado("Pagina " + std::to_string(paginaAtual + 1) + " de " + std::to_string(totalPaginas), cor(Cor::CIANO));
        
        if (totalPaginas > 1) {
            std::string escolha = ControleDeInput::lerEntradaProtegida("[A] Pagina Anterior | [D] Proxima Pagina | [0] Sair\n\nEscolha: ");
            if (escolha == "0") break;
            if ((escolha == "a" || escolha == "A") && paginaAtual > 0) paginaAtual--;
            if ((escolha == "d" || escolha == "D") && paginaAtual < totalPaginas - 1) paginaAtual++;
        } else {
            std::cout << "\n";
            ControleDeInput::aguardarEnter();
            break;
        }
    }
    std::cout << "\n";
}
