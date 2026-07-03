#include "ControleDeInput.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif
#include "../Aparencia/Aparencia.h"

std::function<void()> ControleDeInput::onAguardarEnterUpdate = nullptr;
std::string ControleDeInput::enterPromptText = "";

bool ControleDeInput::teclaPressionada() 
{
#ifdef _WIN32
    return _kbhit() != 0;
#else
    return false; // Ambiente POSIX requer implementacao detalhada non-blocking
#endif
}

char ControleDeInput::lerTecla() 
{
#ifdef _WIN32
    return static_cast<char>(_getch());
#else
    return static_cast<char>(std::cin.get());
#endif
}

void ControleDeInput::limparBuffer() 
{
#ifdef _WIN32
    while (_kbhit()) _getch();
#endif
}

ComandoMapa ControleDeInput::traduzirTeclaParaComando(char tecla)
{
    if (tecla == 'w' || tecla == 'W' || tecla == 72) return ComandoMapa::Cima;
    if (tecla == 's' || tecla == 'S' || tecla == 80) return ComandoMapa::Baixo;
    if (tecla == 'a' || tecla == 'A' || tecla == 75) return ComandoMapa::Esquerda;
    if (tecla == 'd' || tecla == 'D' || tecla == 77) return ComandoMapa::Direita;
    if (tecla == 'i' || tecla == 'I') return ComandoMapa::Inventario;
    if (tecla == 'c' || tecla == 'C') return ComandoMapa::Ficha;
    if (tecla == 'b' || tecla == 'B') return ComandoMapa::Bestiario;
    return ComandoMapa::Nenhum;
}

std::string ControleDeInput::lerEntradaProtegida(const std::string& promptMensagem) {
    if (!promptMensagem.empty()) {
        Aparencia::exibirPrompt(promptMensagem);
    }
    std::cout << "\033[s";
    std::string entrada;
    while (true) {
        if (!std::getline(std::cin, entrada)) std::cin.clear();
        
        if (!entrada.empty()) {
            entrada.erase(0, entrada.find_first_not_of(" \n\r\t"));
            entrada.erase(entrada.find_last_not_of(" \n\r\t") + 1);
        }
        
        if (entrada.empty()) {
            std::cout << "\033[u\033[J";
            continue;
        }
        return entrada;
    }
}

int ControleDeInput::lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt, const std::string& margemPersonalizada) {
    int valor;
    if (centralizarPrompt) Aparencia::exibirPrompt(promptMensagem);
    else std::cout << margemPersonalizada << promptMensagem;

    while (true) {
        std::string entrada = lerEntradaProtegida();
        try {
            valor = std::stoi(entrada);
            if (valor >= minimo && valor <= maximo) break;
        } catch (...) {}
        std::cout << "\033[u\033[J"; // Apenas limpa a entrada invalida
    }
    return valor;
}

int ControleDeInput::lerSelecaoMenuComSetas(const std::vector<std::string>& opcoes, bool centralizar, const std::string& margemPersonalizada, const std::vector<std::string>& painelDireito) {
    if (opcoes.empty()) return -1;
    
    int selecaoAtual = 0;
    int totalOpcoes = static_cast<int>(opcoes.size());
    int totalDir = static_cast<int>(painelDireito.size());

    // Pula para a primeira opcao que nao seja HEADER (evita que comece focado nas bordas da caixa)
    while (selecaoAtual < totalOpcoes && opcoes[selecaoAtual].find("#HEADER#") == 0) {
        selecaoAtual++;
    }
    if (selecaoAtual >= totalOpcoes) selecaoAtual = 0;

    int maxLinhas = std::max(totalOpcoes, totalDir);
    std::string margem = margemPersonalizada;
    
    int maxLargura = 0;
    for (const std::string& op : opcoes) {
        std::string texto = op;
        if (texto.find("#HEADER#") == 0) texto = texto.substr(8);
        int comp = Aparencia::obterComprimentoVisual(texto);
        if (comp > maxLargura) maxLargura = comp;
    }
    int larguraMenuEsq = maxLargura + 15; // 3 para " > " + 12 de espacamento

    if (centralizar) {
        margem = Aparencia::espacosParaCentralizar(larguraMenuEsq + (totalDir > 0 ? 40 : 0));
    }

    // Oculta o cursor do console temporariamente para evitar piscadas visuais na atualização
    Aparencia::ocultarCursor();

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::string cursorIcon = ((tempoMs / 400) % 2 == 0) ? ">  " : " > ";

        for (int i = 0; i < maxLinhas; ++i) {
            std::string linhaEsq = "";
            int lenEsqReal = 0;
            if (i < totalOpcoes) {
                std::string texto = opcoes[i];
                bool isHeader = false;
                if (texto.find("#HEADER#") == 0) {
                    isHeader = true;
                    texto = texto.substr(8);
                }

                if (isHeader) {
                    linhaEsq = "   " + texto;
                } else if (i == selecaoAtual) {
                    linhaEsq = Aparencia::cor(Cor::VERDE) + cursorIcon + texto + Aparencia::cor(Cor::RESET);
                } else {
                    linhaEsq = "   " + texto;
                }
                lenEsqReal = Aparencia::obterComprimentoVisual(texto) + 3;
            }
            
            std::string linhaDir = (i < totalDir) ? painelDireito[i] : "";
            
            int padding = larguraMenuEsq - lenEsqReal;
            if (padding < 0) padding = 0;
            
            std::cout << margem << linhaEsq << std::string(padding, ' ') << linhaDir << "\033[K\n";
        }

        std::cout << std::flush; // Garante que a tela sempre atualize antes de esperar a tecla

        if (teclaPressionada()) {
            unsigned char tecla = static_cast<unsigned char>(lerTecla());
            
            if (tecla == 224 || tecla == 0 || tecla == '\033') {
                unsigned char proxTecla = static_cast<unsigned char>(lerTecla());
                if (proxTecla == '[') proxTecla = static_cast<unsigned char>(lerTecla()); // Para lidar com sequências de escape POSIX (\033[A)
                
                if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
                else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            }

            if (tecla == 'w' || tecla == 'W') { 
                int inicio = selecaoAtual;
                do {
                    selecaoAtual--; 
                    if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1; 
                } while (opcoes[selecaoAtual].find("#HEADER#") == 0 && selecaoAtual != inicio);
            }
            else if (tecla == 's' || tecla == 'S') { 
                int inicio = selecaoAtual;
                do {
                    selecaoAtual++; 
                    if (selecaoAtual >= totalOpcoes) selecaoAtual = 0; 
                } while (opcoes[selecaoAtual].find("#HEADER#") == 0 && selecaoAtual != inicio);
            }
            else if (tecla == '\r' || tecla == '\n') { Aparencia::mostrarCursor(); return selecaoAtual; } // Restaura o cursor

            std::cout << "\r\033[" << maxLinhas << "A"; // Retorna o cursor para cima a fim de reescrever o texto
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "\r\033[" << maxLinhas << "A"; // Retorna o cursor para cima a fim de reescrever o texto
        }
    }
}

int ControleDeInput::lerSelecaoMenuEmPopup(const std::string& titulo, const std::vector<std::string>& texto, const std::vector<std::string>& opcoes, Cor corTema, const std::vector<std::string>& arteOriginal, bool animarEntrada) {
    if (opcoes.empty()) return -1;
        
    std::vector<std::string> arte = arteOriginal;
    if (static_cast<int>(arte.size()) > 10) {
        arte = Aparencia::reduzirEscalaAscii(arteOriginal, Aparencia::FATOR_COMPRESSAO_GLOBAL, Aparencia::FATOR_COMPRESSAO_GLOBAL);
    }

    int larguraArte = 0;
    for (const auto& l : arte) {
        int len = Aparencia::obterComprimentoVisual(l);
        if (len > larguraArte) larguraArte = len;
    }

    int selecaoAtual = 0;
    int totalOpcoes = static_cast<int>(opcoes.size());
    std::string bgPopup = "\033[48;2;15;15;15m"; 
    std::cout << "\033[?25l"; 
    
    ControleDeInput::limparBuffer();
    bool primeiroRender = true;
    while (true) {
        std::vector<std::string> linhasTexto = texto;
        linhasTexto.push_back("");
        for (int i = 0; i < totalOpcoes; ++i) {
            if (i == selecaoAtual) linhasTexto.push_back(Aparencia::cor(Cor::VERDE) + " > " + opcoes[i] + Aparencia::cor(Cor::RESET));
            else linhasTexto.push_back("   " + opcoes[i]);
        }

        int minBoxHeight = Aparencia::obterMinAlturaPopup() - 2;
        int minTotalWidth = Aparencia::obterMinLarguraPopup() - 4;
        if (minBoxHeight < 0) minBoxHeight = 0;
        if (minTotalWidth < 0) minTotalWidth = 0;

        // Preenche com espacos vazios na vertical para garantir o tamanho minimo e sobrepor popups velhos
        while (std::max(arte.size(), linhasTexto.size()) < static_cast<size_t>(minBoxHeight)) {
            linhasTexto.push_back("");
        }

        int larguraTexto = 0;
        for (const auto& l : linhasTexto) {
            int len = Aparencia::obterComprimentoVisual(l);
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
        std::string corStr = Aparencia::cor(corTema);
        std::string resetStr = Aparencia::cor(Cor::RESET);

        std::string top = "╔";
        int tituloLen = Aparencia::obterComprimentoVisual(titulo);
        
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
            int padArte = larguraArte - Aparencia::obterComprimentoVisual(linhaArte);
            std::string artePart = linhaArte + std::string(padArte > 0 ? padArte : 0, ' ');

            std::string linhaTexto = (i < static_cast<int>(linhasTexto.size())) ? linhasTexto[i] : "";
            int padTexto = larguraTexto - Aparencia::obterComprimentoVisual(linhaTexto);
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

        int finalBoxWidth = Aparencia::obterComprimentoVisual(caixa[0]);
        int finalBoxHeight = caixa.size();
        Aparencia::atualizarMinTamanhoPopup(finalBoxWidth, finalBoxHeight);
        
        int larguraTerm = Aparencia::obterLarguraTerminal();
        int alturaTerm = Aparencia::obterAlturaTerminal();
        int startX = (larguraTerm - finalBoxWidth) / 2;
        int startY = (alturaTerm - finalBoxHeight) / 2;
        if (startX < 0) startX = 0;
        if (startY < 0) startY = 0;
        Aparencia::renderizarCaixaPopupAnimada(caixa, startX, startY, primeiroRender && animarEntrada);
        primeiroRender = false;
        
        char tecla = lerTecla();
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            char proxTecla = lerTecla();
            if (proxTecla == '[') proxTecla = lerTecla();
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') { selecaoAtual--; if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1; }
        else if (tecla == 's' || tecla == 'S') { selecaoAtual++; if (selecaoAtual >= totalOpcoes) selecaoAtual = 0; }
        else if (tecla == '\r' || tecla == '\n') { return selecaoAtual; }
    }
}

void ControleDeInput::executarLoopMenuPopup(
    const std::function<void()>& exibirDialogo,
    const std::function<std::vector<std::string>()>& obterOpcoes,
    const std::function<bool(const std::string&)>& processarOpcao,
    const std::string& titulo,
    Cor corTema,
    const std::vector<std::string>& arteAscii
) {
    Aparencia::iniciarInteracaoPopup();
    if (exibirDialogo) exibirDialogo();

    while (true) {
        auto opcoes = obterOpcoes();
        if (opcoes.empty()) break;
        
        int escolha = lerSelecaoMenuEmPopup(titulo, {"O que deseja fazer?"}, opcoes, corTema, arteAscii);
        if (escolha == -1 || escolha == static_cast<int>(opcoes.size()) - 1) break;
        if (!processarOpcao(opcoes[escolha])) break;
    }
}

void ControleDeInput::aguardarEnter(const std::string& mensagem) {
    Aparencia::ocultarCursor();
    if (ControleDeInput::onAguardarEnterUpdate) {
        ControleDeInput::enterPromptText = mensagem;
        ControleDeInput::limparBuffer();
        while (true) {
            ControleDeInput::onAguardarEnterUpdate();
            if (ControleDeInput::teclaPressionada()) {
                char c = ControleDeInput::lerTecla();
                if (c == '\r' || c == '\n') break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        ControleDeInput::enterPromptText = "";
        ControleDeInput::onAguardarEnterUpdate();
    } else {
        if (!mensagem.empty()) {
            std::cout << "\n\033[0m" << Aparencia::espacosParaCentralizar(Aparencia::obterComprimentoVisual(mensagem)) << "\033[5m" << mensagem << "\033[0m\n";
        }
        ControleDeInput::limparBuffer();
        while (true) {
            char c = ControleDeInput::lerTecla();
            if (c == '\r' || c == '\n') break;
        }
    }
}
