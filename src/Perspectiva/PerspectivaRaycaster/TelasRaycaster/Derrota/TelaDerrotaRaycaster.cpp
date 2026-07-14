#include "TelaDerrotaRaycaster.h"
#include "../Utils/MenuRaycasterUtils.h"
#include "../../EngineRaycaster/Raycaster.h"
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../TelasBase/Derrota/TelaDerrotaLayout.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../EngineRaycaster/RaycasterRenderizadorCombate.h"
#include <sstream>
#include <thread>
#include <chrono>

void TelaDerrotaRaycaster::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
{
    int larguraConsole = Aparencia::obterLarguraTerminal();
    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();

    // Usa o mesmo fundo gerado e cacheado durante a batalha
    std::vector<std::string> flatTela = RaycasterRenderizadorCombate::obterUltimoFundoRenderizado();
    int altura3D = std::max(10, Aparencia::obterAlturaTerminal());
    
    // Fallback
    if (flatTela.empty() || static_cast<int>(flatTela.size()) < altura3D * larguraConsole) {
        std::string tituloMapa = "Cemiterio";
        std::vector<std::string> arena = RaycasterRenderizadorCombate::obterArenaPorTitulo(tituloMapa);
        float jX = static_cast<float>(arena[0].size()) / 2.0f;
        float jY = static_cast<float>(arena.size()) - 2.0f;
        flatTela = Raycaster::desenharQuadroEstatico3D(arena, jX, jY, -1.57f, tituloMapa, jogadorAtual, altura3D);
    }
    
    std::vector<std::string> telaFundo(altura3D);
    for (int y = 0; y < altura3D; ++y) {
        std::string linha = "";
        for (int x = 0; x < larguraConsole; ++x) {
            linha += flatTela[y * larguraConsole + x];
        }
        linha += "\033[0m"; // Garante reset
        telaFundo[y] = linha;
    }

    auto stringParaCharsUtf8 = [](const std::string& str) {
        std::vector<std::string> chars;
        size_t i = 0;
        while (i < str.length()) {
            int charLen = 1;
            unsigned char c = static_cast<unsigned char>(str[i]);
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3;
            else if ((c & 0xF8) == 0xF0) charLen = 4;
            chars.push_back(str.substr(i, charLen));
            i += charLen;
        }
        return chars;
    };

    // Desenha o logo do DERROTA
    int logoY = 2;
    int compVisualLogo = 0;
    for (const auto& linha : ArtesDerrota::logoDerrota) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = TelaBaseMenu::calcularOffsetCentral(compVisualLogo, larguraConsole);
    for (int i = 0; i < (int)ArtesDerrota::logoDerrota.size(); ++i) {
        if (logoY + i < altura3D) {
            telaFundo[logoY + i] = Aparencia::sobreporLogoAnsi(telaFundo[logoY + i], stringParaCharsUtf8(ArtesDerrota::logoDerrota[i]), logoX, "\033[1;38;2;255;50;50m", larguraConsole);
        }
    }

    int startY = logoY + (int)ArtesDerrota::logoDerrota.size() + 2;

    MenuRaycasterUtils::s_fundo3DMenu = telaFundo;

    // Helper function to draw a gray box
    auto desenharCaixaCinza = [&](std::ostringstream& buf, int y, int x, int width, int height) {
        std::string bg = "\033[48;2;25;25;25m"; 
        std::string corBorda = "\033[38;2;255;255;255m"; 
        std::string reset = "\033[0m";
        
        std::string tracos = "";
        for (int i = 0; i < width - 2; ++i) tracos += "═";
        std::string emptyLine = "";
        for (int i = 0; i < width - 2; ++i) emptyLine += " ";
        
        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, bg + corBorda + "╔" + tracos + "╗" + reset, y, x);
        for (int i = 1; i < height - 1; ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buf, bg + corBorda + "║" + bg + emptyLine + corBorda + "║" + reset, y + i, x);
        }
        MenuRaycasterUtils::sobreporTextoAbsoluto(buf, bg + corBorda + "╚" + tracos + "╝" + reset, y + height - 1, x);
    };

    int indexSelecionado = 0;
    bool popupAberto = false;
    bool saindoDoJogo = false; // flag if we chose exit
    
    while (true) {
        std::ostringstream buffer;
        buffer << "\033[?25l\033[H"; 

        for (size_t y = 0; y < MenuRaycasterUtils::s_fundo3DMenu.size(); ++y) {
            buffer << MenuRaycasterUtils::s_fundo3DMenu[y];
            if (y < MenuRaycasterUtils::s_fundo3DMenu.size() - 1) buffer << "\n";
        }

        // Caixa Estatisticas
        std::vector<std::string> boxStats;
        boxStats.push_back("\033[38;2;255;100;100mEstatisticas:\033[0m");
        boxStats.push_back(" Turnos Sobrevividos: \033[38;2;255;255;255m" + std::to_string(turnosCombate) + "\033[0m");
        boxStats.push_back(" Dano Causado: \033[38;2;255;100;100m" + std::to_string(totalDeDanoCausado) + "\033[0m");
        boxStats.push_back(" Dano Recebido: \033[38;2;255;50;50m" + std::to_string(totalDeDanoRecebido) + "\033[0m");

        int boxHeight = boxStats.size() + 2;
        int boxW = 40;
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, larguraConsole);
        
        desenharCaixaCinza(buffer, startY, boxX, boxW, boxHeight);
        
        for(int i = 0; i < (int)boxStats.size(); ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m" + boxStats[i], startY + 1 + i, boxX + 2);
        }

        // Popup Interativo
        int boxPopupW = 50;
        int boxPopupX = TelaBaseMenu::calcularOffsetCentral(boxPopupW, larguraConsole);
        int boxPopupY = startY + boxHeight + 2;

        if (popupAberto) {
            // Confirmacao
            desenharCaixaCinza(buffer, boxPopupY, boxPopupX, boxPopupW, 5);
            std::string msgConfirm = saindoDoJogo ? "Tem certeza que deseja sair?" : "Voltar ao menu principal?";
            int textX = boxPopupX + (boxPopupW - Aparencia::obterComprimentoVisual(msgConfirm)) / 2;
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m\033[38;2;255;100;100m" + msgConfirm, boxPopupY + 1, textX);
            
            std::string msgS = " [S] Sim ";
            std::string msgN = " [N] Nao ";
            int btnY = boxPopupY + 3;
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m\033[38;2;200;200;200m" + msgS, btnY, boxPopupX + 8);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m\033[38;2;200;200;200m" + msgN, btnY, boxPopupX + boxPopupW - 8 - 9);

        } else {
            // Escolha
            desenharCaixaCinza(buffer, boxPopupY, boxPopupX, boxPopupW, 6);
            std::string opt0 = "Voltar ao menu principal";
            std::string opt1 = "Sair do jogo";

            std::string format0 = indexSelecionado == 0 ? "\033[48;2;200;200;200m\033[38;2;0;0;0m> " + opt0 + " <\033[0m" : "\033[48;2;25;25;25m\033[38;2;200;200;200m  " + opt0 + "  \033[0m";
            std::string format1 = indexSelecionado == 1 ? "\033[48;2;200;200;200m\033[38;2;0;0;0m> " + opt1 + " <\033[0m" : "\033[48;2;25;25;25m\033[38;2;200;200;200m  " + opt1 + "  \033[0m";

            int opt0X = boxPopupX + (boxPopupW - (opt0.length() + 4)) / 2;
            int opt1X = boxPopupX + (boxPopupW - (opt1.length() + 4)) / 2;

            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, format0, boxPopupY + 2, opt0X);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, format1, boxPopupY + 4, opt1X);
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (ControleDeInput::teclaPressionada()) {
            unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (popupAberto) {
                if (tecla == 's' || tecla == 'S') {
                    if (saindoDoJogo) {
                        exit(0);
                    } else {
                        return; 
                    }
                } else if (tecla == 'n' || tecla == 'N') {
                    popupAberto = false;
                }
            } else {
                if (tecla == 72 || tecla == 'w' || tecla == 'W') { 
                    indexSelecionado--;
                    if (indexSelecionado < 0) indexSelecionado = 1;
                } else if (tecla == 80 || tecla == 's' || tecla == 'S') { 
                    indexSelecionado++;
                    if (indexSelecionado > 1) indexSelecionado = 0;
                } else if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
                    saindoDoJogo = (indexSelecionado == 1);
                    popupAberto = true;
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}
