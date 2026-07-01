#include "ConversorString.h"
#include "Aparencia.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <sstream>

#include "ControleDeInput.h"
#include "../../Visoes/GerenciadorVisao.h"

#ifdef _WIN32
    #include <windows.h>
    #include <timeapi.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
#include "ControleDeInput.h"
#include <fstream>


// Variável global para padronização da compressão de sprites
int Aparencia::FATOR_COMPRESSAO_GLOBAL = 2;

namespace {
    int popupMinLarguraAtual = 0;
    int popupMinAlturaAtual = 0;

    std::vector<std::string> historicoBatalha;

#ifdef _WIN32
    struct TimerResolutionHelper {
        TimerResolutionHelper() {
            timeBeginPeriod(1);
        }
        ~TimerResolutionHelper() {
            timeEndPeriod(1);
        }
    };
    TimerResolutionHelper g_timerResolutionHelper;
#endif
}

int Aparencia::atrasoDigitacaoMS = 25; // Inicialização da velocidade padrão (50ms)
Cor Aparencia::corFundoAtiva = Cor::RESET;

void Aparencia::inicializarConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Configura UTF-8 globalmente apenas uma vez
    // Habilita interpretacao de codigos ANSI nativamente no console do Windows e desabilita quebra de linha automatica
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        dwMode &= ~0x0002; // Desabilita ENABLE_WRAP_AT_EOL_OUTPUT para evitar que o console quebre linhas longas (ex: artes ASCII grandes)
        SetConsoleMode(hOut, dwMode);
    }
    // Desabilita autowrap via sequencia ANSI como seguranca adicional (especialmente util para o Windows Terminal)
    std::cout << "\033[?7l";
#else
    std::cout << "\033[?7l";
#endif
}

std::vector<std::string> Aparencia::carregarArte(const std::string& caminhoArquivo) {
    std::vector<std::string> arte;
    
    // Tenta diferentes diretorios base para encontrar o arquivo
    std::vector<std::string> caminhosBase = {
        "",
        "../",
        "../../",
        "../../../"
    };

    std::ifstream arquivo;
    std::string caminhoSucesso;
    for (const auto& base : caminhosBase) {
        arquivo.open(base + caminhoArquivo);
        if (arquivo.is_open()) {
            caminhoSucesso = base + caminhoArquivo;
            break;
        }
    }

    if (arquivo.is_open()) {
        std::string linha;
        while (std::getline(arquivo, linha)) {
            // Remove \r no final da linha (caso o arquivo txt tenha sido salvo com CRLF)
            if (!linha.empty() && linha.back() == '\r') {
                linha.pop_back();
            }
            arte.push_back(linha);
        }
    } else {
        arte.push_back("ERRO: Nao foi possivel carregar a arte.");
        arte.push_back("Caminho procurado: " + caminhoArquivo);
    }
    return arte;
}

std::string Aparencia::cor(Cor codigo) {
    if (codigo == Cor::RESET) {
        if (corFundoAtiva != Cor::RESET) {
            return "\033[0m" + cor(corFundoAtiva);
        }
        return "\033[0m";
    }
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

    if (codigo == Cor::RESET) {
        if (corFundoAtiva != Cor::RESET) {
            return "\033[" + estiloStr + "0m" + cor(corFundoAtiva);
        }
        return "\033[" + estiloStr + "0m";
    }
    if (codigo == Cor::NEGRITO) return "\033[" + estiloStr + "1m";

    uint32_t val = static_cast<uint32_t>(codigo);
    bool isBg = (val & 0x80000000) != 0;
    uint8_t r = (val >> 16) & 0xFF;
    uint8_t g = (val >> 8) & 0xFF;
    uint8_t b = val & 0xFF;

    std::string type = isBg ? "48;2;" : "38;2;";
    return "\033[" + estiloStr + type + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::corRGB(uint8_t r, uint8_t g, uint8_t b, bool negrito) {
    std::string estiloStr = negrito ? "1;" : "0;";
    return "\033[" + estiloStr + "38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string Aparencia::bgRGB(uint8_t r, uint8_t g, uint8_t b) {
    return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

void Aparencia::maximizarJanelaTerminal() {
#ifdef _WIN32
    // 1. Ajusta a fonte ANTES de maximizar, para garantir colunas/linhas suficientes
    ajustarFonteParaResolucao();

    HWND hwnd = GetConsoleWindow();

    // Define o estilo da janela para "popup" (sem bordas, sem barra de titulo)
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

    // Maximiza a janela popup, que por padrao ocupa a tela inteira
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void Aparencia::ajustarFonteParaResolucao() {
#ifdef _WIN32
    // Resolucao de referencia: 1920x1080 com fonte Consolas tamanho 16 produz ~238 colunas x ~60 linhas.
    // Em telas menores, precisamos reduzir a fonte proporcionalmente para manter essas dimensoes.
    
    int larguraTela = GetSystemMetrics(SM_CXSCREEN);
    int alturaTela = GetSystemMetrics(SM_CYSCREEN);
    
    // Se a resolucao eh igual ou maior que a referencia, nao precisa ajustar
    if (larguraTela >= 1920 && alturaTela >= 1080) return;
    
    // Calcula o fator de escala baseado na menor dimensao proporcional
    // (garante que tanto largura quanto altura fiquem dentro)
    double escalaX = static_cast<double>(larguraTela) / 1920.0;
    double escalaY = static_cast<double>(alturaTela) / 1080.0;
    double escala = std::min(escalaX, escalaY);
    
    // Tamanho da fonte de referencia (o que funciona em 1920x1080)
    const int FONTE_REFERENCIA = 16;
    
    // Calcula o novo tamanho proporcional, arredondando para baixo para garantir que cabe
    int novoTamanho = static_cast<int>(FONTE_REFERENCIA * escala);
    
    // Limites de seguranca: minimo 8 (legivel), maximo 16 (referencia)
    if (novoTamanho < 8) novoTamanho = 8;
    if (novoTamanho >= FONTE_REFERENCIA) return; // Ja esta bom, nao muda
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = {};
    fontInfo.cbSize = sizeof(fontInfo);
    fontInfo.dwFontSize.X = 0; // Largura automatica baseada na altura
    fontInfo.dwFontSize.Y = static_cast<SHORT>(novoTamanho);
    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(fontInfo.FaceName, L"Consolas");
    
    SetCurrentConsoleFontEx(hOut, FALSE, &fontInfo);
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
    if (corFundoAtiva != Cor::RESET) {
        std::cout << cor(corFundoAtiva);
    }
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
}

int Aparencia::obterLarguraTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1; // +1 restaurado. Line wrap disable permite uso da largura exata do monitor
    }
#endif
    return 120; // 120 e mais padrao que 119
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

std::string Aparencia::gerarBarraGradiente(double pct, int tamanho, Cor corFinal) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 1.0) pct = 1.0;
    int qtdReal = static_cast<int>(pct * tamanho * 8);
    std::string barra = "";
    for (int i = 0; i < tamanho; ++i) {
        int intensidade = 130 + (125 * i) / std::max(1, tamanho - 1);
        std::string corAtual = Aparencia::obterCorRGBFade(corFinal, intensidade);
        int charIdx = i * 8;
        if (qtdReal >= charIdx + 4) barra += corAtual + "█";
        else barra += Aparencia::cor(Cor::CINZA) + "░";
    }
    return barra;
}

std::string Aparencia::espacosParaCentralizar(int comprimentoTexto) {
    int espacos = (obterLarguraTerminal() - comprimentoTexto) / 2;
    if (espacos < 0) espacos = 0;
    return std::string(espacos, ' ');
}

std::string Aparencia::centralizarTexto(const std::string& texto) {
    size_t end = texto.find_last_not_of(' ');
    std::string trimmed = (end != std::string::npos) ? texto.substr(0, end + 1) : "";
    return espacosParaCentralizar(obterComprimentoVisual(trimmed)) + texto;
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

    bool isEngineIDE = !GerenciadorVisao::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        std::string tituloIDE = titulo.empty() ? "Info" : titulo;
        std::replace(tituloIDE.begin(), tituloIDE.end(), ' ', '_'); // Remove espaços
        
        caixa.push_back("\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176m" + tituloIDE + "\033[0m {");
        for (const auto& linha : linhas) {
            std::string linhaLimpa = linha;
            if (linhaLimpa.find(":") != std::string::npos) {
                // Tenta estilizar chave-valor como tipo variavel = valor
                size_t pos = linhaLimpa.find(":");
                std::string chave = linhaLimpa.substr(0, pos);
                std::string valor = linhaLimpa.substr(pos + 1);
                
                // Limpa espaços da chave para nome de variável
                std::string chaveVar = removerCoresANSI(chave);
                chaveVar.erase(std::remove(chaveVar.begin(), chaveVar.end(), ' '), chaveVar.end());
                
                linhaLimpa = "    \033[38;2;86;156;214mauto\033[0m " + chaveVar + " = " + valor + ";";
            } else {
                linhaLimpa = "    " + linha + ";";
            }
            
            // Garantir que a linha tenha padding para manter a larguraMinima se necessário (opcional no IDE)
            int comp = obterComprimentoVisual(linhaLimpa);
            int padding = maxLargura - comp;
            if (padding > 0) linhaLimpa += std::string(padding, ' ');
            
            caixa.push_back(linhaLimpa);
        }
        caixa.push_back("};");
        return caixa;
    }

    std::string padBg = !isEngineIDE ? "\033[48;2;0;0;0m" : "";

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
    caixa.push_back(padBg + corStr + top + resetStr);

    for (const auto& linha : linhas) {
        int comp = obterComprimentoVisual(linha);
        int padding = maxLargura - comp;
        caixa.push_back(padBg + corStr + "║ " + resetStr + padBg + linha + padBg + std::string(padding > 0 ? padding : 0, ' ') + corStr + padBg + " ║" + resetStr);
    }

    std::string bottom = "╚";
    for (int i = 0; i < maxLargura + 2; ++i) {
        bottom += "═";
    }
    bottom += "╝";
    caixa.push_back(padBg + corStr + bottom + resetStr);

    if (!isEngineIDE) {
        for (auto& c : caixa) {
            std::string toReplace = "\033[0m";
            std::string replaceWith = "\033[0m\033[48;2;0;0;0m";
            size_t pos = c.find(toReplace);
            while (pos != std::string::npos) {
                c.replace(pos, toReplace.length(), replaceWith);
                pos = c.find(toReplace, pos + replaceWith.length());
            }
            c += "\033[0m"; // Reset no final da linha
        }
    }

    return caixa;
}

void Aparencia::imprimirLinhaDivisoria(char caractere) {
    std::string linha = "";
    int largura = obterLarguraTerminal();
    
    bool isEngineIDE = !GerenciadorVisao::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        linha = "\033[38;2;96;139;78m// ";
        for (int i = 0; i < largura - 3; ++i) linha += "=";
        linha += "\033[0m";
        std::cout << linha << "\n";
        return;
    }

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
    size_t end = texto.find_last_not_of(' ');
    std::string trimmed = (end != std::string::npos) ? texto.substr(0, end + 1) : "";
    std::cout << "\033[0m" << espacosParaCentralizar(obterComprimentoVisual(trimmed)) << corAnsi << texto << "\033[0m\n";
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
    uint32_t val = static_cast<uint32_t>(corTema);
    uint8_t baseR = (val >> 16) & 0xFF;
    uint8_t baseG = (val >> 8) & 0xFF;
    uint8_t baseB = val & 0xFF;
    
    int r = (baseR * intensidade) / 255;
    int g = (baseG * intensidade) / 255;
    int b = (baseB * intensidade) / 255;

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

void Aparencia::exibirTelaIntro(const std::vector<std::string>& arteLogo, const std::vector<std::string>& textoNarracao, Cor corTema) {
    int larguraConsole = obterLarguraTerminal();

    // Calcula margem para arte
    int larguraArte = 0;
    for (const auto& linha : arteLogo) {
        int comp = obterComprimentoVisual(linha);
        if (comp > larguraArte) larguraArte = comp;
    }
    int espacosArte = std::max(0, (larguraConsole - larguraArte) / 2);
    std::string margemArte(espacosArte, ' ');

    int linhasEmBrancoAntesDoTexto = 3;

    ocultarCursor();
    limparTela();

    animarFadeIn(30, 30, [&](int /*frame*/, int intensidade) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        
        std::string corRGB = obterCorRGBFade(corTema, intensidade);
        
        buffer << "\n\n";
        for (const auto& linha : arteLogo) {
            buffer << margemArte << corRGB << linha << "\033[0m\n";
        }
        
        buffer << "\033[J"; // Limpa o restante da tela abaixo
        
        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });

    for(int i = 0; i < linhasEmBrancoAntesDoTexto; ++i) std::cout << "\n";

    imprimirBlocoCentralizadoDigitando(textoNarracao);
    
    std::cout << "\n";
    std::string pressEnter = "[ PRESSIONE ENTER PARA INICIAR ]";
    imprimirCentralizado(pressEnter, cor(Cor::CINZA));
    
    ControleDeInput::aguardarEnter();

    limparTela();
}

void Aparencia::imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi, int atrasoLinhaMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        size_t end = linha.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? linha.substr(0, end + 1) : "";
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(trimmed));
    }
    imprimirCentralizadoMultilinha(linhas, tamanhoDaLinhaMaisLonga, corAnsi, atrasoLinhaMs);
}

void Aparencia::imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        size_t end = linha.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? linha.substr(0, end + 1) : "";
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(trimmed));
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
    std::cout << "\033[s\033[" << linhaMsg << ";1H" << cor(Cor::NEGRITO, Cor::CINZA) << "[Pressione ENTER para pular]" << cor(Cor::RESET) << "\033[u";

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
            if (tecla == '\r' || tecla == '\n') { 
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

    bool isEngineIDE = !GerenciadorVisao::obterInstancia().isVisao3DAtiva();

    if (isEngineIDE) {
        std::cout << "\n\n";
        std::string classDef = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176m" + tituloUpper + "\033[0m {";
        std::string publ = "\033[38;2;86;156;214mpublic:\033[0m";
        
        if (!artePrincipal.empty()) {
            int recuo = std::max(0, (larguraTerminal - larguraArte) / 2);
            std::string margem(recuo, ' ');
            std::cout << margem << "\033[38;2;96;139;78m/* ==========================================================\033[0m\n";
            for (size_t i = 0; i < artePrincipal.size(); ++i) {
                std::cout << margem << "\033[38;2;96;139;78m * \033[0m" << artePrincipal[i];
                if (!arteSecundaria.empty() && i < arteSecundaria.size()) {
                    std::cout << arteSecundaria[i];
                }
                std::cout << "\n";
            }
            std::cout << margem << "\033[38;2;96;139;78m ========================================================== */\033[0m\n\n";
        }
        
        imprimirCentralizado(classDef);
        std::cout << "\n";
        imprimirCentralizado(publ);
        std::cout << "\n";
        return;
    }

    bool temArte = !artePrincipal.empty();
    bool temArteSecundaria = !arteSecundaria.empty();
    bool temTitulo = !tituloUpper.empty();

    if (animarFadeIn) {
        int targetY = 2;

        // Pre-renderiza as linhas finais para usar no fade
        std::vector<std::string> linhasFinais;
        if (temArte) {
            int recuo = std::max(0, (larguraTerminal - larguraArte) / 2);
            std::string margem(recuo, ' ');
            for (size_t i = 0; i < artePrincipal.size(); ++i) {
                std::string linha = margem + cor(corPrincipal) + artePrincipal[i];
                if (temArteSecundaria && i < arteSecundaria.size()) {
                    linha += cor(corSecundaria) + arteSecundaria[i];
                }
                linhasFinais.push_back(linha);
            }
            linhasFinais.push_back(cor(corPrincipal) + linhaDivisoria);
        }
        if (temTitulo) {
            linhasFinais.push_back(espacosParaCentralizar(obterComprimentoVisual(tituloUpper)) + cor(corPrincipal) + tituloUpper);
            linhasFinais.push_back(cor(corPrincipal) + linhaDivisoria);
        }

        // Fade in lento sem trailing \n (evita scroll bounce)
        Aparencia::animarFadeIn(30, 40, [&](int /*frame*/, int intensidade) {
            float pct = intensidade / 255.0f;
            std::ostringstream buffer;
            for (size_t i = 0; i < linhasFinais.size(); ++i) {
                int y = targetY + static_cast<int>(i);
                buffer << "\033[" << (y + 1) << ";1H" << fadarLinhaAnsi(linhasFinais[i], pct) << "\033[K";
                if (i < linhasFinais.size() - 1) buffer << "\n";
            }
            buffer << "\033[0m";
            std::cout << buffer.str() << std::flush;
        });
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

void Aparencia::exibirTituloPadrao(const std::string& titulo, Cor corTema) {
    std::string tituloUpper = titulo;
    std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int larguraTerminal = obterLarguraTerminal();
    std::string linhaDivisoriaStr = "";
    for(int i = 0; i < larguraTerminal; ++i) linhaDivisoriaStr += "═";

    std::string linhaTitulo = espacosParaCentralizar(obterComprimentoVisual(tituloUpper)) + cor(corTema) + tituloUpper + cor(Cor::RESET);
    std::string linhaDiv = cor(corTema) + linhaDivisoriaStr + cor(Cor::RESET);

    std::vector<std::string> linhasFinais = { linhaTitulo, linhaDiv };
    int targetY = 2;

    // Fade in lento sem trailing \n (evita scroll bounce)
    Aparencia::animarFadeIn(30, 40, [&](int /*frame*/, int intensidade) {
        float pct = intensidade / 255.0f;
        std::ostringstream buffer;
        for (size_t i = 0; i < linhasFinais.size(); ++i) {
            int y = targetY + static_cast<int>(i);
            buffer << "\033[" << (y + 1) << ";1H" << fadarLinhaAnsi(linhasFinais[i], pct) << "\033[K";
            if (i < linhasFinais.size() - 1) buffer << "\n";
        }
        buffer << "\033[0m";
        std::cout << buffer.str() << std::flush;
    });

    // Renderiza final sem trailing \n
    for (size_t i = 0; i < linhasFinais.size(); ++i) {
        int y = targetY + static_cast<int>(i);
        std::cout << "\033[" << (y + 1) << ";1H" << linhasFinais[i] << "\033[K";
        if (i < linhasFinais.size() - 1) std::cout << "\n";
    }
    std::cout << "\033[J" << std::flush;
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

void Aparencia::iniciarInteracaoPopup() {
    popupMinLarguraAtual = 0;
    popupMinAlturaAtual = 0;
}
void Aparencia::atualizarMinTamanhoPopup(int largura, int altura) {
    if (largura > popupMinLarguraAtual) popupMinLarguraAtual = largura;
    if (altura > popupMinAlturaAtual) popupMinAlturaAtual = altura;
}
int Aparencia::obterMinLarguraPopup() { return popupMinLarguraAtual; }
int Aparencia::obterMinAlturaPopup() { return popupMinAlturaAtual; }

void Aparencia::exibirPopup(const std::string& titulo, const std::vector<std::string>& texto, Cor corTema, const std::vector<std::string>& arteOriginal) {
    std::vector<std::string> arte = arteOriginal;
    if (static_cast<int>(arte.size()) > 10) {
        arte = reduzirEscalaAscii(arteOriginal, FATOR_COMPRESSAO_GLOBAL, FATOR_COMPRESSAO_GLOBAL);
    }

    int larguraArte = 0;
    for (const auto& l : arte) {
        int len = obterComprimentoVisual(l);
        if (len > larguraArte) larguraArte = len;
    }

    std::vector<std::string> linhasTexto = texto;
    
    int minBoxHeight = Aparencia::obterMinAlturaPopup() - 2;
    int minTotalWidth = Aparencia::obterMinLarguraPopup() - 4;
    if (minBoxHeight < 0) minBoxHeight = 0;
    if (minTotalWidth < 0) minTotalWidth = 0;

    // Preenche com espacos vazios na vertical para garantir o tamanho minimo e sobrepor popups velhos
    // Considerando que vamos adicionar 2 linhas no final ("" e o prompt de ENTER)
    while (std::max(arte.size(), linhasTexto.size() + 2) < static_cast<size_t>(minBoxHeight)) {
        linhasTexto.push_back("");
    }
    linhasTexto.push_back("");
    linhasTexto.push_back(cor(Cor::CINZA) + "[ Pressione ENTER para continuar ]" + cor(Cor::RESET));

    int larguraTexto = 0;
    for (const auto& l : linhasTexto) {
        int len = obterComprimentoVisual(l);
        if (len > larguraTexto) larguraTexto = len;
    }

    int totalWidth = larguraArte + (larguraArte > 0 ? 3 : 0) + larguraTexto; 
    
    // Expande a largura de texto para garantir que a janela fique com a largura ideal padrao
    if (totalWidth < minTotalWidth) {
        larguraTexto += (minTotalWidth - totalWidth);
        totalWidth = minTotalWidth;
    }

    int boxHeight = std::max(static_cast<int>(arte.size()), static_cast<int>(linhasTexto.size()));

    std::vector<std::string> caixa;
    std::string corStr = cor(corTema);
    std::string resetStr = cor(Cor::RESET);

    std::string top = "╔";
    int tituloLen = obterComprimentoVisual(titulo);
    
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = totalWidth + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) top += "═";
    } else {
        for (int i = 0; i < totalWidth + 2; ++i) top += "═";
    }
    top += "╗";
    caixa.push_back(corStr + top + resetStr);

    for (int i = 0; i < boxHeight; ++i) {
        std::string linhaArte = (i < static_cast<int>(arte.size())) ? arte[i] : "";
        int padArte = larguraArte - obterComprimentoVisual(linhaArte);
        std::string artePart = linhaArte + std::string(padArte > 0 ? padArte : 0, ' ');

        std::string linhaTexto = (i < static_cast<int>(linhasTexto.size())) ? linhasTexto[i] : "";
        int padTexto = larguraTexto - obterComprimentoVisual(linhaTexto);
        std::string textoPart = linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ');

        if (larguraArte > 0) {
            caixa.push_back(corStr + "║ " + resetStr + artePart + corStr + " ║ " + resetStr + textoPart + corStr + " ║" + resetStr);
        } else {
            caixa.push_back(corStr + "║ " + resetStr + textoPart + corStr + " ║" + resetStr);
        }
    }

    std::string bottom = "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝";
    caixa.push_back(corStr + bottom + resetStr);

    int finalBoxWidth = obterComprimentoVisual(caixa[0]);
    int finalBoxHeight = caixa.size();
    Aparencia::atualizarMinTamanhoPopup(finalBoxWidth, finalBoxHeight);
    
    int larguraTerm = obterLarguraTerminal();
    int alturaTerm = obterAlturaTerminal();
    int startX = (larguraTerm - finalBoxWidth) / 2;
    int startY = (alturaTerm - finalBoxHeight) / 2;
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    
    renderizarCaixaPopupAnimada(caixa, startX, startY, true);
    
    ControleDeInput::limparBuffer();
    while (true) {
        char c = ControleDeInput::lerTecla();
        if (c == '\r' || c == '\n') break;
    }
}

void Aparencia::renderizarCaixaPopupAnimada(const std::vector<std::string>& caixa, int startX, int startY, bool animar) {
    if (caixa.empty()) return;
    int finalBoxHeight = caixa.size();
    std::string bgPopup = "\033[48;2;15;15;15m"; 
    
    auto formatarLinha = [&](const std::string& linhaOriginal) {
        std::string linha = linhaOriginal;
        size_t pos = 0;
        while ((pos = linha.find("\033[0m", pos)) != std::string::npos) {
            linha.replace(pos, 4, "\033[0m" + bgPopup);
            pos += 4 + bgPopup.length(); 
        }
        return bgPopup + linha + "\033[0m"; 
    };

    if (animar) {
        int metade = finalBoxHeight / 2;
        for (int expansao = 0; expansao <= metade; expansao++) {
            int inicioSlice = metade - expansao;
            int fimSlice = metade + expansao;
            if (fimSlice >= finalBoxHeight) fimSlice = finalBoxHeight - 1;

            moverCursor(startX, startY + inicioSlice);
            std::cout << formatarLinha(caixa[0]);

            for (int i = inicioSlice + 1; i < fimSlice; i++) {
                moverCursor(startX, startY + i);
                std::cout << formatarLinha(caixa[i]);
            }

            if (fimSlice > inicioSlice) {
                moverCursor(startX, startY + fimSlice);
                std::cout << formatarLinha(caixa[finalBoxHeight - 1]);
            }

            std::cout << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    }

    for (int i = 0; i < finalBoxHeight; ++i) {
        moverCursor(startX, startY + i);
        std::cout << formatarLinha(caixa[i]);
    }
    std::cout << std::flush;
}

int Aparencia::lerInteiroEmPopupFlutuante(const std::string& mensagem, int limiteMin, int limiteMax, Cor corTema) {
    int termW = obterLarguraTerminal();
    int termH = obterAlturaTerminal();
    int msgLen = obterComprimentoVisual(mensagem);
    int boxW = msgLen + 8;
    
    int startX = std::max(0, (termW - boxW) / 2);
    int startY = std::max(0, termH / 2);
    
    std::string corBox = cor(corTema);
    std::string reset = cor(Cor::RESET);
    std::string bg = "\033[48;2;15;15;15m";

    moverCursor(startX, startY - 1);
    std::string top = "╔"; for(int i = 0; i < boxW - 2; ++i) top += "═"; top += "╗";
    std::cout << bg << corBox << top << reset;
    
    moverCursor(startX, startY);
    std::cout << bg << corBox << "║ " << reset << bg << mensagem << "    " << corBox << "║" << reset;
    
    moverCursor(startX, startY + 1);
    std::string bottom = "╚"; for(int i = 0; i < boxW - 2; ++i) bottom += "═"; bottom += "╝";
    std::cout << bg << corBox << bottom << reset;

    moverCursor(startX + 2 + msgLen, startY);
    std::cout << bg << reset;
    int resultado = ControleDeInput::lerInteiroComLimites("", limiteMin, limiteMax, false);
    std::cout << "\033[0m";
    return resultado;
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
    int larguraHUD = 91; // Largura visual aproximada do interior da HUD de combate
    int larguraTerminal = obterLarguraTerminal();
    // Se o terminal for menor que a HUD, nao adiciona margem
    int larguraRef = std::min(larguraHUD, larguraTerminal);
    return espacosParaCentralizar(larguraRef);
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

void Aparencia::ordenarAlfabeticamente(std::vector<std::string>& lista) {
    std::sort(lista.begin(), lista.end());
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


void Aparencia::padronizarTamanhoVetor(std::vector<std::string>& linhas) {
    if (linhas.empty()) return;
    size_t maxLen = 0;
    for (const auto& l : linhas)
        if (l.length() > maxLen) maxLen = l.length();
    for (auto& l : linhas)
        if (l.length() < maxLen) l.append(maxLen - l.length(), ' ');
}

std::string Aparencia::sobreporPainelNaLinhaAnsi(const std::string& backgroundLine, const std::string& panelLine, int startX) {
    std::string result = "";
    result.reserve(backgroundLine.size() + panelLine.size() + 50);

    int panelWidth = Aparencia::obterComprimentoVisual(panelLine);
    int endX = startX + panelWidth;

    int visualX = 0;
    size_t i = 0;

    std::string currentBg = "";
    std::string currentFg = "";

    while (i < backgroundLine.size() && visualX < startX) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                result += esc;
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        result += backgroundLine.substr(i, len);
        visualX++;
        i += len;
    }

    while (visualX < startX) {
        result += " ";
        visualX++;
    }

    result += panelLine;

    while (i < backgroundLine.size() && visualX < endX) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        visualX++;
        i += len;
    }

    result += "\033[0m" + currentBg + currentFg;

    if (i < backgroundLine.size()) {
        result += backgroundLine.substr(i);
    }

    return result;
}

std::string Aparencia::sobreporLogoAnsi(const std::string& backgroundLine, const std::vector<std::string>& logoChars, int startX, const std::string& fgColor, int larguraTerminal) {
    std::string result = "";
    result.reserve(backgroundLine.size() + 200);

    std::string currentBg = "";
    std::string currentFg = "";

    int visualX = 0;
    size_t i = 0;

    while (i < backgroundLine.size() && visualX < larguraTerminal) {
        if (backgroundLine[i] == '\033' && i + 1 < backgroundLine.size() && backgroundLine[i+1] == '[') {
            size_t end = backgroundLine.find('m', i);
            if (end != std::string::npos) {
                std::string esc = backgroundLine.substr(i, end - i + 1);
                if (esc == "\033[0m") {
                    currentBg = "";
                    currentFg = "";
                } else if (esc.find("\033[48;2;") == 0 || esc == "\033[49m") {
                    currentBg = (esc == "\033[49m") ? "" : esc;
                } else if (esc.find("\033[38;2;") == 0 || esc == "\033[39m" || esc == "\033[1;37m" || esc == "\033[1;31m") {
                    currentFg = (esc == "\033[39m") ? "" : esc;
                }
                result += esc;
                i = end + 1;
                continue;
            }
        }

        int len = 1;
        unsigned char c = static_cast<unsigned char>(backgroundLine[i]);
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        std::string charStr = backgroundLine.substr(i, len);

        int logoCol = visualX - startX;
        if (logoCol >= 0 && logoCol < static_cast<int>(logoChars.size()) && logoChars[logoCol] != " ") {
            result += "\033[0m" + currentBg + fgColor + logoChars[logoCol] + "\033[0m" + currentBg + currentFg;
        } else {
            result += charStr;
        }

        visualX++;
        i += len;
    }
    return result;
}


void Aparencia::animarTransicaoCena3D(
    const std::vector<std::string>& logoBase,
    const std::string& corFinalLogo,
    const std::vector<std::string>& fundoLivre,
    std::function<std::vector<std::string>(float)> getFundoFinal,
    int targetY
) {
    int larguraTerminal = obterLarguraTerminal();
    int altura3D = static_cast<int>(fundoLivre.size());
    int logoHeight = static_cast<int>(logoBase.size());
    
    int logoWidth = 0;
    for(const auto& l : logoBase) {
        logoWidth = std::max(logoWidth, obterComprimentoVisual(l));
    }
    int logoX = (larguraTerminal - logoWidth) / 2;
    if (logoX < 0) logoX = 0;

    std::vector<std::vector<std::string>> decomposedLogo(logoHeight);
    for (int i = 0; i < logoHeight; i++) {
        const std::string& logoRow = logoBase[i];
        for (size_t j = 0; j < logoRow.length(); ) {
            int len = 1;
            unsigned char c = logoRow[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            decomposedLogo[i].push_back(logoRow.substr(j, len));
            j += len;
        }
    }

    // Renderiza cena completa uma unica vez (sem logo, com todos elementos em opacidade 100%)
    std::vector<std::string> cenaBase = getFundoFinal(1.0f);
    {
        std::ostringstream buffer;
        buffer << "\033[H";
        for (int y = 0; y < altura3D; y++) {
            buffer << cenaBase[y];
            if (y < altura3D - 1) buffer << "\n";
        }
        std::cout << buffer.str() << std::flush;
    }

    // Fade in apenas o logo (sem redesenharl a cena inteira)
    Aparencia::animarFadeIn(30, 40, [&](int frame, int /*intensidade*/) {
        float opacity = frame / 30.0f;
        
        std::string fadedCorLogo = corFinalLogo;
        if (corFinalLogo == "\033[1;31m") {
            fadedCorLogo = "\033[38;2;" + std::to_string((int)(255 * opacity)) + ";0;0m";
        } else if (corFinalLogo == "\033[1;32m") {
            fadedCorLogo = "\033[38;2;0;" + std::to_string((int)(255 * opacity)) + ";0m";
        } else if (corFinalLogo == "\033[1;37m" || corFinalLogo == "\033[37m") {
            int c = (int)(255 * opacity);
            fadedCorLogo = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
        }
        
        std::ostringstream buffer;
        for (int y = targetY; y < targetY + logoHeight && y < altura3D; y++) {
            int logoRowIdx = y - targetY;
            std::string linhaLogo = sobreporLogoAnsi(cenaBase[y], decomposedLogo[logoRowIdx], logoX, fadedCorLogo, larguraTerminal);
            buffer << "\033[" << (y + 1) << ";1H" << linhaLogo << "\033[K";
        }
        std::cout << buffer.str() << std::flush;
    });

}

std::string Aparencia::fadarLinhaAnsi(const std::string& linha, float ratio) {
    std::string res;
    res.reserve(linha.size());
    size_t i = 0;
    while (i < linha.size()) {
        if (i + 7 < linha.size() && linha[i] == '\033' && linha[i+1] == '[') {
            bool isBg = false;
            bool isFg = false;
            size_t startColor = i + 2;
            if (linha.compare(startColor, 5, "48;2;") == 0) {
                isBg = true;
            } else if (linha.compare(startColor, 5, "38;2;") == 0) {
                isFg = true;
            }
            
            if (isBg || isFg) {
                size_t p = startColor + 5;
                int r = 0, g = 0, b = 0;
                while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                    r = r * 10 + (linha[p] - '0');
                    p++;
                }
                if (p < linha.size() && linha[p] == ';') {
                    p++;
                    while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                        g = g * 10 + (linha[p] - '0');
                        p++;
                    }
                    if (p < linha.size() && linha[p] == ';') {
                        p++;
                        while (p < linha.size() && linha[p] >= '0' && linha[p] <= '9') {
                            b = b * 10 + (linha[p] - '0');
                            p++;
                        }
                    }
                }
                
                if (p < linha.size() && linha[p] == 'm') {
                    int fr = static_cast<int>(r * ratio);
                    int fg = static_cast<int>(g * ratio);
                    int fb = static_cast<int>(b * ratio);
                    res += "\033[";
                    if (isBg) res += "48;2;";
                    else res += "38;2;";
                    res += std::to_string(fr) + ";" + std::to_string(fg) + ";" + std::to_string(fb) + "m";
                    i = p + 1;
                    continue;
                }
            }
        }
        res.push_back(linha[i]);
        i++;
    }
    return res;
}
