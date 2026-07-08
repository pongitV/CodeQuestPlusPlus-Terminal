#include "TelaBase.h"
#include "../GerenciadorPerspectiva.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Entidades/Personagem.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Sistemas/Inventario/Inventario.h"
#include <iostream>
#include <algorithm>

std::string TelaBase::gerarBarraGradiente(double pct, int tamanho, Cor corFinal) {
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

void TelaBase::imprimirLinhaDivisoria(char caractere) {
    std::string linha = "";
    int largura = Aparencia::obterLarguraTerminal();
    
    bool isEngineIDE = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
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

void TelaBase::executarLoop(
    const std::function<void(bool)>& renderCabecalho,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha,
    bool centralizarMenu,
    const std::string& margemMenu)
{
    bool primeiraVez = true;
    while (true) {
        Aparencia::limparTela();
        
        if (renderCabecalho) {
            renderCabecalho(primeiraVez);
            primeiraVez = false;
        }

        if (renderConteudo) {
            renderConteudo();
        }

        std::vector<std::string> opcoes = construtorOpcoesMenu();
        std::cout << "\n";
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, centralizarMenu, margemMenu);
        
        if (!processarEscolha(escolha)) {
            break;
        }
    }
}

void TelaBase::executarLoopPadrao(
    const std::string& titulo,
    Cor corTema,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha)
{
    executarLoop(
        [titulo, corTema](bool animar) { Aparencia::exibirPainelTexto(titulo, corTema, animar); },
        renderConteudo,
        construtorOpcoesMenu,
        processarEscolha
    );
}

std::vector<std::string> TelaBase::criarCaixa(const std::vector<std::string>& linhas, const std::string& titulo, int larguraMinima, Cor corCaixa, const std::string& bgAnsi) {
    int maxLargura = larguraMinima;
    for (const auto& linha : linhas) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > maxLargura) maxLargura = comp;
    }
    
    std::vector<std::string> caixa;
    std::string corStr = Aparencia::cor(corCaixa);
    std::string resetStr = Aparencia::cor(Cor::RESET);

    bool isEngineIDE = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isEngineIDE) {
        std::string tituloIDE = titulo.empty() ? "Info" : titulo;
        std::replace(tituloIDE.begin(), tituloIDE.end(), ' ', '_');
        
        caixa.push_back("\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176m" + tituloIDE + "\033[0m {");
        for (const auto& linha : linhas) {
            std::string linhaLimpa = linha;
            if (linhaLimpa.find(":") != std::string::npos) {
                size_t pos = linhaLimpa.find(":");
                std::string chave = linhaLimpa.substr(0, pos);
                std::string valor = linhaLimpa.substr(pos + 1);
                
                std::string chaveVar = Aparencia::removerCoresANSI(chave);
                chaveVar.erase(std::remove(chaveVar.begin(), chaveVar.end(), ' '), chaveVar.end());
                
                linhaLimpa = "    \033[38;2;86;156;214mauto\033[0m " + chaveVar + " = " + valor + ";";
            } else {
                linhaLimpa = "    " + linha + ";";
            }
            
            int comp = Aparencia::obterComprimentoVisual(linhaLimpa);
            int padding = maxLargura - comp;
            if (padding > 0) linhaLimpa += std::string(padding, ' ');
            
            caixa.push_back(linhaLimpa);
        }
        caixa.push_back("};");
        return caixa;
    }

    std::string padBg = "";
    if (!bgAnsi.empty()) {
        padBg = bgAnsi;
    } else if (!isEngineIDE) {
        padBg = "\033[48;2;0;0;0m";
    }

    std::string top = "╔";
    int tituloLen = Aparencia::obterComprimentoVisual(titulo);
    
    // Em modo 3D (raycaster), não colocamos o texto na borda, pois um título em ASCII flutua acima!
    if (!isEngineIDE) {
        tituloLen = 0;
    }
    
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
        int comp = Aparencia::obterComprimentoVisual(linha);
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
            c += "\033[0m";
        }
    }

    return caixa;
}

std::vector<std::string> TelaBase::criarCaixaComArte(const std::vector<std::string>& arte, const std::vector<std::string>& linhasTexto, const std::string& titulo, int larguraMinima, Cor corCaixa, const std::string& bgAnsi) {
    int larguraArte = 0;
    for (const auto& l : arte) {
        int len = Aparencia::obterComprimentoVisual(l);
        if (len > larguraArte) larguraArte = len;
    }

    int larguraTexto = larguraMinima;
    for (const auto& l : linhasTexto) {
        int len = Aparencia::obterComprimentoVisual(l);
        if (len > larguraTexto) larguraTexto = len;
    }

    bool temArte = larguraArte > 0;
    int totalWidth = larguraTexto;
    if (temArte) totalWidth += larguraArte + 3;

    if (totalWidth < larguraMinima) totalWidth = larguraMinima;

    int boxHeight = std::max(static_cast<int>(arte.size()), static_cast<int>(linhasTexto.size()));

    std::vector<std::string> caixa;
    std::string corStr = Aparencia::cor(corCaixa);
    std::string resetStr = Aparencia::cor(Cor::RESET);
    std::string padBg = bgAnsi.empty() ? "" : bgAnsi;

    std::string top = padBg + corStr + "╔";
    int tituloLen = Aparencia::obterComprimentoVisual(titulo);
    if (tituloLen > 0) {
        top += "══ " + titulo + " ";
        int restantes = totalWidth + 2 - (tituloLen + 4);
        if (restantes < 0) restantes = 0;
        for (int i = 0; i < restantes; ++i) top += "═";
    } else {
        for (int i = 0; i < totalWidth + 2; ++i) top += "═";
    }
    top += "╗" + resetStr;
    caixa.push_back(top);

    for (int i = 0; i < boxHeight; ++i) {
        std::string linhaArte = (i < static_cast<int>(arte.size())) ? arte[i] : "";
        int compArte = Aparencia::obterComprimentoVisual(linhaArte);
        int padArte = larguraArte - compArte;

        std::string linhaTexto = (i < static_cast<int>(linhasTexto.size())) ? linhasTexto[i] : "";
        int compTexto = Aparencia::obterComprimentoVisual(linhaTexto);
        int padTexto = larguraTexto - compTexto;

        std::string row;
        if (temArte) {
            row = padBg + corStr + "║ " + resetStr + padBg + linhaArte + std::string(padArte > 0 ? padArte : 0, ' ') + corStr + padBg + " ║ " + resetStr + padBg + linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ') + corStr + padBg + " ║" + resetStr;
        } else {
            row = padBg + corStr + "║ " + resetStr + padBg + linhaTexto + std::string(padTexto > 0 ? padTexto : 0, ' ') + corStr + padBg + " ║" + resetStr;
        }
        caixa.push_back(row);
    }

    std::string bottom = padBg + corStr + "╚";
    for (int i = 0; i < totalWidth + 2; ++i) bottom += "═";
    bottom += "╝" + resetStr;
    caixa.push_back(bottom);

    if (!bgAnsi.empty()) {
        for (auto& c : caixa) {
            std::string toReplace = "\033[0m";
            std::string replaceWith = "\033[0m" + bgAnsi;
            size_t pos = c.find(toReplace);
            while (pos != std::string::npos) {
                c.replace(pos, toReplace.length(), replaceWith);
                pos = c.find(toReplace, pos + replaceWith.length());
            }
        }
    }

    return caixa;
}

bool TelaBase::deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos) {
    auto agora = std::chrono::steady_clock::now();
    bool animar = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoAcesso).count() > delayMilissegundos;
    ultimoAcesso = agora;
    return animar;
}