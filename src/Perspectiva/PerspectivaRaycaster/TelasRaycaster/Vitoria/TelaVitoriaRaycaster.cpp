#include "TelaVitoriaRaycaster.h"
#include "../Utils/MenuRaycasterUtils.h"
#include "../../EngineRaycaster/Raycaster.h"
#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../TelasBase/Vitoria/TelaVitoriaLayout.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include <sstream>
#include <thread>
#include <chrono>
#include <map>

void TelaVitoriaRaycaster::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
    const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano,
    int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::pair<std::string, int>>& dropsUnicos,
    bool podeSubirNivel, const std::vector<std::string>& novasDescobertas,
    const std::string& tituloMapa)
{
    int larguraConsole = Aparencia::obterLarguraTerminal();
    std::cout << "\033[?25l";
    ControleDeInput::limparBuffer();
    // Agrupar inimigos para exibicao
    std::map<std::string, int> inimigosAgrupados;
    for (const auto& ini : inimigosDerrotados) {
        inimigosAgrupados[ini]++;
    }

    // Gera o fundo 3D estatico da arena do combate
    std::vector<std::string> arena = RaycasterRenderizadorCombate::obterArenaPorTitulo(tituloMapa);
    float jX = static_cast<float>(arena[0].size()) / 2.0f;
    float jY = static_cast<float>(arena.size()) - 2.0f;
    float angulo = -1.57f;
    int altura3D = std::max(10, Aparencia::obterAlturaTerminal());
    
    std::vector<std::string> flatTela = Raycaster::desenharQuadroEstatico3D(arena, jX, jY, angulo, tituloMapa, jogadorAtual, altura3D);
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

    // Desenha o logo do Vitoria
    int logoY = 2;
    int compVisualLogo = 0;
    for (const auto& linha : ArtesVitoria::logoVitoria) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = TelaBaseMenu::calcularOffsetCentral(compVisualLogo, larguraConsole);
    for (int i = 0; i < (int)ArtesVitoria::logoVitoria.size(); ++i) {
        if (logoY + i < altura3D) {
            telaFundo[logoY + i] = Aparencia::sobreporLogoAnsi(telaFundo[logoY + i], stringParaCharsUtf8(ArtesVitoria::logoVitoria[i]), logoX, "\033[1;38;2;50;255;50m", larguraConsole);
        }
    }

    int startY = logoY + (int)ArtesVitoria::logoVitoria.size() + 2;

    MenuRaycasterUtils::s_fundo3DMenu = telaFundo;

    // Helper function to draw a gray box directly into the buffer string stream
    auto desenharCaixaCinza = [&](std::ostringstream& buf, int y, int x, int width, int height) {
        std::string bg = "\033[48;2;25;25;25m"; // Cinza HUD
        std::string corBorda = "\033[38;2;255;255;255m"; // Branco
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

    while (true) {
        std::ostringstream buffer;
        buffer << "\033[?25l\033[H"; // Cursor home LOGO NO INICIO para não rolar o terminal

        for (size_t y = 0; y < MenuRaycasterUtils::s_fundo3DMenu.size(); ++y) {
            buffer << MenuRaycasterUtils::s_fundo3DMenu[y];
            if (y < MenuRaycasterUtils::s_fundo3DMenu.size() - 1) buffer << "\n";
        }

        // Caixa Inimigos
        std::vector<std::string> boxInimigos;
        boxInimigos.push_back("\033[38;2;200;200;200mInimigos Derrotados:\033[0m");
        if (inimigosAgrupados.empty()) boxInimigos.push_back(" \033[38;2;100;100;100mNenhum\033[0m");
        for (auto const& [nome, qtd] : inimigosAgrupados) {
            boxInimigos.push_back(" \033[38;2;255;100;100m" + std::to_string(qtd) + "x " + nome + "\033[0m");
        }

        // Caixa Recompensas separada
        std::vector<std::string> boxDrops;
        boxDrops.push_back("\033[38;2;255;215;0mRecompensas:\033[0m");
        boxDrops.push_back(" \033[38;2;255;215;0mOuro: " + std::to_string(quantidadeDeOuroObtido) + "\033[0m" + "  \033[38;2;0;255;255mXP: " + std::to_string(quantidadeDeXpObtido) + "\033[0m");
        if (!dropsUnicos.empty()) boxDrops.push_back("\033[38;2;200;200;200mItens:\033[0m");
        for (auto const& drop : dropsUnicos) {
            std::string nomeLower = drop.first;
            std::string corRaridade = "\033[38;2;150;150;150m"; // Cinza (Comum)
            if (nomeLower.find("Pocao") != std::string::npos || nomeLower.find("Madeira") != std::string::npos || nomeLower.find("Talisma") != std::string::npos) corRaridade = "\033[38;2;50;255;50m"; // Verde
            if (nomeLower.find("Elixir") != std::string::npos || nomeLower.find("Gosma") != std::string::npos || nomeLower.find("Talisma") != std::string::npos) corRaridade = "\033[38;2;50;150;255m"; // Azul
            if (nomeLower.find("Magica") != std::string::npos || nomeLower.find("Encantado") != std::string::npos) corRaridade = "\033[38;2;200;50;255m"; // Roxo
            if (nomeLower.find("Exterminio") != std::string::npos || nomeLower.find("Cristal") != std::string::npos || nomeLower.find("Cavaleiro") != std::string::npos) corRaridade = "\033[38;2;255;215;0m"; // Ouro

            boxDrops.push_back(" " + corRaridade + std::to_string(drop.second) + "x " + drop.first + "\033[0m");
        }

        // Caixa Estatisticas
        std::vector<std::string> boxStats;
        boxStats.push_back("\033[38;2;150;150;255mEstatisticas de Combate:\033[0m");
        boxStats.push_back(" Turnos: \033[38;2;255;255;255m" + std::to_string(turnosCombate) + "\033[0m");
        boxStats.push_back(" Dano Causado: \033[38;2;255;100;100m" + std::to_string(totalDeDanoCausado) + "\033[0m");
        boxStats.push_back(" Dano Recebido: \033[38;2;255;50;50m" + std::to_string(totalDeDanoRecebido) + "\033[0m");
        boxStats.push_back(" Cura Recebida: \033[38;2;50;255;50m" + std::to_string(curaTotalRecebida) + "\033[0m");
        boxStats.push_back(" Maior Hit: \033[38;2;255;150;0m" + std::to_string(maiorDano) + "\033[0m");
        boxStats.push_back(" Parrys Perfeitos: \033[38;2;0;255;255m" + std::to_string(parriesPerfeitos) + "\033[0m");

        int boxHeight = std::max((int)boxInimigos.size(), (int)boxStats.size());
        
        int totalW = 60;
        int boxX = TelaBaseMenu::calcularOffsetCentral(totalW, larguraConsole);
        desenharCaixaCinza(buffer, startY, boxX, totalW, boxHeight + 4);

        int col1X = boxX + 2;
        int col2X = boxX + 32;

        for(int i = 0; i < (int)boxInimigos.size(); ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m" + boxInimigos[i], startY + 2 + i, col1X);
        }
        for(int i = 0; i < (int)boxStats.size(); ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m" + boxStats[i], startY + 2 + i, col2X);
        }
        
        int boxDropsY = startY + boxHeight + 5;
        int boxDropsW = 50;
        int boxDropsX = TelaBaseMenu::calcularOffsetCentral(boxDropsW, larguraConsole);
        desenharCaixaCinza(buffer, boxDropsY, boxDropsX, boxDropsW, boxDrops.size() + 2);
        
        for(int i = 0; i < (int)boxDrops.size(); ++i) {
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[48;2;25;25;25m" + boxDrops[i], boxDropsY + 1 + i, boxDropsX + 2);
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (ControleDeInput::teclaPressionada()) {
            unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
                break;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
}
