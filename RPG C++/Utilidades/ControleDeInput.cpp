#include "ControleDeInput.h"
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif
#include "Aparencia.h"

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
    int maxLinhas = std::max(totalOpcoes, totalDir);
    std::string margem = margemPersonalizada;
    
    int maxLargura = 0;
    for (const std::string& op : opcoes) {
        int comp = Aparencia::obterComprimentoVisual(op);
        if (comp > maxLargura) maxLargura = comp;
    }
    int larguraMenuEsq = maxLargura + 15; // 3 para " > " + 12 de espacamento

    if (centralizar) {
        margem = Aparencia::espacosParaCentralizar(larguraMenuEsq + (totalDir > 0 ? 40 : 0));
    }

    // Oculta o cursor do console temporariamente para evitar piscadas visuais na atualização
    std::cout << "\033[?25l";

    while (true) {
        for (int i = 0; i < maxLinhas; ++i) {
            std::string linhaEsq = "";
            int lenEsqReal = 0;
            if (i < totalOpcoes) {
                if (i == selecaoAtual) {
                    linhaEsq = Aparencia::cor(Cor::VERDE) + " > " + opcoes[i] + Aparencia::cor(Cor::RESET);
                } else {
                    linhaEsq = "   " + opcoes[i];
                }
                lenEsqReal = Aparencia::obterComprimentoVisual(opcoes[i]) + 3;
            }
            
            std::string linhaDir = (i < totalDir) ? painelDireito[i] : "";
            
            int padding = larguraMenuEsq - lenEsqReal;
            if (padding < 0) padding = 0;
            
            std::cout << margem << linhaEsq << std::string(padding, ' ') << linhaDir << "\033[K\n";
        }

        std::cout << std::flush; // Garante que a tela sempre atualize antes de esperar a tecla

        unsigned char tecla = static_cast<unsigned char>(lerTecla());
        
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(lerTecla()); // Para lidar com sequências de escape POSIX (\033[A)
            
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') { selecaoAtual--; if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1; }
        else if (tecla == 's' || tecla == 'S') { selecaoAtual++; if (selecaoAtual >= totalOpcoes) selecaoAtual = 0; }
        else if (tecla == '\r' || tecla == '\n') { std::cout << "\033[?25h"; return selecaoAtual; } // Restaura o cursor

        std::cout << "\r\033[" << maxLinhas << "A"; // Retorna o cursor para cima a fim de reescrever o texto
    }
}

void ControleDeInput::aguardarEnter(const std::string& mensagem) {
    std::cout << "\n" << Aparencia::espacosParaCentralizar(Aparencia::obterComprimentoVisual(mensagem)) << mensagem << "\n";
    ControleDeInput::limparBuffer();
    while (true) {
        char c = ControleDeInput::lerTecla();
        if (c == '\r' || c == '\n') break;
    }
}
