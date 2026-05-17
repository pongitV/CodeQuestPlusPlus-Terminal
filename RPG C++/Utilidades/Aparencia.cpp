#include "Aparencia.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>

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

void Aparencia::exibirCabecalho(const std::string& titulo, Cor corDoCabecalho) {
    std::string tituloUpper = titulo;
    std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int largura = obterLarguraTerminal();
    std::string linha = "";
    for(int i = 0; i < largura; ++i) linha += "═";
    
    std::cout << cor(corDoCabecalho) << linha << cor(Cor::RESET) << "\n\n";
    imprimirCentralizado(tituloUpper, cor(corDoCabecalho));
    std::cout << "\n" << cor(corDoCabecalho) << linha << cor(Cor::RESET) << "\n";
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

void Aparencia::exibirLogoAscii(const std::vector<std::string>& arteAscii, int larguraVisual, Cor corDaArte, const std::string& tituloSecundario, int atrasoLinhaMs) {
    std::cout << "\n";
    imprimirLinhaDivisoria();
    std::cout << "\n";
    imprimirCentralizadoMultilinha(arteAscii, larguraVisual, cor(corDaArte), atrasoLinhaMs);
    std::cout << "\n";
    imprimirLinhaDivisoria();
    
    if (!tituloSecundario.empty()) {
        imprimirCentralizado(tituloSecundario);
        imprimirLinhaDivisoria();
    }
    std::cout << "\n";
}

std::vector<std::string> Aparencia::reduzirEscalaAscii(const std::vector<std::string>& arteOriginal) {
    std::vector<std::string> arteReduzida;
    for (size_t i = 0; i < arteOriginal.size(); i += 2) {
        std::string novaLinha = "";
        for (size_t j = 0; j < arteOriginal[i].length(); j += 2) {
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
        exibirCabecalho("HISTORICO DE BATALHA", Cor::CIANO);
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
        exibirCabecalho("HISTORICO DE BATALHA", Cor::CIANO);

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

void Aparencia::imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::string& texto, bool novaLinhaAntes, bool novaLinhaDepois) {
    if (novaLinhaAntes) {
        std::cout << "\n";
        // Imprime a tag colorida, depois reseta a cor
        std::cout << Aparencia::cor(npcCor) << "[" << npcNome << "]: " << Aparencia::cor(Cor::RESET);
        // Imprime o texto com a cor padrão
        Aparencia::imprimirDigitando(texto, Aparencia::atrasoDigitacaoMS, novaLinhaDepois);
    } else {
        // Calcula o preenchimento para alinhar com o texto da primeira linha e o imprime
        std::string tag = "[" + npcNome + "]: ";
        std::cout << std::string(tag.length(), ' ');
        // Imprime o texto com a cor padrão
        Aparencia::imprimirDigitando(texto, Aparencia::atrasoDigitacaoMS, novaLinhaDepois);
    }
}

void Aparencia::imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::vector<std::string>& linhas) {
    if (linhas.empty()) return;
    
    // A primeira linha imprime a quebra de linha inicial e o Nome
    imprimirDialogoNPC(npcNome, npcCor, linhas[0], true, true);
    
    // As linhas subsequentes apenas herdam o alinhamento
    for (size_t i = 1; i < linhas.size(); ++i) {
        imprimirDialogoNPC(npcNome, npcCor, linhas[i], false, true);
    }
}
