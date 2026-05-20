#include "FuncoesDialogo.h"
#include "Aparencia.h"
#include <iostream>

void FuncoesDialogo::imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::string& texto, bool novaLinhaAntes, bool novaLinhaDepois) {
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

void FuncoesDialogo::imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::vector<std::string>& linhas) {
    if (linhas.empty()) return;
    
    // A primeira linha imprime a quebra de linha inicial e o Nome
    imprimirDialogoNPC(npcNome, npcCor, linhas[0], true, true);
    
    // As linhas subsequentes apenas herdam o alinhamento
    for (size_t i = 1; i < linhas.size(); ++i) {
        imprimirDialogoNPC(npcNome, npcCor, linhas[i], false, true);
    }
}

std::string FuncoesDialogo::formatarMsgNarracao(const std::string& texto) {
    return Aparencia::cor(Cor::CINZA) + "[NARRACAO]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgSistema(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[SISTEMA]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgHabilidade(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[HABILIDADE]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgStatus(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[STATUS]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgDrop(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[DROP]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgCombate(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[COMBATE]: " + texto + Aparencia::cor(Cor::RESET);
}

std::string FuncoesDialogo::formatarMsgInteracao(const std::string& texto, Cor corTema) {
    return Aparencia::cor(corTema) + "[INTERACAO]: " + texto + Aparencia::cor(Cor::RESET);
}
