#include "TelaMenuRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

int TelaMenuRaycaster::exibirOpcoesMenuPrincipal() {
    std::vector<std::string> opcoes = {"Novo Jogo", "Sair do jogo"};

    std::vector<std::vector<std::string>> artes = {
        ArtesRaycaster::arteNovaAventura,
        ArtesRaycaster::arteSair
    };

    int selecaoAtual = 0;
    bool confirmandoSaida = false;
    int larguraConsole = Aparencia::obterLarguraTerminal();

    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);
        std::string titulo = "MENU PRINCIPAL";
        std::string corTitulo = "\033[38;2;255;215;0m";
        int espTitulo = TelaBaseMenu::calcularOffsetCentral(titulo, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, corTitulo + titulo + "\033[0m", 3, espTitulo, larguraConsole);
        int espNav = TelaBaseMenu::calcularOffsetCentral(10, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;100;100;100m<  \u2191  |  \u2193  >\033[0m", 4, espNav, larguraConsole);

        int linhaInicio = 6;
        int espOpcoes = std::max(0, (larguraConsole - 30) / 2 - 10);

        int maxArtH = 0;
        int maxArtW = 0;
        if (selecaoAtual >= 0 && selecaoAtual < (int)artes.size()) {
            maxArtH = (int)artes[selecaoAtual].size();
            for (const auto& l : artes[selecaoAtual]) {
                maxArtW = std::max(maxArtW, Aparencia::obterComprimentoVisual(l));
            }
        }
        int numOpcoes = (int)opcoes.size();
        int boxH = std::max(maxArtH, numOpcoes) + 2;
        if (confirmandoSaida) {
            boxH += 3; 
        }
        int boxW = 32 + maxArtW + 2; 
        if (confirmandoSaida && boxW < 40) boxW = 40; 
        TelaBaseMenu::desenharCaixaPreta(buffer, linhaInicio - 1, espOpcoes - 2, boxW, boxH);

        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string cursor = (i == selecaoAtual && !confirmandoSaida) ? "> " : "  ";
            std::string corOpcao = (i == selecaoAtual && !confirmandoSaida)
                ? "\033[38;2;255;215;0m"
                : "\033[38;2;180;180;180m";
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corOpcao + cursor + opcoes[i] + "\033[0m", linhaInicio + i, espOpcoes);
        }

        if (confirmandoSaida) {
            std::string msgConf = "Confirmar saida? [S]im / [N]ao";
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;255;100;100m" + msgConf + "\033[0m", linhaInicio + numOpcoes + 1, espOpcoes);
        }

        if (selecaoAtual >= 0 && selecaoAtual < (int)artes.size()) {
            const auto& arte = artes[selecaoAtual];
            int arteX = espOpcoes + 32;
            int arteY = linhaInicio;
            
            if (selecaoAtual == 0) {
                long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                double pulsacao = (std::sin(ms * 0.005) + 1.0) / 2.0; 
                int brilhoChama = 150 + (int)(pulsacao * 105);

                std::vector<MenuRaycasterUtils::GrupoCor> grupos = {
                    {"@", 255, brilhoChama, 0}, 
                    {"*", 255, brilhoChama / 2, 0}, 
                    {"#", 139, 69, 19}
                };
                MenuRaycasterUtils::imprimirArtePixelada(buffer, arte, grupos, arteX, arteY);
            } else if (selecaoAtual == 1) {
                long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                int frameIdx = (ms / 150) % 6; 
                std::vector<std::string> frameArte;
                if (frameIdx == 0 || frameIdx == 5) frameArte = ArtesRaycaster::tvFrame1;
                else if (frameIdx == 1) frameArte = ArtesRaycaster::tvFrame2;
                else if (frameIdx == 2) frameArte = ArtesRaycaster::tvFrame3;
                else if (frameIdx == 3) frameArte = ArtesRaycaster::tvFrame4;
                else frameArte = ArtesRaycaster::tvFrame5; 

                std::vector<MenuRaycasterUtils::GrupoCor> gruposTV = {
                    {"#", 80, 80, 80},     
                    {"%", 200, 255, 255}   
                };
                MenuRaycasterUtils::imprimirArtePixelada(buffer, frameArte, gruposTV, arteX, arteY);
            } else {
                MenuRaycasterUtils::imprimirArtePixeladaSimples(buffer, arte, 200, 180, 255, arteX, arteY);
            }
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            else if (proxTecla == 27) return -1;
        }

        if (confirmandoSaida) {
            if (tecla == 's' || tecla == 'S') {
                return 1;
            } else if (tecla == 'n' || tecla == 'N' || tecla == 27) {
                confirmandoSaida = false;
            }
            continue;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 1) {
                confirmandoSaida = true;
            } else {
                return selecaoAtual;
            }
        }
    }
}



void TelaMenuRaycaster::exibirPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    if (animarFadeIn) {
        for (int f = 0; f <= 10; ++f) {
            MenuRaycasterUtils::incrementarCicloDia();
            std::ostringstream buffer;
            MenuRaycasterUtils::exibirFundo3D(buffer);

            int brilho = f * 255 / 10;
            if (brilho > 255) brilho = 255;
            std::string cor = "\033[38;2;" + std::to_string(brilho) + ";" +
                              std::to_string(brilho) + ";" + std::to_string(brilho) + "m";
            int esp = TelaBaseMenu::calcularOffsetCentral(tituloDaTela, larguraConsole);
            MenuRaycasterUtils::sobreporTexto3D(buffer, cor + tituloDaTela + "\033[0m", 3, esp, larguraConsole);
            MenuRaycasterUtils::flushFrameParaConsole(buffer.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    } else {
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);
        int esp = TelaBaseMenu::calcularOffsetCentral(tituloDaTela, larguraConsole);
        MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;215;0m" + tituloDaTela + "\033[0m", 3, esp, larguraConsole);
        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());
    }
}

bool TelaMenuRaycaster::exibirConfirmacaoDeEscolhaComArteLadoALado(
    const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir,
    const std::vector<std::string>& arteAsciiParaExibir)
{
    int larguraConsole = Aparencia::obterLarguraTerminal();
    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();

    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);
    MenuRaycasterUtils::incrementarCicloDia();

    std::ostringstream buffer;
    MenuRaycasterUtils::exibirFundo3D(buffer);

    std::string titulo = "PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha;
    int esp = TelaBaseMenu::calcularOffsetCentral(titulo, larguraConsole);
    MenuRaycasterUtils::sobreporTexto3D(buffer, "\033[38;2;255;215;0m" + titulo + "\033[0m", 3, esp, larguraConsole);

    int infoY = 5;
    int arteY = 5;
    int infoX = 5;
    int arteX = larguraConsole / 2 + 2;

    for (size_t i = 0; i < informacoesParaExibir.size(); ++i) {
        MenuRaycasterUtils::sobreporTexto3D(buffer, informacoesParaExibir[i], infoY + (int)i, infoX, larguraConsole);
    }
    MenuRaycasterUtils::imprimirArtePixeladaSimples(buffer, arteAsciiParaExibir, 200, 180, 255, arteX, arteY);

    MenuRaycasterUtils::flushFrameParaConsole(buffer.str());
    std::vector<std::string> opcoes = {"VOLTAR", "CONFIRMAR"};
    std::string margem = std::to_string(TelaBaseMenu::calcularOffsetCentral(20, larguraConsole));
    int opcaoDeConfirmacao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, margem);
    return opcaoDeConfirmacao == 1;
}

std::vector<std::string> TelaMenuRaycaster::comporQuadroDeAtributos(
    const Atributos& stats, const std::string& tituloSecao,
    const std::string& tituloHabilidade, const std::string& nomeHab,
    const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2,
    const std::string& descHab2)
{
    auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) {
        std::string corVal;
        if (nomeAtr == "Resistencia") corVal = "\033[38;2;100;100;255m";
        else if (nomeAtr == "Constituicao") corVal = "\033[38;2;0;255;255m";
        else if (nomeAtr == "Vida") {
            if (valorAtr > 100) corVal = "\033[38;2;100;255;100m";
            else if (valorAtr < 100) corVal = "\033[38;2;255;80;80m";
            else corVal = "\033[38;2;255;255;255m";
        } else {
            if (valorAtr > 10) corVal = "\033[38;2;100;255;100m";
            else if (valorAtr < 10) corVal = "\033[38;2;255;80;80m";
            else corVal = "\033[38;2;255;255;255m";
        }
        std::string sinal = (valorAtr >= 0 ? "+" : "");
        return " - " + nomeAtr + ": " + corVal + sinal + std::to_string(valorAtr) + "\033[0m";
    };

    std::vector<std::string> resultado;
    resultado.push_back("\033[38;2;255;255;255m" + tituloSecao + "\033[0m");
    resultado.push_back(formatarAtributo("Vida", stats.vida));
    resultado.push_back(formatarAtributo("Forca", stats.forca));
    resultado.push_back(formatarAtributo("Destreza", stats.destreza));
    resultado.push_back(formatarAtributo("Resistencia", stats.resistencia));
    resultado.push_back(formatarAtributo("Constituicao", stats.constituicao));
    resultado.push_back(formatarAtributo("Inteligencia", stats.inteligencia));
    resultado.push_back(formatarAtributo("Sabedoria", stats.sabedoria));
    resultado.push_back("");
    resultado.push_back("\033[38;2;255;255;255m" + tituloHabilidade + "\033[0m");
    resultado.push_back(" \033[38;2;0;200;255m" + nomeHab + "\033[0m");

    std::istringstream stream(descHab);
    std::string linha;
    while (std::getline(stream, linha)) {
        if (!linha.empty())
            resultado.push_back(" - \033[38;2;180;180;180m" + linha + "\033[0m");
    }

    if (!tituloHabilidade2.empty()) {
        resultado.push_back("");
        resultado.push_back("\033[38;2;255;255;255m" + tituloHabilidade2 + "\033[0m");
        resultado.push_back(" \033[38;2;0;200;255m" + nomeHab2 + "\033[0m");
        std::istringstream stream2(descHab2);
        while (std::getline(stream2, linha)) {
            if (!linha.empty())
                resultado.push_back(" - \033[38;2;180;180;180m" + linha + "\033[0m");
        }
    }

    return resultado;
}

bool TelaMenuRaycaster::exibirConfirmacaoSaida() {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);
    int selecaoAtual = 1;
    
    std::string pergunta = "Deseja realmente sair do jogo?";
    std::vector<std::string> opcoes = {"NAO", "SIM"};
    
    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);
        
        int boxW = Aparencia::obterComprimentoVisual(pergunta) + 10;
        int boxH = 7;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        int boxY = 14;
        
        TelaBaseMenu::desenharCaixaPreta(buffer, boxY, boxX, boxW, boxH);
        
        int espPergunta = TelaBaseMenu::calcularOffsetCentral(pergunta, larguraConsole);
        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;255;255;255m" + pergunta + "\033[0m", boxY + 2, espPergunta);

        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string cursor = (i == selecaoAtual) ? "> " : "  ";
            std::string corOpcao = (i == selecaoAtual) ? "\033[38;2;255;215;0m" : "\033[38;2;180;180;180m";
            int espOpcoes = TelaBaseMenu::calcularOffsetCentral(opcoes[i] + cursor, larguraConsole);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corOpcao + cursor + opcoes[i] + "\033[0m", boxY + 4 + i, espOpcoes);
        }
        
        int arteY = std::max(2, boxY - 11);

        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        int frameIdx = (ms / 150) % 6; 
        std::vector<std::string> frameArte;
        if (frameIdx == 0 || frameIdx == 5) frameArte = ArtesRaycaster::tvFrame1;
        else if (frameIdx == 1) frameArte = ArtesRaycaster::tvFrame2;
        else if (frameIdx == 2) frameArte = ArtesRaycaster::tvFrame3;
        else if (frameIdx == 3) frameArte = ArtesRaycaster::tvFrame4;
        else frameArte = ArtesRaycaster::tvFrame5; 

        int arteX = TelaBaseMenu::calcularOffsetCentral((int)frameArte[0].length(), larguraConsole);

        std::vector<MenuRaycasterUtils::GrupoCor> gruposTV = {
            {"#", 80, 80, 80},     
            {"%", 200, 255, 255}   
        };
        MenuRaycasterUtils::imprimirArtePixelada(buffer, frameArte, gruposTV, arteX, arteY);
        
        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());
        
        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        
        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            else if (proxTecla == 27) return false;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual == 1;
        }
    }
}
