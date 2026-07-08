#include "TelaInventarioRaycaster.h"
#include <iostream>
#include <vector>
#include "../../../../Sistemas/ConfiguracaoTerminal/Aparencia/Aparencia.h"
#include "../../../../Sistemas/ConfiguracaoTerminal/Aparencia/Cor.h"
#include "../../../../Sistemas/Inventario/Item.h"
#include "../../../../Entidades/Personagem.h"
#include "../../../TelasBase/TelaBase.h"
#include "../../../TelasBase/Inventario/TelaInventarioLayout.h"
#include "../../EngineRaycaster/Raycaster.h"
#include "../Utils/MenuRaycasterUtils.h"

void TelaInventarioRaycaster::exibirCabecalho(bool, int startY) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    
    // Desenha o logo do INVENTARIO
    int logoHeight = ArtesInventario::logoInventario.size();
    int logoY = startY > 0 ? (startY - 1 - logoHeight) : 2;
    if (logoY < 0) logoY = 0;
    
    int compVisualLogo = 0;
    for (const auto& linha : ArtesInventario::logoInventario) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;
    
    // Apenas desenha o logo sobre a tela atual, pulando espaços para não pintar fundo preto
    std::string corTitulo = Aparencia::cor(Cor::AMARELO);
    for (int i = 0; i < (int)ArtesInventario::logoInventario.size(); ++i) {
        Aparencia::moverCursor(logoX, logoY + i);
        const std::string& linha = ArtesInventario::logoInventario[i];
        
        std::string buffer = corTitulo;
        int spaceCount = 0;
        
        for (size_t j = 0; j < linha.length(); ) {
            if (linha[j] == ' ') {
                spaceCount++;
                j++;
            } else {
                if (spaceCount > 0) {
                    buffer += "\033[" + std::to_string(spaceCount) + "C";
                    spaceCount = 0;
                }
                
                unsigned char uc = linha[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                
                buffer += linha.substr(j, charLen);
                j += charLen;
            }
        }
        buffer += "\033[0m";
        std::cout << buffer;
    }
    std::cout << std::flush;
}

void TelaInventarioRaycaster::exibirCaixaEquipados(Personagem*) {}
void TelaInventarioRaycaster::exibirDetalheItem(Item*) {}

void TelaInventarioRaycaster::renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) {
    std::vector<std::string> linhasBase = linhas;
    int interactiveIdx = 0;
    
    std::string strBolso = "BOLSO:"; // Helper to identify non-interactive lines if needed
    
    for (size_t i = 0; i < linhasBase.size(); ++i) {
        if (linhasBase[i].empty() || linhasBase[i].find(strBolso) != std::string::npos || linhasBase[i].find("   ") == 0) {
            // Already formatted or empty space
        } else {
            if (interactiveIdx == selecaoAtual) {
                linhasBase[i] = Aparencia::cor(Cor::VERDE) + " > " + linhasBase[i] + Aparencia::cor(Cor::BRANCO) + "\033[48;2;25;25;25m";
            } else {
                linhasBase[i] = "   " + linhasBase[i];
            }
            interactiveIdx++;
        }
    }
    
    // Add extra empty lines if needed to keep box size consistent? 
    // It's better if InventarioCombate passes the exact lines.
    std::vector<std::string> caixaFinal = TelaBase::criarCaixa(linhasBase, titulo, 0, Cor::AMARELO, "\033[48;2;25;25;25m");
    
    if (outH > 0 && outW > 0) {
        Raycaster::restaurarUltimoQuadro();
    }
    
    outW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
    outH = caixaFinal.size();
    
    int larguraTerminal = Aparencia::obterLarguraTerminal();
    int alturaTerminal = Aparencia::obterAlturaTerminal();
    int startX = (larguraTerminal - outW) / 2;
    int startY = (alturaTerminal - outH) / 2;
    
    if (startX < 0) startX = 0;
    if (startY < 8) startY = 8;
    
    std::cout << "\033[?25l";
    for (size_t i = 0; i < caixaFinal.size(); ++i) {
        Aparencia::moverCursor(startX, startY + i);
        std::cout << caixaFinal[i];
    }
    std::cout << std::flush;
}
