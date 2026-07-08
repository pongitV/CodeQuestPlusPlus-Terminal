#include "TelaMenuIDE.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include <iostream>
#include <thread>
#include <chrono>

void TelaMenuIDE::exibirPainelLogoJogo(const std::string& tituloDaTela, bool /*animarFadeIn*/) {
    // Na IDE, nao usamos animacao de fadeIn, printamos como codigo estatico
    Aparencia::limparTela();
    
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorPunct = "\033[38;2;212;212;212m"; // White/Gray
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string reset = "\033[0m";

    std::cout << "\n\n  " << colorType << "GameEngine" << colorPunct << "::" 
              << colorType << "Display" << colorPunct << "::" 
              << colorKeyword << "renderTitle" << colorPunct << "("
              << colorString << "\"" << tituloDaTela << "\"" << colorPunct << ");\n\n" << reset;
}

int TelaMenuIDE::exibirOpcoesMenuPrincipal() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    std::vector<std::string> opcoes = {"Novo Jogo", "Sair do Jogo"};
    
    std::string colorEnum = "\033[38;2;78;201;176m"; // Cyan

    int selecaoAtual = 0;
    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();

    while (true) {
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back("  " + colorType + "GameEngine" + colorPunct + "::" 
              + colorType + "Display" + colorPunct + "::" 
              + colorKeyword + "renderTitle" + colorPunct + "("
              + colorString + "\"MENU PRINCIPAL\"" + colorPunct + ");");
        blocoCentral.push_back("");
        blocoCentral.push_back(colorComment + "// Selecione uma opcao para continuar" + reset);
        blocoCentral.push_back(colorKeyword + "enum class " + colorEnum + "MenuOption " + colorPunct + "{");
        
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string linha = "    ";
            std::string nomeOpcao = opcoes[i] == "Novo Jogo" ? "NEW_GAME" : "EXIT_GAME";
            
            if (i == selecaoAtual) {
                linha += colorHighlight + nomeOpcao + reset;
            } else {
                linha += colorPunct + nomeOpcao + reset;
            }
            
            if (i < (int)opcoes.size() - 1) {
                linha += colorPunct + ",";
            }
            linha += " " + colorComment + "// " + opcoes[i] + reset;
            
            blocoCentral.push_back(linha);
        }
        
        blocoCentral.push_back(colorPunct + "};");
        blocoCentral.push_back("");
        blocoCentral.push_back(colorKeyword + "return " + colorPunct + "await_selection();");

        int espacosY = calcularEspacoY(blocoCentral.size());
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";

        imprimirBlocoCentralizadoIDE(blocoCentral);

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 1) {
                if (exibirConfirmacaoSaida()) {
                    return 1;
                }
            } else {
                return selecaoAtual;
            }
        }
    }
}

bool TelaMenuIDE::exibirConfirmacaoSaida() {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorFunc = "\033[38;2;220;220;170m"; // Yellow
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    int selecaoAtual = 1;
    ControleDeInput::limparBuffer();

    while (true) {
        Aparencia::limparTela();
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorKeyword + "bool " + colorFunc + "confirmExit" + colorPunct + "() {");
        blocoCentral.push_back("    " + colorFunc + "print" + colorPunct + "(" + colorString + "\"Deseja realmente sair do jogo?\"" + colorPunct + ");");
        
        std::string opNao = (selecaoAtual == 0) ? (colorHighlight + "false" + reset) : (colorKeyword + "false" + reset);
        std::string opSim = (selecaoAtual == 1) ? (colorHighlight + "true" + reset) : (colorKeyword + "true" + reset);
        
        blocoCentral.push_back("    " + colorKeyword + "return " + colorPunct + (selecaoAtual == 0 ? opNao : opSim) + colorPunct + "; // [0] false (NAO), [1] true (SIM)");
        blocoCentral.push_back(colorPunct + "}");

        int espacosY = calcularEspacoY(blocoCentral.size());
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";

        imprimirBlocoCentralizadoIDE(blocoCentral);

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W' || tecla == 'a' || tecla == 'A') {
            selecaoAtual = 0;
        } else if (tecla == 's' || tecla == 'S' || tecla == 'd' || tecla == 'D') {
            selecaoAtual = 1;
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual == 1;
        }
    }
}

bool TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorFunc = "\033[38;2;220;220;170m"; // Yellow
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string colorHighlight = "\033[48;2;38;79;120m\033[38;2;255;255;255m"; // Azul escuro bg
    std::string reset = "\033[0m";

    int selecaoAtual = 1;
    ControleDeInput::limparBuffer();

    while (true) {
        Aparencia::limparTela();
        
        std::vector<std::string> blocoCentral;
        blocoCentral.push_back(colorComment + "// Previa da " + tipoDeEscolha + ": " + nomeDaEscolha + reset);
        blocoCentral.push_back(colorType + "PreviewData " + colorPunct + "data = {");
        
        for (const auto& info : informacoesParaExibir) {
            blocoCentral.push_back("    " + colorString + "\"" + info + "\"" + colorPunct + ",");
        }
        
        blocoCentral.push_back("    " + colorKeyword + "struct " + colorPunct + "{");
        for (const auto& arte : arteAsciiParaExibir) {
            blocoCentral.push_back("        " + colorComment + "// " + arte + reset);
        }
        blocoCentral.push_back("    " + colorPunct + "} art");
        blocoCentral.push_back(colorPunct + "};");
        blocoCentral.push_back("");
        
        std::string opVoltar = (selecaoAtual == 0) ? (colorHighlight + "false" + reset) : (colorKeyword + "false" + reset);
        std::string opConfirmar = (selecaoAtual == 1) ? (colorHighlight + "true" + reset) : (colorKeyword + "true" + reset);
        
        blocoCentral.push_back(colorKeyword + "return " + colorFunc + "confirm" + colorPunct + "(" + (selecaoAtual == 0 ? opVoltar : opConfirmar) + colorPunct + ");");
        
        int espacosY = calcularEspacoY(blocoCentral.size());
        for (int i = 0; i < espacosY; ++i) std::cout << "\n";
        
        imprimirBlocoCentralizadoIDE(blocoCentral);

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
        }

        if (tecla == 'w' || tecla == 'W' || tecla == 'a' || tecla == 'A') {
            selecaoAtual = 0;
        } else if (tecla == 's' || tecla == 'S' || tecla == 'd' || tecla == 'D') {
            selecaoAtual = 1;
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual == 1;
        }
    }
}

std::vector<std::string> TelaMenuIDE::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    std::string colorKeyword = "\033[38;2;86;156;214m"; // Blue
    std::string colorType = "\033[38;2;78;201;176m"; // Cyan
    std::string colorPunct = "\033[38;2;212;212;212m"; // Gray
    std::string colorString = "\033[38;2;214;157;133m"; // Orange/Greenish
    std::string colorNum = "\033[38;2;181;206;168m"; // Light Green (numbers)
    std::string colorComment = "\033[38;2;87;166;74m"; // Green
    std::string reset = "\033[0m";

    std::vector<std::string> res;
    res.push_back(colorComment + "// " + tituloSecao + reset);
    res.push_back(colorKeyword + "struct " + colorType + "Stats " + colorPunct + "{");
    res.push_back("    " + colorType + "int " + colorPunct + "Vida = " + colorNum + std::to_string(stats.vida) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Forca = " + colorNum + std::to_string(stats.forca) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Destreza = " + colorNum + std::to_string(stats.destreza) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Resistencia = " + colorNum + std::to_string(stats.resistencia) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Constituicao = " + colorNum + std::to_string(stats.constituicao) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Inteligencia = " + colorNum + std::to_string(stats.inteligencia) + colorPunct + ";");
    res.push_back("    " + colorType + "int " + colorPunct + "Sabedoria = " + colorNum + std::to_string(stats.sabedoria) + colorPunct + ";");
    res.push_back(colorPunct + "};" + reset);
    res.push_back("");
    
    res.push_back(colorComment + "// " + tituloHabilidade + reset);
    res.push_back(colorType + "Ability " + colorPunct + "skill1 = " + colorString + "\"" + nomeHab + "\"" + colorPunct + ";");
    res.push_back(colorComment + "/* " + descHab + " */" + reset);
    
    if (!tituloHabilidade2.empty()) {
        res.push_back("");
        res.push_back(colorComment + "// " + tituloHabilidade2 + reset);
        res.push_back(colorType + "Ability " + colorPunct + "skill2 = " + colorString + "\"" + nomeHab2 + "\"" + colorPunct + ";");
        res.push_back(colorComment + "/* " + descHab2 + " */" + reset);
    }
    
    return res;
}

std::vector<std::string> TelaMenuIDE::comprimirArteASCII(const std::vector<std::string>& arteOriginal, int fatorY, int fatorX) {
    std::vector<std::string> comprimida;
    if (arteOriginal.empty() || fatorY <= 0 || fatorX <= 0) return comprimida;

    for (size_t y = 0; y < arteOriginal.size(); y += fatorY) {
        std::string novaLinha = "";
        for (size_t x = 0; x < arteOriginal[y].length(); x += fatorX) {
            char c = ' ';
            // Tenta pegar o caractere mais denso no bloco (fatorY x fatorX)
            for (size_t dy = 0; dy < (size_t)fatorY && y + dy < arteOriginal.size(); dy++) {
                for (size_t dx = 0; dx < (size_t)fatorX && x + dx < arteOriginal[y + dy].length(); dx++) {
                    char cur = arteOriginal[y + dy][x + dx];
                    if (cur != ' ' && cur != '\n' && cur != '\r' && c == ' ') {
                        c = cur;
                    } else if (cur == '#' || cur == '@' || cur == '%' || cur == '*') { // Prioriza caracteres mais "visíveis"
                        c = cur;
                    }
                }
            }
            novaLinha += c;
        }
        // Trim right spaces to keep it clean
        size_t end = novaLinha.find_last_not_of(" ");
        if (end != std::string::npos) {
            novaLinha = novaLinha.substr(0, end + 1);
            if (!novaLinha.empty()) {
                comprimida.push_back(novaLinha);
            }
        }
    }
    return comprimida;
}

int TelaMenuIDE::calcularEspacoY(int linhasDoConteudo) {
    int altura = Aparencia::obterAlturaTerminal();
    return std::max(0, (altura - linhasDoConteudo) / 2);
}

void TelaMenuIDE::imprimirBlocoCentralizadoIDE(const std::vector<std::string>& bloco, int recuoAdicionalX) {
    int largura = Aparencia::obterLarguraTerminal();
    int maxLen = 0;
    for (const auto& linha : bloco) {
        int len = Aparencia::obterComprimentoVisual(linha);
        if (len > maxLen) maxLen = len;
    }
    
    int espacos = std::max(0, (largura - maxLen) / 2) + recuoAdicionalX;
    std::string pad(espacos, ' ');
    
    for (const auto& linha : bloco) {
        std::cout << pad << linha << "\n";
    }
}
