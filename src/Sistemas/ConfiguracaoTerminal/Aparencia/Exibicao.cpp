#include "Aparencia.h"
#include "../../../Perspectiva/GerenciadorPerspectiva.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>

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

void Aparencia::imprimirBlocoCentralizado(const std::vector<std::string>& linhas, const std::string& corAnsi, int atrasoLinhaMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        size_t end = linha.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? linha.substr(0, end + 1) : "";
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(trimmed));
    }
    imprimirCentralizadoMultilinha(linhas, tamanhoDaLinhaMaisLonga, corAnsi, atrasoLinhaMs);
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

    bool isEngineIDE = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();

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
